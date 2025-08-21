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

inline std::filesystem::path _parentDir = std::filesystem::current_path().parent_path();
inline QSettings* _settings = new QSettings(QString::fromStdString((_parentDir / "Configure.ini").string()), QSettings::IniFormat);
inline std::filesystem::path _databaseDir = _parentDir / _settings->value("Database/path").toString().toStdString();
inline std::filesystem::path _dbPath = _parentDir /_settings->value("Database/gpsPath").toString().toStdString();
inline std::filesystem::path _geoPolygonPath = _parentDir / _settings->value("Database/geoPolygonPath").toString().toStdString();
inline std::filesystem::path _gallery = _parentDir / _settings->value ("Database/galleryPath").toString().toStdString();
inline std::filesystem::path _albumDB = _parentDir / _settings->value ("Database/albumsDB").toString().toStdString();
inline std::filesystem::path _playIconPath = _parentDir/ _settings->value("Assets/playicon").toString().toStdString();
inline std::filesystem::path _mainbackgroundPath = _parentDir/ _settings->value("Assets/mainbackground").toString().toStdString();
