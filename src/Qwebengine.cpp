#include <QWebEngineView>
#include <QUrl>
#include "mainwindow.h"

MAP::MAP(QVBoxLayout* mainlayout,QObject* parent): _mainlayout(mainlayout), QObject(parent),_maplabel(nullptr){};

void MAP::setUpMap(){
    _maplabel =new QWebEngineView();
    _maplabel->setObjectName("Map-Label");
    _maplabel->load(QUrl("http://192.168.0.52:5000"));
    _maplabel->resize(200,200);
    _mainlayout->insertWidget(3,_maplabel);

}
void MAP::reloadMap(){
    _maplabel->reload();
}
void MAP::displayMap(){
    if(!_isDisplay){
        _maplabel->show();
        _isDisplay =true;
    }
    else if (_isDisplay){
        _maplabel->hide();
        _isDisplay = false;
    }
}

MAP::~MAP(){
    delete _maplabel;
}
// #include <QWebEngineView>
// #include <QUrl>
// #include <QVBoxLayout>
// #include <QLabel>
// #include <QPixmap>
// #include <QDebug>
// #include "mainwindow.h"

// MAP::MAP(QVBoxLayout* mainlayout, QObject* parent)
//     : _mainlayout(mainlayout), QObject(parent), _maplabel(nullptr), _mainLabel(nullptr),
//       _mainBackgroundLabel(nullptr), _isMap(false), _isDisplay(false) {}

// void MAP::setUpMap() {
//     // Use a concrete layout
//     _mainLabel = new QVBoxLayout();  

//     _maplabel = new QWebEngineView();
//     _maplabel->setObjectName("Map-Label");
//     _maplabel->load(QUrl("http://192.168.0.52:5000"));
//     _maplabel->resize(200, 200);

//     _mainBackgroundLabel = new QLabel();
//     QPixmap pix(QString::fromStdString(_mainbackgroundPath.string()));
//     _mainBackgroundLabel->setPixmap(pix.scaled(800, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//     _mainBackgroundLabel->setFixedSize(800, 200);
//     _mainBackgroundLabel->setAlignment(Qt::AlignCenter);

//     qDebug() << QString::fromStdString(_mainbackgroundPath.string());

//     // Add background label to the internal layout
//     _mainLabel->addWidget(_mainBackgroundLabel);

//     // Insert this layout into the main layout
//     _mainlayout->insertLayout(3, _mainLabel);
// }

// void MAP::reloadMap() {
//     _maplabel->reload();
// }

// void MAP::switchBackground() {
//     if (!_isMap) {
//         _mainLabel->removeWidget(_mainBackgroundLabel);
//         _mainBackgroundLabel->hide();
//         _mainLabel->addWidget(_maplabel);

//         _isMap = true;
//     } else {
//         _mainBackgroundLabel->show();
//         _mainLabel->removeWidget(_maplabel);
//         _mainLabel->addWidget(_mainBackgroundLabel);
//         _isMap = false;
//     }
// }

// void MAP::displayMap() {
//     if (!_isDisplay) {
//         qDebug() << "SHOWING";
//         _mainlayout->insertLayout(3, _mainLabel);
//         _isDisplay = true;
//     } else {
//         qDebug() << "HIDING";
//         _mainlayout->removeItem(_mainLabel);
//         _isDisplay = false;
//     }
// }

// MAP::~MAP() {
//     delete _maplabel;
//     // _mainLabel and _mainBackgroundLabel will be deleted by the parent layout
// }
