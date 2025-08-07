#include <QSerialPort>
#include <QSerialPortInfo>
#include <filesystem>
#include <QSettings>
#include <sqlite3.h>
#include <unodered_map>
class GPS{
    private:
        std::filesystem::_parentDir = std::filesystem::current_path().parent_path();
        QSettings _settings(_parentDir+"/Configure.ini",QSettings::IniFormat);
        std::filesystem::path _databaseDir = _settings.value["Database/path"];
        std::filesystem::path _dbPath = _settings.value["Database/gpsPath"];
        std::filesystem::path _geoPolygonPath = _settings.value["Database/geoPolygonPath"];
        std::string _currentCity;
        std::string _currentRealnamecity;
        QserialPort* _serial;
        sqlite3* _db;
        float _speed=0;
        struct _gpsMetadataStruct{
            std::string city,
            float lat,
            float lng
        }
        static std::unordered_map<std::string, gpsMetadata> _cache;
        static std::mutex cacheMutex;
        bool initPath();
    public:
        GPS();
        ~GPS();
        void startReadingFromGps();
        void setUpDB();
        std::string getCurrentTime();
        void speedMonitor();
        
}