#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject>  


StatusBar::StatusBar(QVBoxLayout* mainlayout, QObject* parent): QObject(parent),_mainlayout(mainlayout)
,_statusbar(nullptr),_batteryPer(nullptr),_optionBtn(nullptr),_clockLabel(nullptr),_exitBtn(nullptr),_optionMenu(nullptr){};


void StatusBar::option_menu(){
    _optionMenu = new QMenu();
    _optionMenu->setStyleSheet(
    "QMenu {"
    "  background-color: #222;"
    "  color: #fff;"
    "  border: 1px solid #555;"
    "}"
    "QMenu::item {"
    "  padding: 10px 30px;"        // ↑ increase padding for bigger click area
    "  min-height: 40px;"           // ↑ ensure items are taller
    "  font-size: 16px;"  
    "}"
    "QMenu::item:selected {"
    "  background-color: #555;"
    "}"
);
    _exitBtn =new QAction("Exit",this);
    connect(_exitBtn, &QAction::triggered, this, [this](){
        QWidget * w = qobject_cast<QWidget*>(parent());
        if (w){
            w->close();
        }
    });
    _optionMenu-> addAction(_exitBtn);
}
void StatusBar::SetUpStatusBar(){
    _statusbar = new QHBoxLayout;
    _clockLabel= new QLabel();
    _clockLabel->setObjectName ("Status-Bar");
    _clockLabel->setStyleSheet("color: white; font-size: 15px; margin: 0; padding: 0;");

    _optionBtn = new QPushButton ("≡");
    _optionBtn->setStyleSheet(_btn_properties);
    connect (_optionBtn, & QPushButton::clicked, this, [this](){
        this->option_menu();
        if(_optionMenu){
            _optionMenu->exec(_optionBtn->mapToGlobal(QPoint(0,_optionBtn->height())));
        }
    });

    _batteryPer = new QLabel("batterry");
    _batteryPer->setObjectName("Battery-percentage");
    _batteryPer->setStyleSheet("color: white; font-size: 15px;");
    
    _statusbar->addWidget(_optionBtn);
    _statusbar->addStretch(1);
    _statusbar->addWidget(_clockLabel);
    _statusbar->addStretch(1);
    _statusbar->addWidget(_batteryPer);
    _mainlayout->insertLayout(0,_statusbar);

    
}
// void StatusBar::closeApplication(){
//     if (QWidget* widgetParent = qobject_cast<QWidget*>(parent())) {
//         widgetParent->close(); // this will close your main window safely
//     }
// }
StatusBar::~StatusBar(){
    delete _statusbar;
}