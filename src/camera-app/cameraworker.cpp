#include "cameraworker.h"
#include <thread>
#include <iostream>
#include <sys/mman.h>
#include <QImage>
#include <QPixmap>

using namespace libcamera;
using namespace std::chrono_literals;

CameraWorker::CameraWorker(QObject *parent) : QObject(parent) {
    cm = std::make_unique<CameraManager>();
    frameCounter = 0;
    frameSkip = 1;
    m_stopping = false;
    _requestPhoto = false;
    allocator = nullptr;
    stream = nullptr;
}

CameraWorker::~CameraWorker() {
    stopCamera();
    if (cm) {
        cm->stop();
    }
}

void CameraWorker::requestComplete(libcamera::Request *request)
{
    if (request->status() != libcamera::Request::RequestComplete || m_stopping || !camera) {
        // Don't requeue if we're stopping or camera is invalid
        if (!m_stopping) {
            request->reuse(libcamera::Request::ReuseBuffers);
            if (camera) {
                camera->queueRequest(request);
            }
        }
        return;
    }

    for (auto &pair : request->buffers()) {
        const auto  currentStream = pair.first;
        const auto &buffer = pair.second;
        
        if (!buffer || buffer->planes().empty()) {
            continue;
        }

        const auto &plane = buffer->planes()[0];
        int fd = plane.fd.get();
        size_t size = plane.length;
        
        // Validate buffer parameters before mapping
        if (fd < 0 || size == 0) {
            std::cerr << "Invalid buffer parameters: fd=" << fd << ", size=" << size << std::endl;
            continue;
        }
        
        void* mem = mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
        if (mem == MAP_FAILED) {
            perror("mmap failed");
            continue;
        }
        
        // Get actual stream configuration
        if (!stream) {
            std::cerr << "Stream is null in requestComplete!" << std::endl;
            munmap(mem, size);
            continue;
        }
        
        // Use actual stream dimensions instead of hardcoded values
        const StreamConfiguration &config = stream->configuration();
        int width = config.size.width;
        int height = config.size.height;
        // Calculate stride based on pixel format
        int stride = width*3; // Assuming RGB32/ARGB32 format
        
        // Validate memory bounds
        size_t expected_size = height * stride;
        if (size < expected_size) {
            std::cerr << "Buffer size mismatch: expected " << expected_size << ", got " << size << std::endl;
            munmap(mem, size);
            continue;
        }
        
        // Create QImage with proper error checking
        QImage image;
        try {
            // Create image from mapped memory
            QImage temp_image(static_cast<const uchar*>(mem), width, height, stride, QImage::Format_BGR888);
            
            // IMPORTANT: Make a deep copy immediately to avoid using dangling pointer
            image = temp_image.copy();
            
            // Unmap immediately after copying
            munmap(mem, size);
            
            if (image.isNull()) {
                std::cerr << "Created QImage is null!" << std::endl;
                continue;
            }
            
            // Emit the copied image
            Q_EMIT newFrameAvailable(image);
            
            // Handle photo capture
            if ( _requestPhoto) {
                if (image.save(QString::fromStdString(_pictureFilename))) {
                    qDebug() << "Picture saved successfully: " << QString::fromStdString(_pictureFilename);
                } else {
                    qDebug() << "Failed to save picture: " << QString::fromStdString(_pictureFilename);
                }
                _requestPhoto = false;
            }
            
        } catch (const std::exception &e) {
            std::cerr << "Exception in image processing: " << e.what() << std::endl;
            munmap(mem, size);
            continue;
        }
        
        break; // Process only first valid buffer
    }
    
    // Requeue the request only if not stopping
    if (!m_stopping && camera) {
        request->reuse(libcamera::Request::ReuseBuffers);
        camera->queueRequest(request);
    }
}

void CameraWorker::startCamera() {
    try {
        // Reset stopping flag
        m_stopping = false;
        
        if (!cm) {
            std::cerr << "CameraManager is null!" << std::endl;
            return;
        }
        
        cm->start();
        auto cameras = cm->cameras();
        if (cameras.empty()) {
            std::cout << "No cameras found!" << std::endl;
            return;
        }
        
        camera = cm->get(cameras[0]->id());
        if (!camera) {
            std::cout << "Failed to get camera!" << std::endl;
            return;
        }
        
        if (camera->acquire() != 0) {
            std::cout << "Failed to acquire camera!" << std::endl;
            return;
        }
        
        auto config = camera->generateConfiguration({ StreamRole::Viewfinder });
        if (!config) {
            std::cout << "Failed to generate configuration!" << std::endl;
            return;
        }
        
        StreamConfiguration &streamConfig = config->at(0);
        streamConfig.size.width = _frameWidth;
        streamConfig.size.height = _frameHeight;
        streamConfig.pixelFormat = libcamera::formats::RGB888; // or RGB888
        ////
        // StreamConfiguration & stillConfig = config->at(1);
        // stillConfig.size.width = _frameWidthStill;
        // stillConfig.size.height = _frameHeightStill;
        // stillConfig.pixelFormat = libcamera::formats::RGB888; // Ensure we're using a compatible pixel format
        
        if (config->validate() == CameraConfiguration::Invalid) {
            std::cout << "Camera configuration invalid!" << std::endl;
            return;
        }
        
        if (camera->configure(config.get()) != 0) {
            std::cout << "Failed to configure camera!" << std::endl;
            return;
        }
        
        std::cout << "Camera pixel format: " << streamConfig.pixelFormat.toString() << std::endl;
        std::cout << "Camera resolution: " << streamConfig.size.width << "x" << streamConfig.size.height << std::endl;
        stream = streamConfig.stream();
        // stillStream = stillConfig.stream();
        if (!stream) {
            std::cerr << "Stream pointer is null!" << std::endl;
            return;
        }
        
        // Clean up any existing allocator
        if (allocator) {
            delete allocator;
        }
        
        allocator = new FrameBufferAllocator(camera);
        if (allocator->allocate(stream) < 0) {
            std::cout << "Failed to allocate buffers!" << std::endl;
            delete allocator;
            allocator = nullptr;
            return;
        }
        
        // Clear any existing requests
        requests.clear();
        
        // Create requests
        for (auto &buf : allocator->buffers(stream)) {
            auto req = camera->createRequest();
            if (!req) {
                std::cout << "Failed to create request!" << std::endl;
                continue;
            }
            
            if (req->addBuffer(stream, buf.get()) != 0) {
                std::cout << "Failed to add buffer to request!" << std::endl;
                continue;
            }
            requests.push_back(std::move(req));
        }
        
        if (requests.empty()) {
            std::cout << "No valid requests created!" << std::endl;
            return;
        }
        
        // Connect signal before starting
        camera->requestCompleted.connect(this, &CameraWorker::requestComplete);
        
        // Set frame rate controls
        camcontrols = std::make_unique<libcamera::ControlList>();
        unsigned int frameDurationUs = 1000000 / _fps; // 30fps = 33333 microseconds per frame
        // 40fps = 25000 microseconds per frame
        // camcontrols->set(libcamera::controls::Brightness, 0.5f);
        camcontrols->set(controls::FrameDurationLimits, libcamera::Span<const std::int64_t, 2>({frameDurationUs, frameDurationUs})); // 30 fps
        if (camera->start(camcontrols.get()) != 0) {
            std::cout << "Failed to start camera!" << std::endl;
            return;
        }
        
        // Queue initial requests
        for (auto &req : requests) {
            if (camera->queueRequest(req.get()) != 0) {
                std::cout << "Failed to queue request!" << std::endl;
            }
        }
        
        std::cout << "Camera started successfully!" << std::endl;
        
    } catch (const std::exception &e) {
        std::cerr << "Exception in CameraWorker::startCamera: " << e.what() << std::endl;
        return; 
    }
}

void CameraWorker::requestPicture(std::string filename) {
    _requestPhoto = true;
    _pictureFilename = filename;
}

void CameraWorker::stopCamera() {
    if (!camera) {
        return;
    }
    
    std::cout << "Stopping camera..." << std::endl;
    
    // Set stopping flag to prevent new requests
    m_stopping = true;
    
    // Disconnect signal handler
    camera->requestCompleted.disconnect(this, &CameraWorker::requestComplete);
    
    // Stop camera
    camera->stop();
    
    // Clean up allocator
    if (allocator && stream) {
        allocator->free(stream);
        delete allocator;
        allocator = nullptr;
    }
    
    // Clear requests
    requests.clear();
    
    // Release camera
    camera->release();
    camera.reset();
    stream = nullptr;
    
    std::cout << "Camera stopped successfully" << std::endl;
}

#include "cameraworker.moc"