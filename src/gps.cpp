#include "include/gps.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <fstream>
#include <filesystem>
#include <sqlite3.h>
GPS::GPS(){
    _serial =new QserialPort(this);
    try{
        _serial->setPortName("/dev/serial0");
        _serial->setBauRate(QSerialPort::Baud9600);
        

    }
    catch(const std::exception &e ){
        qDebug()<<"Serial error:"<<e.what();
    }
    if initPath();
}
bool GPS:initPath(){
    if (!filesystem::exist(_databaseDir)){
        try{
        qDebug()<<"Path doesnt exist..\n Attempting to create...\n";
        if(!filesystem::create_directories(_databaseDir)){
            throw std::runtime_error("Fail to create database folder.\n");
        }
       }
       catch (std::exception &e){
        qDebug()<<"Database eror:" <<e.what();
        return false;
       }

    }
    if(!filesystem::exist(_dbPath)){
        try{
            qDebug()<<"Db path does exist..\nAttrmpting to create...\n";
            std::ofstream db (_dbPath)
            if(!db){
                throw std::runtime_error("Fail to create gps db file.'n");
            }
            
            qDebug()<<"Done creating>>"
            db.close()
            
        }
        catch(const std::exception &e){
            qDebug()<<"Database erorr:"<<e.what();
            return false;
        }
    }
    return true;

}
void setUpDB(){
    try{
        int rc = sqlite3_open(_dbPath,&_db);
        char* errMsg = nullptr;
        if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return 1;
        }
        const char* createTableSQL="CREATE TABLE IF NOT EXISTS log (city TEXT, lat REAL, lng REAL, time TEXT )";
        rc = sqlite3_exec(_db, createTableSQL,nullptr,nullptr,&errMsg);
        if(rc != SQLITE_OK){
            qDebug()<<"SQL error (INSERT): "<<errMsg<<std::endl;
            sqlite3_free(errMsg);
        }
    }
    catch (const std::exception &e){
        qDebug()<<"Faild to create table: "<<e.what();
    }
}
vodi addingToCache(std::string &time, const gpsMetadata & data){
    std::lock_guard<std::mutex> lock(cacheMutex);
    _cache[time] = data;

}
void GPS::startReadingFromGps(){

    try{
        if(!_serial->open(QIODevice::ReadOnly)){
            throw std::runtime_error("Failed to open serial port!")
        } // open to read form port
    }
    catch (const std::exception &e){
        qDebug()<<"Serial error"<<e.what();
    }
    connect(_serial, &QSerialPort::readyRead,this ,[&](){
        QByteArray data = _serial->readAll();
        QString line = QString::fromUtf8(data);
        QStringList parts =line.split(',');
        if(parts[0] == "$GPRMC" && part.size()>5){
            QString lat= part[3];
            QString lng= part[5];
            
        }
        else(
            qDebug()<<"Fail to read data or there is no good data!"
        )
    });
}
