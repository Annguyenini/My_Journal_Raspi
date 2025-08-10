#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>  // if you use layouts
#include <QLabel>
#include <QPoint>       // for QPoint stuff
#include <QWidget>      // base widget stuff
#include <QObject>     
#include <QString> 
#include <QWebEngineView>
#include <QUrl>
#include <string>
#include "button.h"
#include "camera.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


class MAP: public QObject{
    Q_OBJECT
    private:
        QVBoxLayout* _mainlayout;
        QWebEngineView* _maplabel;
        
    public:
        MAP(QVBoxLayout* mainlayout,QObject* parent);
        ~MAP();
        void setUpMap();
        void reloadMap();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:  
    
    QVBoxLayout* _mainlayout;
     QString _btn_properties = R"(QPushButton {
                background-color: #000000;
                color: white;
                font-size: 20px;
                padding: 10px 20px;
                border: none;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #005f99;
})";

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setUpConnectionWithButtons();
private:
    Ui::MainWindow *ui;
    QWidget* _central_widget;
    Camera* _camera;
    Buttons* _buttons;
    MAP* _map;
    auto* _currentActFeaPtr;
};

class StatusBar: public QObject{
    Q_OBJECT
    private:
        QVBoxLayout* _mainlayout;
        QHBoxLayout* _statusbar;
        QLabel* _batteryPer;
        QPushButton* _optionBtn;
        QMenu* _optionMenu;
        QLabel* _clockLabel;
        QAction* _exitBtn;
        QString _btn_properties = R"(QPushButton {
                background-color: #000000;
                color: white;
                font-size: 20px;
                padding: 10px 20px;
                border: none;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #005f99;
        })";
    public:
        explicit StatusBar(QVBoxLayout* mainlayoutn, QObject* parent =nullptr);
        ~StatusBar();
        void option_menu();
        void SetUpStatusBar();
    }; 



#endif // MAINWINDOW_H
