#include "mainwindow.h"
#include "camera.h"
#include "gps.h"
#include "button.h"
#include "./ui_mainwindow.h"
#include <any>
#include <thread>
#include <chrono>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject>
#include <QTimer>  
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Class-Based Window");
    resize(800, 480);
    // showFullScreen();
    _central_widget = new QWidget(this);
    _central_widget->setStyleSheet("background-color: #292828;");
    setCentralWidget(_central_widget);
    _mainlayout = new QVBoxLayout (_central_widget);
    _mainlayout->setContentsMargins(0,0,0,0);
    _mainlayout->setSpacing(0);
    _central_widget->setLayout(_mainlayout);

    StatusBar* stus = new StatusBar(_mainlayout,this);
    stus -> SetUpStatusBar();
    _camera = new Camera(_mainlayout);
    _camera->setUpCamera();    
    // _gscamera =new GSCamera();
    // _gscamera ->start();
    // _mainlayout ->addWidget(_gscamera);
    // _tcamera = new TCamera(_mainlayout);
    // _tcamera->startCamera();
    _gps =new GPS(_mainlayout,this );
    _gps -> setUpGps();
    _map = new MAP(_mainlayout,this);
    _map->setUpMap();
    _buttons = new Buttons(_mainlayout,this);
    _buttons ->setUpButtons();

    _album = new Album(_mainlayout,this);
    
    _currentActFeaPtr =_map;

    MainWindow::setUpConnectionWithButtons();
}
void MainWindow::setUpConnectionWithButtons(){
    connect(_buttons, &Buttons::callCamera, this, [this] { ////If no active feature at all
    if (!_currentActFeaPtr.has_value()) { ///when no active feature 
        _buttons->cameraButtons();
        qDebug() << "click1";
        if (_camera == nullptr) {
            qDebug() << "Camera = null";
            _camera = new Camera(_mainlayout);
        }

        _camera->setUpCamera();
        _camera->StartCamera();

        _currentActFeaPtr = _camera;
        return;
    }

    // If current feature is NOT a Camera
    if (_currentActFeaPtr.type() != typeid(Camera*)) { /// when other feature active
        qDebug() << "Switching from non-camera feature";
        _buttons -> cameraButtons();
        if (_currentActFeaPtr.type() == typeid(MAP*)) {
            _map->hideLabel();
        }
        if (_currentActFeaPtr.type()== typeid(Album*)){
            _album->closeAlbum();
        }
        _gps->hideLabel();
        _currentActFeaPtr.reset();

        if (_camera != nullptr) {
            qDebug() << "Camera != null";
            _camera ->StartCamera();
        }
        else if(_camera == nullptr){
            qDebug() << "Camera is null, creating new instance";
            _camera = new Camera(_mainlayout);
            _camera->setUpCamera();
            _camera->StartCamera();
        }
        _currentActFeaPtr = _camera;
        // _camera->setUpCamera();
        // _camera->StartCamera();
        MainWindow::showFullScreen();
        QThread::sleep(1);
        return;
    }

    // If current feature is a Camera (toggle it off)
    if (_currentActFeaPtr.type() == typeid(Camera*)) { // when camera active 
        _buttons->showNormalButtons();
        qDebug() << "click2 (stopping camera)";
        

        if (_camera) {
            _camera->stopCamera();       // Stop cleanly (disconnects, stops threads)
            // delete _camera;              // Fully destroy object
            // _camera = nullptr;
            qDebug() << "Camera destroyed";
        }

        _gps->showLabel();
        qDebug() << "Showing GPS label";
        _map->showLabel();
        qDebug() << "Showing map label";
        _currentActFeaPtr.reset();
        qDebug() << "Resetting current feature pointer";
        _currentActFeaPtr = _map;
        qDebug() << "Setting current feature to map";
        QThread::sleep(1);
        return;
    }
    

    qDebug() << "Fail at connect with camera!";
    });

    connect (_buttons,&Buttons::callReloadMap,this,[this](){
        _map->reloadMap();
    });
    connect(_buttons, &Buttons::callSnap, this,[this](){
        // _map ->reloadMap();
        // _camera->stopCamera();  
        _camera -> onSnapButtonClicked();
    });
    connect (_buttons, & Buttons::callRecord, this, [this](){
        // _camera->stopCamera();       
        _camera ->timerForVideo();

        // QTimer::singleShot(500, this, [this]() {
        // delete _camera;
        // _camera = nullptr;

        // Schedule re-init after another 500ms (optional)
        // QTimer::singleShot(500, this, [this]() {
        //     _camera = new Camera(_mainlayout);
        //     _camera->setUpCamera();
        //     _camera->StartCamera();
        // });
    // });
    });
    connect (_camera, &Camera::recordFinished,this,[this](){
            qDebug()<<"stop camera";
            _camera->StartCamera();            
            
        });
    
    
    connect(_buttons,&Buttons::callPhotoSetting,this,[this](){
        // if (_camera) {
        //     qDebug() << "Displaying camera settings";
        //     _camera->displayCameraSettingBar();
        // } else {
        //     qDebug() << "Camera not initialized yet!";
        // }
    });
    connect(_buttons,&Buttons::callAlbum, this,[this](){
        bool isAlbumDisplay =_album ->setUpAlbumLabel();
        if(isAlbumDisplay==true){
            _map->hideLabel();
            _gps->hideLabel();  
            _currentActFeaPtr = _album; 
            qDebug()<<"album open";
        }     
        else if(isAlbumDisplay==false){
            _gps->showLabel();
            _map->showLabel();
            _currentActFeaPtr.reset();
            _currentActFeaPtr = _map;
        }
        qDebug()<<"Finished set up album";
        });
}

// void MainWindow::timer(){
// }
// void MainWindow::resetCamera(){
//     // if (_camera) {
//     //     _camera->stopCamera();
//     //     delete _camera;
//     //     _camera = nullptr;
//     // }
//     // _camera = new Camera(_mainlayout);
//     // _camera->setUpCamera(_cameraSetting);
//     // _camera->StartCamera();
// }
MainWindow::~MainWindow()
{
    delete ui;
}
