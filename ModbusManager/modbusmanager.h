#ifndef MODBUSMANAGER_H
#define MODBUSMANAGER_H

#include <QThread>
#include<QDateTime>

#include<QList>
#include"modbus.h"

/***********************
  usage
    ModbusManager* modbus = new ModbusManager("/dev/ttyUSB1",4);
    modbus->open();
    connect(modbus,SIGNAL(readyRead()),this,SLOT(fun()));


void MainWindow::fun()
{
    Modbus *mb = modbus->getOneModbus();
    mb->print();
    delete mb;
}

  ********************/

class Modbus;
class QextSerialPort;
class ModbusManager : public QObject
{
    Q_OBJECT
        QextSerialPort *serial;
        QList<Modbus*> mbList;

signals:
        void readyRead();
public:
    enum QueryMode {
        Polling,
        EventDriven
    };
    enum ResultState
    {
        SUCCESS,
        TIMEOUT,
        FAILED
    };
    Modbus* getOneModbus();//返回动态生成对象 ，使用后要delete  失败返回NULL
    Modbus* WaitOneModbus(int timeout = 300);//failed return NULL
    void SendOneModbus(Modbus& ac);

    ModbusManager(const QString& devName,int  dirIO = 0,QueryMode mode = EventDriven);
    bool open();
    void close();
    ~ModbusManager();
    void setDirection(int d); //485方向　0收1发
private:
    //int bytesWrittenCnt;
    int _dirIO;

private slots:
    //void onByteWritten(){bytesWrittenCnt++;}
    void readyData();
public slots:
    void _setDirection();
};

#endif // MODBUSMANAGER_H

