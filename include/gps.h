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
        QserialPort* _serial;
        sqlite3* _db;
        struct gpsMetadata{
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

        
}