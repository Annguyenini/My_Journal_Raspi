#include "gps.h"
#include <iostream>
#include <thread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>

#include <fstream>
#include <filesystem>
#include <sqlite3.h>
#include <ctime>
#include "json.hpp"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
using Point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;

// Define your polygon type
using Polygon = boost::geometry::model::polygon<Point>;
using json = nlohmann::json;
namespace bg = boost::geometry;
std::unordered_map<std::string, GPSWorker::_gpsMetadataStruct> GPSWorker::_cache;
std::mutex GPSWorker::cacheMutex;
GPSWorker::GPSWorker(){
    _parentDir = std::filesystem::current_path().parent_path();
    _settings = nullptr;
    // Delay all QObject creation to initialPort()
    // Paths will be set in initialPort()
}
void GPSWorker::initialPort(){
    // Create QSettings and QSerialPort in the correct thread
    if (!_settings) {
        _settings = new QSettings(QString::fromStdString((_parentDir / "Configure.ini").string()), QSettings::IniFormat);
        _databaseDir = _parentDir / _settings->value("Database/path").toString().toStdString();
        _dbPath = _parentDir /_settings->value("Database/gpsPath").toString().toStdString();
        _geoPolygonPath = _parentDir / _settings->value("Database/geoPolygonPath").toString().toStdString();
        qDebug() << QString::fromStdString(_databaseDir.string());
        qDebug() << QString::fromStdString(_dbPath.string());
        qDebug() << QString::fromStdString(_geoPolygonPath.string());
        if (!GPSWorker::initPath()){
            qDebug()<<"Error initial paths!";
            return;
        }
    }
    
    GPSWorker::setUpDB();
    _serial = new QSerialPort();
    
    try{
        _serial->setPortName("/dev/serial0");
        _serial->setBaudRate(QSerialPort::Baud9600);
    }
    catch(const std::exception &e ){
        qDebug()<<"Serial error:"<<e.what();
    }
}
bool GPSWorker::initPath(){
    if (!std::filesystem::exists(_databaseDir.string())){
        try{
        qDebug()<<"Path doesnt exist..\n Attempt to create...\n";
        if(!std::filesystem::create_directories(_databaseDir)){
            throw std::runtime_error("Fail to create database folder.\n");
        }
       }
       catch (std::exception &e){
        qDebug()<<"Database eror:" <<e.what();
        return false;
       }

    }
    if(!std::filesystem::exists(_dbPath.string())){
        try{
            qDebug()<<"Db path doesn't exist..\nAttempt to create...\n";
            std::ofstream db (_dbPath.string());
            if(!db){
                throw std::runtime_error("Fail to create gps db file.'n");
            }
            
            qDebug()<<"Done creating>>";
            db.close();
            
        }
        catch(const std::exception &e){
            qDebug()<<"Database erorr:"<<e.what();
            return false;
        }
    }
    if(!std::filesystem::exists(_geoPolygonPath)){
        qDebug()<<"Geo file not found\n";
        return false;
    }
    return true;

}

void GPSWorker::setUpDB(){
    try{
        int rc = sqlite3_open(_dbPath.string().c_str(),&_db);
        qDebug() << "Is _db nullptr? " << (_db == nullptr);

        char* errMsg = nullptr;
        qDebug()<<"pass1";
        if (rc!= SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(_db) << std::endl;
        return;
        }
        qDebug()<<"pass22";
        const char* createTableSQL="CREATE TABLE IF NOT EXISTS log (time TEXT, city TEXT, lat REAL, lng REAL )";
        qDebug() << "SQL: " << createTableSQL;
        qDebug() << "sqlite3_db_errmsg: " << sqlite3_errmsg(_db);
        rc = sqlite3_exec(_db, createTableSQL,nullptr,nullptr,&errMsg);
        qDebug() << "sqlite3_exec rc:" << rc;
        qDebug()<<"pass222";
        if(rc != SQLITE_OK){
            qDebug()<<"SQL error (INSERT): "<<errMsg;
            sqlite3_free(errMsg);
        }
    }
    catch (const std::exception &e){
        qDebug()<<"Faild to create table: "<<e.what();
    }
    qDebug()<<"pass2";
}

std::string GPSWorker::getCurrentTime(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    std::ostringstream oss;
    oss << std::put_time(now_tm,"%H:%M:%S");
    return oss.str();
}


void GPSWorker::addingToCache(std::string time, const _gpsMetadataStruct & data){
    std::lock_guard<std::mutex> lock(cacheMutex);
    _cache[time] = data;
    if(_cache.size()>=10){
        sqlite3_exec(_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);
        const char* sql ="INSERT INTO LOG (time,city, lat, lng) VALUES(?,?,?,?) ";
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
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

void GPSWorker::loadGeoToCache(){
    if(_isGeoLoaded)return;
    json vnGeo;
    std::ifstream geoOutput(_geoPolygonPath.string());
    geoOutput>>vnGeo;
    try{
        for (const auto& feature :vnGeo["features"]){
            Polygon poly;
            for(const auto& pair: feature["geometry"]["coordinates"][0]){
                
                float lng = pair[0];
                float lat = pair[1];
                bg::append(poly.outer(),Point(lng,lat));
                
            }
            cityNames CN{
                feature["properties"].value("name", "Unknown City"),
                feature["properties"].value("real_name","Unknown City")
            };
            _geoCache.push_back({CN,poly});
            qDebug() << "Loaded city:" << QString::fromStdString(CN.nameId);
        }
    }
    catch (const std::exception & e){
        qDebug()<<"Fail to load Geo Polygon data: "<<e.what();
    }
    _isGeoLoaded =true;
}

std::string GPSWorker::getCurrentCity(double lat, double lng){
    qDebug()<<"pass3";
    if(!_isGeoLoaded)this->loadGeoToCache();
    qDebug()<<"pass4";
    Point point (lng,lat);
    for(const auto& [names,poly]: _geoCache){
        if (bg::within(point,poly)|| bg::intersects(point, poly)){
            if(names.nameId != _currentCity && names.nameId != "Unknown City"){
                _currentCity = names.nameId;
                Q_EMIT cityChanged(names.nameId,names.realName);
                return names.nameId;
            }
            
        }
    } 

    return "Unknown City";
}


void GPSWorker::startReadingFromGps(){
    qDebug()<<"pass33";
    try{
        if(!_serial->open(QIODevice::ReadOnly)){
            throw std::runtime_error("Failed to open serial port!");
        } // open to read form port
    }
    catch (const std::exception &e){
        qDebug()<<"Serial error"<<e.what();
    }
    qDebug()<<"pass333";
    connect(_serial, &QSerialPort::readyRead,this ,[&](){
        qDebug()<<"pass3333";
        QByteArray data = _serial->readAll();
        QString line = QString::fromUtf8(data);
        QStringList parts =line.split(',');
        if (parts[0]=="$GPRMC" ){
                float lat= parts[3].toFloat();
                float lng= parts[5].toFloat();
                this-> getCurrentCity(lat,lng);
                _gpsMetadataStruct data{
                    _currentCity,
                    lat,
                    lng
                };
                this ->addingToCache(this->getCurrentTime(),data);
                Q_EMIT coordinatesUpdate (lat,lng);
            }
                
        else if (parts[0]=="$GPVTG"){

            float speed=parts[7].toFloat();
        }
        else{
            qDebug()<<"Fail to read data or there is no good data!";
        }
        
        
    });
}


GPSWorker::~GPSWorker(){
    if (_settings) {
        delete _settings;
        _settings = nullptr;
    }
    if (_serial) {
        if (_serial->isOpen()) {
            _serial->close();
        }
        delete _serial;
        _serial = nullptr;
    }
}