#include <QSerialPort>
#include <QObject>

#include <QSerialPortInfo>
#include <filesystem>
#include <QSettings>
#include <sqlite3.h>
#include <mutex>
#include <unordered_map>
class GPS: public QObject{
    Q_OBJECT

    private:
        std::filesystem::path _parentDir;
        QSettings* _settings;
        std::filesystem::path _databaseDir;
        std::filesystem::path _dbPath;
        std::filesystem::path _geoPolygonPath;
        std::string _currentCity;
        std::string _currentRealnamecity;
        QSerialPort* _serial;
        sqlite3* _db;
        float _speed=0;
        float _lat =0;
        float _lng =0;
        struct _gpsMetadataStruct{
            std::string city;
            float lat;
            float lng;
        };
        static std::unordered_map<std::string, _gpsMetadataStruct> _cache;
        static std::mutex cacheMutex;
        bool initPath();
    public:
        GPS();
        ~GPS();
        
        void startReadingFromGps();
        void setUpDB();
        std::string getCurrentTime();
        std::string getCurrentCity(double lat, double lng);
        void speedMonitor();
        
        void addingToCache(std::string time, const _gpsMetadataStruct & data);
        void startThread();
};
class GPSWoker:public QObject{
    Q_OBJECT
    public:
        Q_SIGNAL void coordinatesUpdate();
        Q_SIGNAL void cityChanged();
        void startReadingFromGps();
        void speedMonitor();
    
}