#ifndef BUTTONS_H
#define BUTTONS_H
#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

class Buttons :public QObject{
    Q_OBJECT
    private:
        QVBoxLayout* _mainlayout;
        QHBoxLayout* _buttonsLayout;
        QPushButton* _logBtn;
        QPushButton* _cameraBtn;
        QPushButton* _reloadBtn;
        QPushButton* _albumBtn;
        QPushButton* _snapPictureBtn;
        QPushButton* _recordBtn;
        QString _btn_properties = R"(QPushButton {
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

    public:
        Buttons(QVBoxLayout* mainlayout, QObject* parent);
        void setUpButtons();
        Q_SIGNAL void callAlbum();
        Q_SIGNAL void callCamera();
        Q_SIGNAL void callLog();
        Q_SIGNAL void callReloadMap();
        Q_SIGNAL void callSnap();
        Q_SIGNAL void callRecord();
};

#endif