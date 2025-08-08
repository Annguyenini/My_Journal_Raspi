#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject>  

void StatusBar::option_menu(){
    _optionMenu = new QMenu(this);
    QAction* exit_btn =new QAction("Exit",this);
    connect(exit_btn, &QAction::triggered, this, &QMainWindow::close);
    _optionMenu-> addAction(exit_btn);
}
void StatusBar::SetUpStatusBar(){
    QHBoxLayout* statusbar = new QHBoxLayout;
    QLabel* clock_label= new QLabel();
    clock_label->setObjectName ("Status-Bar");
    clock_label->setStyleSheet("color: white; font-size: 15px; margin: 0; padding: 0;");

    QPushButton* options_btn = new QPushButton ("≡");
    options_btn->setStyleSheet(_btn_properties);
    connect (options_btn, & QPushButton::clicked, this, [this,options_btn](){
        this->option_menu();
        if(_optionMenu){
            _optionMenu->exec(options_btn->mapToGlobal(QPoint(0,options_btn->height())));
        }
    });

    QLabel* battery_percentage = new QLabel("batterry");
    battery_percentage->setObjectName("Battery-percentage");
    battery_percentage->setStyleSheet("color: white; font-size: 15px;");
    
    statusbar->addWidget(options_btn);
    statusbar->addStretch(1);
    statusbar->addWidget(clock_label);
    statusbar->addStretch(1);
    statusbar->addWidget(battery_percentage);
    _mainlayout->insertLayout(0,statusbar);

    
}