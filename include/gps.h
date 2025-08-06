#include <QSerialPort>
#include <QSerialPortInfo>

class GPS{
    private:
        QserialPort* _serial;
    public:
        GPS();
        ~GPS();
        void startReadingFromGps();

        
}