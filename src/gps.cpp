#include "include/gps.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <fstream>
#include <filesystem>
#include <sqlite3.h>
#include <ctime>
#include "include/json.hpp"

using json = nlohmann::json;
namespace bg = boost::geometry;
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
    if(!filesystem::exist(_geoPolygonPath)){
        qDebug()<<"Geo file not found\n";
        return false;
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
        const char* createTableSQL="CREATE TABLE IF NOT EXISTS log (time TEXT, city TEXT, lat REAL, lng REAL )";
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

std::string GPS::getCurrentTime(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    std::ostringstream oss;
    oss << std::put_time(now_tm,"%H:%M:%S");
    return oss.str();
}

void addingToCache(std::string &time, const gpsMetadata & data){
    std::lock_guard<std::mutex> lock(cacheMutex);
    _cache[time] = data;
    if(_cache.size()>=10){
        sqlite3_exec(_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        const char* sql ="INSERT INTO LOG (time,city, lat, lng) VALUES(?,?,?,?) ";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(_db) << "\n";
            return;
        }
        for (const auto &[time,data]: _cache){
            sqlite3_bind_text (stmt,1,time.c_str(),-1,SQLITE_TRANSIENT);
            sqlite3_bind_text (stmt,2,data.city.c_str(),-1,SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 3, data.lat);
            sqlite3_bind_double(stmt, 4, data.lng);
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Insert failed: " << sqlite3_errmsg(_db) << "\n";
            }

            sqlite3_reset(stmt);             // Reuse statement
            sqlite3_clear_bindings(stmt);    // Clear previous bindings
        }

        sqlite3_finalize(stmt);
        sqlite3_exec(_db, "COMMIT;", nullptr, nullptr, nullptr);

        _cache.clear(); // Clear cache after flushing to DB
    }
    
}

void GPS::startReadingFromGps(){

    try{
        if(!_serial->open(QIODevice::ReadOnly)){
            throw std::runtime_error("Failed to open serial port!");
        } // open to read form port
    }
    catch (const std::exception &e){
        qDebug()<<"Serial error"<<e.what();
    }
    connect(_serial, &QSerialPort::readyRead,this ,[&](){
        std::time_t now = std::time(nullptr);
        
        QByteArray data = _serial->readAll();
        QString line = QString::fromUtf8(data);
        QStringList parts =line.split(',');
        switch(parts[0]){
            case "$GPRMC":
                float lat= parts[3].toFloat();
                float lng= parts[5].toFloat();
                _gpsMetadataStruct data {
                    _currentCity,
                    lat.toFloat(),
                    lng.toFloat()
                };
                this -> addingToCache(this->getCurrentTime(),data);
                break;
            case "$GPVTG":
                _speed=parts[7].toFloat();
                break;
            default:
                qDebug()<<"Fail to read data or there is no good data!"
        }
        
    });
}

void GPS::getCurrentCity(){
    json vnGeo;
    std::ofstream geoOutput(_geoPolygonPath);
    geoOutput>>vnGeo;
    for(const auto &features : vnGeo["features"] ){
        Polygon poly;
        for (const autu& pair: features["geometry"]["coordinates"]){
            float lng = pair[0];
            float lat = pair[1];
            bg::append(poly.outer(),Point(lng,lat));
        }

        if (bg::within(point, poly)||bg::insterect(point,poly)){
                _currentCity=features["properties"].value("name","Unkown City");
                return features["properties".value("real_name","Unkown City")];
            }
    }
    return "Unkown City"
}





void GPS::speedMonitor(){
    while true{
        std::lock_guard<std::mutex> lock(cacheMutex);
        float lat = _lat;
        float lng =_lng;
        float speed= _speed;
        _currentRealnamecity = this->getCurrentCity();

        if (speed >=40){
            std::this_thread::sleep_for(std::chrono::seconds(30));
        }
        else if (speed >=20){
            std::this_thread::sleep_for(std::chrono::seconds(40));
        }
        else if (speed <20){
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    }   

}