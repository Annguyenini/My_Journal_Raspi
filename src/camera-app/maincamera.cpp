#include "camera.h"
#include "gps.h"
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <QObject>
#include <QImage>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QPixmap>
#include <QPushButton>
#include <QCoreApplication>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
using namespace cv;
using namespace std;
// Libcam2OpenCV Camera::camera;
Camera::Camera(QWidget *parent) 
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Camera Window");
    resize(800, 480);

    _central_widget = new QWidget(this);
    _central_widget->setStyleSheet("background-color: #292828;");
    setCentralWidget(_central_widget);
    _mainlayout = new QVBoxLayout (_central_widget);
    _mainlayout->setContentsMargins(0,0,0,0);
    _mainlayout->setSpacing(0);
    _central_widget->setLayout(_mainlayout);
    this->setUpCamera();
    this->setUpButtons();
    this->initDB();    
    this->startCamera();
}

void Camera::setUpCamera(){
    qDebug() << "Setting up camera with settings:";
    // _gpsObject = new GPSWorker();
    _CameraLabel = new QLabel();
    _albumObject = new AlbumWorker();
    _camera =new CameraWorker();
    qDebug()<<"pass";
    _CameraLabel->setObjectName("Camera-label");
    // _CameraLabel->setStyleSheet("background-color: red;");

    _CameraLabel->setMinimumSize(200, 200);
    // _CameraLabel->setMinimumSize(800, 300);  // set preferred size
    _CameraLabel->setAlignment(Qt::AlignCenter);
    // _CameraLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // _mainlayout->insertWidget(1, _CameraLabel,1);
    _settings = new QSettings(QString::fromStdString((_parentDir / "Configure.ini").string()), QSettings::IniFormat);

    _parentdir = _settings->value("Database/gallery").toString().toStdString();
    _galleryPath = parentdir / settings->value("Database/galleryPath").toString().toStdString();
    __albumDB = _settings->value("Database/albumsDB").toString().toStdString();
    
}


void Camera::setUpButtons(){
    _buttonsLayout = new QHBoxLayout();
    _snapPictureBtn =new QPushButton("Snap");
    _recordBtn =new QPushButton("Record");
    _cameraBtn =new QPushButton("Exit Camera");
    for (QPushButton* btn :{_snapPictureBtn,_recordBtn,_cameraBtn}){
        btn->setStyleSheet(_btn_properties);
        _buttonsLayout->addWidget(btn);
    }
    connect(_snapPictureBtn, &QPushButton::clicked, this, [this](){
        onSnapButtonClicked();
    });
    connect(_recordBtn, &QPushButton::clicked, this, [this](){
        qDebug() << "Recording video...";
        Camera::timerForVideo();
    });
    connect(_cameraBtn, &QPushButton::clicked, this, [this](){
        qDebug() << "Exiting camera mode...";
        this->stopCamera();
        Camera::close();
    });
    _mainlayout->addLayout(_buttonsLayout);
        

}
static QMutex g_camMutex;

void Camera::StartCamera() {
    QMutexLocker lock(&g_camMutex);

    if (_isStarted) return;
    if (!_CameraLabel) return;


  

    // Make sure no stale connections

    connect(_camera, &CameraWorker::newFrameAvailable, this, [this](const QImage &image) {
        QPixmap pixmap = QPixmap::fromImage(image);
        if (!_CameraLabel) return;
        _CameraLabel->setPixmap(pixmap.scaled(_CameraLabel->size(), Qt::KeepAspectRatio));
    });


    qDebug() << "Starting camera...";
    _camera->startCamera();     // assume returns quickly; actual HW start may be async

    // Show UI only after start is requested
    if (_mainlayout && _CameraLabel && _CameraLabel->parent() == nullptr)
        _mainlayout->insertWidget(1, _CameraLabel, 1);
    if (_CameraLabel) _CameraLabel->show();

    _isStarted  = true;
}

void Camera::stopCamera() {
    QMutexLocker lock(&g_camMutex);

    if (!_isStarted) return;


    

    // 2) Hide UI
    if (_CameraLabel) _CameraLabel->hide();
    if (_mainlayout && _CameraLabel) _mainlayout->removeWidget(_CameraLabel);

    // 3) Ask camera to stop
    if (_camera) {
        qDebug() << "Stopping camera...";
        _camera->stopCamera();
    }

    
    _isStarted  = false;
    sqlite3_close(_db);
    qDebug() << "Camera stopped/reset cleanly";
}


bool Camera::takePicture() {
    std::string filename = _galleryPath+"/"+getCurrentTime()+".jpg";
        qDebug()<<QString::fromStdString(filename);
        _camera->requestPicture(filename);
        this -> insertToDB(filename,"photo");
        return true;
}
void Camera::onSnapButtonClicked() {
    if (takePicture()) {
        qDebug() << "Picture saved!";
    } else {
        qDebug() << "No frame to save!";
    }
}
std::string Camera::getCurrentTime(){
    QDateTime current = QDateTime::currentDateTime();
    QString formattedTime = current.toString("yyyy-MM-dd_hh-mm-ss");
    return formattedTime.toStdString();
}
void Camera::startRpiCamHello(const int& duration) {
    _camera->stopCamera();
    Camera::hideTimer();
    std::string filename = _galleryPath+"/"+getCurrentTime();
    std::string filenameH264 = filename+".h264";
    std::string filenameMP4 = filename+".mp4";
    std::string recordCommand = "rpicam-vid --width 1920 --height 1080 -t "+std::to_string(duration)+" -o "+filenameH264;
    // std::string recordCommand = "rpicam-vid --width 1920 --height 1080 --framerate 60 -t 5000 -o "+filename+".mp4 --inline --container mp4";
    std::cout << "Running: " << recordCommand << std::endl;
    int ret = std::system(recordCommand.c_str());
    if (ret != 0) {
        std::cerr << "rpicam-hello failed with code " << ret << std::endl;
    }
    std::string convertCommand ="ffmpeg -i "+ filenameH264 +" -c copy "+ filenameMP4;
    int conv = std::system(convertCommand.c_str());
    if (conv != 0){
        std::cerr << "Convert failed: " << ret << std::endl;

    }
    std::string removeCommand = "rm -rf "+filenameH264;
    int rm = std::system(removeCommand.c_str());
    if(rm !=0 ){
        std::cerr << "rm fail" << ret << std::endl;

    }
    this->hideTimer();
    _albumObject -> insertToDB(filenameMP4,"video");
    // Q_EMIT recordFinished();
    _camera->startCamera();  // Restart camera after recording
    Camera::hideTimer();
    qDebug() << "Recording finished, camera restarted";
}   
void Camera::timerForVideo(){
    _recordTimers = new QHBoxLayout();
    fiveTimer = new QPushButton("5");
    tenTimer = new QPushButton("10");
    fifteenTimer = new QPushButton("15");
    twentyTimer = new QPushButton("20");
    for (QPushButton* btn : {fiveTimer,tenTimer,fifteenTimer,twentyTimer}){
        btn ->setStyleSheet(_btn_properties);
        _recordTimers->addWidget(btn);
        
    }
    connect(fiveTimer, &QPushButton::clicked,this,[this](){
            qDebug()<<"callde";
            Camera::startRpiCamHello(5*1000);
        });
    connect(tenTimer, &QPushButton::clicked,this,[this](){
            qDebug()<<"callde";
            Camera::startRpiCamHello(10*1000);
        });

    connect(fifteenTimer, &QPushButton::clicked,this,[this](){
            qDebug()<<"callde";
            Camera::startRpiCamHello(15*1000);
        });
    connect(twentyTimer, &QPushButton::clicked,this,[this](){
            qDebug()<<"callde";
            Camera::startRpiCamHello(20*1000);
        });
    Camera::showTimer();
}
void Camera::showTimer(){
    if (!_recordTimers) return;
        for (int i = 0; i < _recordTimers->count(); ++i) {
            QWidget* w = _recordTimers->itemAt(i)->widget();
            if (w) w->show();
        }
    _mainlayout->insertLayout(2,_recordTimers);
    
}
void Camera::hideTimer(){
    
    if (!_recordTimers) return;
        for (int i = 0; i < _recordTimers->count(); ++i) {
            QWidget* w = _recordTimers->itemAt(i)->widget();
            if (w) w->hide();
        }
    _mainlayout->removeItem(_recordTimers);
}



void Camera::initDB(){
    int rc = sqlite3_open(_albumDB.string().c_str(),&_db);
    
    char* errMsg = nullptr;
    if (rc != SQLITE_OK){
        qDebug()<<"Cant open db";
        return;
    }
    const char*  createTableSQL = "CREATE TABLE IF NOT EXISTS album (path TEXT, time TEXT, type TEXT)";
    rc = sqlite3_exec(_db, createTableSQL, nullptr,nullptr,&errMsg);

    if (rc!= SQLITE_OK){
        qDebug()<<"Fail to create the table";
    }
    if (sqlite3_exec(_db, "PRAGMA journal_mode = WAL;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to enable WAL: " << errMsg << "\n";
        sqlite3_free(errMsg);
    } else {
        std::cout << "WAL mode enabled!\n";
    }


}
void Camera::insertToDB(const std::string& path,std::string type){
    std::string time = this->getCurrentTime();
    const char* insertToDB = "INSERT INTO Album (time,path,type) VALUES (?,?,?)";
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(_db,insertToDB,-1,&stmt,nullptr)!= SQLITE_OK){
        qDebug()<<"Failed to prepare statement: " << sqlite3_errmsg(_db) << "\n";
        return;
    }
    sqlite3_bind_text (stmt, 1, time.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 2, path.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 3, type.c_str(),-1,SQLITE_TRANSIENT);
  
     if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(_db) << "\n";
    } else {
        std::cout << "Insert successful!\n";
    }

    sqlite3_finalize(stmt);
    // sqlite3_close(_db);

    return;
}
//
Camera::~Camera() {
    // delete myCallback;
}





