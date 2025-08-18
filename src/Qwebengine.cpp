#include <QWebEngineView>
#include <QUrl>
#include "mainwindow.h"

MAP::MAP(QVBoxLayout* mainlayout,QObject* parent): _mainlayout(mainlayout), QObject(parent),_maplabel(nullptr){};

void MAP::setUpMap(){
    _maplabel =new QWebEngineView();
    _maplabel->setObjectName("Map-Label");
    _maplabel->load(QUrl("http://192.168.0.52:5000"));
    _maplabel->resize(200,200);
    this -> showLabel();
}
void MAP::reloadMap(){
    _maplabel->reload();
}
void MAP::hideLabel(){
    _maplabel->hide();
    _mainlayout->removeWidget(_maplabel);
}
void MAP::showLabel(){
    _mainlayout->insertWidget(3,_maplabel);
    _maplabel->show();
}
MAP::~MAP(){
    delete _maplabel;
}