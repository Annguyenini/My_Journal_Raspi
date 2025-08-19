#include "album.h"

#include "gps.h"

AlbumWorker::AlbumWorker(){
    initAlbumPath();
    initDB();
}

void AlbumWorker::initAlbumPath(){
    _parentDir = std::filesystem::current_path().parent_path();
    _settings = new QSettings(QString::fromStdString((_parentDir / "Configure.ini").string()), QSettings::IniFormat);
    _albumDB = _parentDir / _settings->value ("Database/albumsDB").toString().toStdString();
   

}
void AlbumWorker::initDB(){
    int rc = sqlite3_open(_albumDB.string().c_str(),&_db);
    
    char* errMsg = nullptr;
    if (rc != SQLITE_OK){
        qDebug()<<"Cant open db";
        return;
    }
    const char*  createTableSQL = "CREATE TABLE IF NOT EXISTS album (path TEXT , time TEXT, type TEXT)";
    rc = sqlite3_exec(_db, createTableSQL, nullptr,nullptr,&errMsg);

    if (rc!= SQLITE_OK){
        qDebug()<<"Fail to create the table";
    }
    if (sqlite3_exec(_db, "PRAGMA journal_mode = WAL;", nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Failed to enable WAL: " << errMsg << "\n";
        sqlite3_free(errMsg);
    } else {
        std::cout << "WAL mode enabled!\n";
    }


}
void AlbumWorker::insertToDB(const std::string& path,std::string type){
    std::string time = _gpsObject.getCurrentTime();
    
    const char* insertToDB = "INSERT INTO Album (path,time,type) VALUES (?,?,?)";
    sqlite3_stmt *stmt;
    if(sqlite3_prepare_v2(_db,insertToDB,-1,&stmt,nullptr)!= SQLITE_OK){
        qDebug()<<"Failed to prepare statement: " << sqlite3_errmsg(_db) << "\n";
        return;
    }
    sqlite3_bind_text (stmt, 1, path.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 2, time.c_str(),-1,SQLITE_TRANSIENT);
    sqlite3_bind_text (stmt, 3, type.c_str(),-1,SQLITE_TRANSIENT);
     if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(_db) << "\n";
    } else {
        std::cout << "Insert successful!\n";
    }

    sqlite3_finalize(stmt);
    // sqlite3_close(_db);

    return;
}
void AlbumWorker::getAllDatafromAlbumDB(std::vector<std::pair<std::string, _albumsStruct>>& _albumCache){
    qDebug() << "Fetching all data from album DB";
    const char* sql = "SELECT path,time,type FROM album ORDER BY time DESC";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        qDebug() << "Failed to prepare SELECT:" << sqlite3_errmsg(_db);
        return;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        std::string path = reinterpret_cast<const char*>(sqlite3_column_text(stmt,0));
        std::string time = reinterpret_cast<const char*>(sqlite3_column_text(stmt,1));
        std::string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt,2));
        
        _albumsStruct at{
            path,
            time,
            type,
        };
        _albumCache.emplace_back(path, at);
    }
}
