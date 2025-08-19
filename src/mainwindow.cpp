#include "mainwindow.h"
#include "camera.h"
#include "gps.h"
#include "button.h"
#include "./ui_mainwindow.h"
#include <filesystem>
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
        std::string currentDir = std::filesystem::current_path().parent_path().string();
        std::string command = currentDir + "/src/camera-app/build/trip";
        int rc = std::system(command.c_str());
        if(rc!=0){
            qDebug() << "Failed to start camera app";
        } else {
            qDebug() << "Camera app started successfully";
        
        }

    qDebug() << "Fail at connect with camera!";
    });

    connect (_buttons,&Buttons::callReloadMap,this,[this](){
        _map->reloadMap();
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
