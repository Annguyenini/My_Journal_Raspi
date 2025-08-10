#include <QSerialPort>
#include <QObject>
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QThread>
#include <QSettings>
#include <QSerialPortInfo>
#include <filesystem>
#include <QSettings>
#include <sqlite3.h>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <utility>
#include "mainwindow.h"
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/polygon.hpp>
using Point = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
using Polygon = boost::geometry::model::polygon<Point>;
class GPSWorker: public QObject{
    Q_OBJECT

    private:
        std::filesystem::path _parentDir;
        QSettings* _settings;
        std::filesystem::path _databaseDir;
        std::filesystem::path _dbPath;
        std::filesystem::path _geoPolygonPath;
        std::string _currentCity;
        std::string _currentRealnamecity;
        QSerialPort* _serial = nullptr;
        sqlite3* _db;
        QString _buffer;
        float _speed=0;
        float _lat = 21.357298;
        float _lng = 105.834322;
        struct _gpsMetadataStruct{
            std::string city;
            float lat;
            float lng;
        };
        static std::unordered_map<std::string, _gpsMetadataStruct> _cache;
        static std::mutex cacheMutex;
        bool initPath();
        struct cityNames{
            std::string nameId;
            std::string realName;
        };
        bool _isGeoLoaded = false;
        std::vector<std::pair<cityNames,Polygon>> _geoCache;
    public:
        GPSWorker();
        ~GPSWorker();
        void initialPort();
        void startReadingFromGps();
        void setUpDB();
        std::string getCurrentTime();
        std::string getCurrentCity(double lat, double lng);
        // void speedMonitor();
        
        void addingToCache(std::string time, const _gpsMetadataStruct & data);
        void startThread();
        Q_SIGNAL void coordinatesUpdate(const float& lat, const float& lng);
        Q_SIGNAL void cityChanged(std::string nameId, std::string realName);
        Q_SIGNAL void readyToShow();
        void loadGeoToCache();
        
};
class GPS: public QObject{
    Q_OBJECT
    private:
        QLabel*  _cityLabel;
        QLabel* _coordinatesLabel; 
        QVBoxLayout* _mainlayout;
        QThread* _workerThread;
        GPSWorker* _worker;

    public:
    explicit GPS(QVBoxLayout * layout, QObject *parent = nullptr);

    void setUpGps();
    ~GPS();
    GPSWorker* worker() const { return _worker; }
    void handleCityUpdate(const std::string &nameId, const std::string &realName);
    void handleCoordinatesUpdate(float lat, float lng);
    void stop();
    Q_SIGNAL void stopWorker();

    
};