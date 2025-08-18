#ifndef ALBUM_H
#define ALBUM_H
#include <QObject>
#include <QLabel>
#include <QSlider>
#include <QPixmap>
#include <QVBoxLayout>
#include <QDialog>
#include "libcam2opencv.h"
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>
#include <QSettings>
#include <filesystem>
#include <sqlite3.h>
#include <gps.h>
struct _albumsStruct {
        std::string time;
        std::string city;
        std::string path;
        std::string type;
        float lat;
        float lng;
        
    };

class AlbumWorker : public QObject{
    Q_OBJECT

    private:
    
    std::vector<std::pair<std::string, _albumsStruct>> _albumCache;
    QSettings * _settings = nullptr;
    std::filesystem::path _parentDir;
    std::filesystem::path _albumDB;
    sqlite3* _db;
    GPSWorker _gpsObject;
    
   public:
   AlbumWorker();
   void initAlbumPath();
    void initDB();
    void insertToDB(const std::string& path,std::string type);
    void getAllDatafromAlbumDB(std::vector<std::pair<std::string, _albumsStruct>>& _albumCache);
};
class Album : public QObject{
    Q_OBJECT

    private:
    AlbumWorker* _albumWorkerObject;
    std::vector<std::pair<std::string, _albumsStruct>> _albumCache;
    QVBoxLayout* _mainlayout;
    QSettings * _settings = nullptr;
    std::filesystem::path _parentDir;
    std::filesystem::path _albumDB;
    sqlite3* _db;
    GPSWorker _gpsObject;
    QScrollArea* _scrollArea;
    QWidget* _albumWidget;
    QGridLayout* _albumLayout;
    bool isDisplay =false;

   public:
   Album(QVBoxLayout * layout, QObject *parent);
   void initAlbumPath();
    void initDB();
    void insertToDB(const std::string& path,std::string type);
    void getAllDatafromAlbumDB();
    bool setUpAlbumLabel();
    void showPreview(const std::string& filepath);
    bool eventFilter(QObject* obj, QEvent* event)override;
    void closeAlbum();
    // void disPlayAlbum();

};
#endif