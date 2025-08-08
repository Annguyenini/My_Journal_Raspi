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
#include <string>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:  
    
    QVBoxLayout* _mainlayout;
    QMenu* _optionMenu;
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
private:
    Ui::MainWindow *ui;
    QWidget* _central_widget;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
};
class StatusBar : public MainWindow{
public:
    void option_menu();
    void SetUpStatusBar();

}; 
#endif // MAINWINDOW_H
