#include "include/gps.h"
#include <QSerialPort>
#include <QSerialPortInfo>
GPS::GPS(){
    _serial =new QserialPort(this);
    _serial->setPortName("/dev/serial0");
    _serial->setBauRate(QSerialPort::Baud9600);
    
}
void GPS::startReadingFromGps(){
    _serial->open(QIODevice::ReadOnly);
    connect(_serial, &QSerialPort::readyRead,this ,[&](){
        QByteArray data = _serial->readAll();
        QString line = QString::fromUtf8(data)
        QStringList parts =line.split(',');
        if(parts[0] == "$GPRMC"){
            QString lat= part[3];
            QString lng= part[5];
        }
    });;
}