#pragma once
#include <filesystem>
#include <QSettings>
inline QString _btn_properties = R"(QPushButton {
                background-color: #000000;
                color: white;
                font-size: 20px;
                padding: 10px 20px;
                border: none;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #005f99;
    })";
inline QString actionPro = R"(QMenu {
  background-color: #222;
  color: #fff;
  border: 1px solid #555;
}
QMenu::item {
  padding: 10px 70px;
  min-height: 40px;
  font-size: 16px;
}
QMenu::item:selected {
  background-color: #555;
})";

inline std::string getCurrentTime(){
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time_t);

    std::ostringstream oss;
    oss << std::put_time(now_tm,"%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

inline std::filesystem::path _parentDir = std::filesystem::current_path().parent_path();
inline QSettings* _settings = new QSettings(QString::fromStdString((_parentDir / "Configure.ini").string()), QSettings::IniFormat);
inline std::filesystem::path _databaseDir = _parentDir / _settings->value("Database/path").toString().toStdString();
inline std::filesystem::path _dbPath = _parentDir /_settings->value("Database/gpsPath").toString().toStdString();
inline std::filesystem::path _geoPolygonPath = _parentDir / _settings->value("Database/geoPolygonPath").toString().toStdString();
inline std::filesystem::path _gallery = _parentDir / _settings->value ("Database/galleryPath").toString().toStdString();
inline std::filesystem::path _albumDB = _parentDir / _settings->value ("Database/albumsDB").toString().toStdString();
inline std::filesystem::path _playIconPath = _parentDir/ _settings->value("Assets/playicon").toString().toStdString();
inline std::filesystem::path _mainbackgroundPath = _parentDir/ _settings->value("Assets/mainbackground").toString().toStdString();
inline QString var =  _settings->value("Settings/tripActive").toString();
inline unsigned int TRIP_ID= _settings->value("Settings/tripID").toInt();
inline std::string TRIP_NAME="";
inline bool TRIP_ACTIVE = var.toInt()!=0;



// class Config {
// public:
//     static Config& instance() {
//         static Config cfg;
//         return cfg;
//     }

//     QSettings* settings;
//     std::filesystem::path parentDir;
//     unsigned int tripID;
//     bool tripActive;

// private:
//     Config() {
//         parentDir = std::filesystem::current_path().parent_path();
//         settings = new QSettings(QString::fromStdString((parentDir / "Configure.ini").string()), QSettings::IniFormat);
//         tripID = settings->value("Settings/tripID", 0).toUInt();
//         tripActive = settings->value("Settings/tripActive", 0).toInt() != 0;
//     }
// };
