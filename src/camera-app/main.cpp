#include "camera.h"
#include "gps.h"

#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // QSerialPort serial;

    camera w;
    // StatusBar s;
    // s.SetUpStatusBar();
    // GPS g;
    // g.show();
    // GPSWorker*gps =new GPSWorker();
    // gps->startReadingFromGps();
    w.show();
    w.showFullScreen();
    return a.exec();
}
