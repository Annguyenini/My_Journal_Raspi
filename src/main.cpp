#include "mainwindow.h"
#include "camera.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StatusBar s;
    
    s.SetUpStatusBar();
   
    return a.exec();
}
