#include "mainwindow.h"
#include "camera.h"
#include "gps.h"
#include "button.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject>
#include <QTimer>  

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Class-Based Window");
    resize(480, 800);
    showFullScreen();
    _central_widget = new QWidget(this);
    _central_widget->setStyleSheet("background-color: #292828;");
    setCentralWidget(_central_widget);
    _mainlayout = new QVBoxLayout (_central_widget);
    _mainlayout->setContentsMargins(0,0,0,0);
    _mainlayout->setSpacing(0);
    _central_widget->setLayout(_mainlayout);
    _camera = new Camera(_mainlayout);
    // camera->StartCamera();
    StatusBar* stus = new StatusBar(_mainlayout,this);
    stus -> SetUpStatusBar();
    GPS* gps =new GPS(_mainlayout,this );
    gps -> setUpGps();
    _map = new MAP(_mainlayout,this);
    _map->setUpMap();
    _buttons = new Buttons(_mainlayout,this);
    _buttons ->setUpButtons();

    // GPSWorker*gps =new GPSWorker();
    // gps->startReadingFromGps();
    // gps.startThread();
    // show();
    MainWindow::setUpConnectionWithButtons();
}

void MainWindow::setUpConnectionWithButtons(){
    connect(_buttons, &Buttons::callCamera, this,[this]{
        if (_currentActFeaPtr !=nullptr){
            _currentActFeaPtr->closeFea();
            _currentActFeaPtr =nullptr;

            };

        _camera->setUpCamera();
        _camera->StartCamera();
        _currentActFeaPtr =_camera
    });
    connect(_button, &Buttons::callReloadMap, this,[this](){
        _maplabel ->reloadMap();
    });
    
}
// void MainWindow::timer(){
// }
MainWindow::~MainWindow()
{
    delete ui;
}
