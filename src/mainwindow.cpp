#include "mainwindow.h"
#include "camera.h"
#include "gps.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject>  

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Class-Based Window");
    resize(480, 800);
    // showFullScreen();
    _central_widget = new QWidget(this);
    _central_widget->setStyleSheet("background-color: #292828;");
    setCentralWidget(_central_widget);
    _mainlayout = new QVBoxLayout (_central_widget);
    _mainlayout->setContentsMargins(0,0,0,0);
    _mainlayout->setSpacing(0);
    _central_widget->setLayout(_mainlayout);
    // Camera* camera = new Camera(_mainlayout);
    // camera->StartCamera();
    GPS gps;
    gps.startThread();
    show();
}

// void MainWindow::timer(){
// }
MainWindow::~MainWindow()
{
    delete ui;
}
