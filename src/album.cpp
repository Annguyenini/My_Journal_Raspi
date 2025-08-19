#include "album.h"



Album::Album(QVBoxLayout * layout, QObject *parent):_mainlayout(layout),QObject(parent){
        _albumWorkerObject =new AlbumWorker();

};

bool Album::setUpAlbumLabel(){
    if (isDisplay ==true) {
        qDebug()<<"album closing";
        closeAlbum();
        return false;
    };
    if(!_albumCache.empty()){
        _albumCache.clear();
    }
    qDebug()<<"runnnnnnnnnnnn";
    _albumWorkerObject->getAllDatafromAlbumDB(_albumCache);
    qDebug() << "Album cache size:" << _albumCache.size();
    _scrollArea = new QScrollArea();
    _scrollArea ->setWidgetResizable(true);
    _albumWidget = new QWidget();
    _albumLayout = new QGridLayout(_albumWidget);
    _albumLayout -> setContentsMargins(0,0,0,0);
    _albumLayout -> setSpacing(5);
    _albumLayout -> setVerticalSpacing(5);
    int row=0, column = 0;
    for(const auto& pair : _albumCache){
        QLabel* label = new QLabel();
        qDebug()<<QString::fromStdString(pair.first);
        QPixmap pixmap = QPixmap(QString::fromStdString(pair.first)).scaled(350, 350, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(pixmap);
        label->setFixedSize(label->pixmap().size());
        label->setProperty("filepath", QString::fromStdString(pair.first));
        label->installEventFilter(this); 
        _albumLayout->addWidget(label,row,column);
        column ++;
        if (column>=2) {
            row++;
            column = 0;
        }
    }    
    _scrollArea->setWidget(_albumWidget);
    _mainlayout ->insertWidget(1,_scrollArea);
    isDisplay =true;
    return true;
}
bool Album::eventFilter(QObject* obj, QEvent* event)  {
    if (event->type() == QEvent::MouseButtonPress) {
        if (QLabel* lbl = qobject_cast<QLabel*>(obj)) {
            qDebug() << "Label clicked!";
            QString filepath = lbl->property("filepath").toString();
            showPreview(filepath.toStdString());
            return true; // event handled
        }
    }
    return QObject::eventFilter(obj, event);
}
void Album::showPreview(const std::string& filepath){
    QDialog* dlg = new QDialog();
    dlg->setWindowTitle(QString::fromStdString(filepath));
    QVBoxLayout* dlgLayout = new QVBoxLayout();
    dlg->setLayout(dlgLayout);
    QLabel* label = new QLabel();
    label->setPixmap(QPixmap(QString::fromStdString(filepath)).scaled(600,600,Qt::KeepAspectRatio, Qt::SmoothTransformation));
    label->setAlignment(Qt::AlignCenter);

    QPushButton* closeBtn = new QPushButton();
    connect (closeBtn, &QPushButton::clicked,dlg, &QDialog::accept);
    dlgLayout->addWidget(label);
    dlgLayout->addWidget(closeBtn);
    dlg->exec();
}

void Album::closeAlbum(){
    qDebug()<<"cleaning";
    _scrollArea->hide();
    _mainlayout->removeWidget(_scrollArea);
    delete _scrollArea;  // deletes child album widget and labels too
    _scrollArea = nullptr;
    _albumWidget = nullptr;
    _albumLayout = nullptr;
    isDisplay =false;
    return;
}