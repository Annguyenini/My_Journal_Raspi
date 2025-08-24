#include "configure.h"
#include "trip.h"




Trip::Trip(){
    
}
void Trip::setUpTripDialog(){
    _tripDialog = new QDialog();
    _tripDialog->setWindowTitle("TRIP MODE");
    _tripStatus =new QHBoxLayout();
   
    _tripInfobar =new QHBoxLayout();
    _tripTotalpic = new Qabel();
    _tripTotalTime = new QLabel();
    _tripSettingsBar = new QHBoxLayout();
    _tripEditBoxButton = new QPushButton();
    _tripStopTripButton = new QPushButton();


}


void Trip::displayTrip(){
    if (TRIP_ACTIVE){
        displayTripTabel();
    }
    else{
        setUpNewTrip();
    }
}


void Trip::displayTripTabel(){
    _tripId = new QLabel();
    _tripName = new QLabel();
    _tripId ->setText();
    _tripId ->setStyleSheet();
    _tripName ->setText();
    _tripName ->setStyleSheet();
    _tripStatus->addWidget(_tripId);
    _tripStatus->addWidget(_tripName);

    _tripTotalpic ->setText();
    _tripTotalpic ->setStyleSheet();
    _tripTotalTime ->setText();
    _tripTotalTime ->setStyleSheet();
    _tripInfobar ->addWidget(_tripTotalpic);
    _tripInfobar ->addWidget(_tripTotalTime);

    _tripSettingsBar->addWidget(_tripEditBoxButton);
    _tripSettingsBar->addWidget(_tripStopTripButton);
    connect(_tripEditBoxButton, &QPushButton, this ,[this](){

    });

    connect(_tripStopTripButton, &QPushButton, this ,[this](){

    });
}


void Trip::setUpNewTrip(){

}