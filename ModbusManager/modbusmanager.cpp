#include "modbusmanager.h"
#include "modbus.h"
#include "qextserialport/src/qextserialport.h"
#include<iostream>
#include<stdio.h>
#include "ztools.h"
#include<QTimer>
#include<QFile>
#include <QEventLoop>
#include<QThread>
#include <unistd.h>
#include<QDebug>
#include<stdio.h>
#include <sys/time.h>
#include <signal.h>
using namespace std;

static void QextSerialPort_init(QextSerialPort *port)
{
    port->setDtr(false);
    port->setBaudRate(BAUD9600);
    port->setFlowControl(FLOW_OFF);
    port->setParity(PAR_EVEN);
    port->setDataBits(DATA_8);
    port->setStopBits(STOP_1);
    port->setTimeout(10);

}
bool ModbusManager::open()
{
    bool res = serial->open(QIODevice::ReadWrite);
    if(!res)
        qDebug()<<serial->errorString();
    return res;
}
void ModbusManager::close()
{
    serial->close();
}

void ModbusManager::setDirection(int d)
{
    QString io = QString::number(_dirIO);
    if(_dirIO == 0)
        return;

    QFile f("/sys/class/gpio/gpio"+io+"/value");
    if(!f.open(QFile::ReadWrite))
        qDebug("open file value failed!!");
	f.write(QString::number(d).toAscii());
    f.close();
    sync();
}

ModbusManager::ModbusManager(const QString& devName,int dirIO,QueryMode mode)
{
    _dirIO = dirIO;
    QString io = QString::number(dirIO);
    if(0 != _dirIO)
    {
        if(!QFile::exists("/sys/class/gpio/gpio"+io))
        {
            QString cmd = "echo "+io+" > /sys/class/gpio/export";
            int res = system(cmd.toAscii().data());
            if(res == -1)
            {
                qDebug("echo io > /sys/class/gpio/export failed!!");
            }
            QFile f;
            if(!QFile::exists("/sys/class/gpio/gpio"+io))
                qDebug()<<"not exits gpio"+io;
            f.setFileName("/sys/class/gpio/gpio"+io+"/direction");
            f.open(QFile::ReadWrite);
            f.write("out");
            f.close();
            sync();
        }
        setDirection(0);
    }
    if(mode == EventDriven)
    {
        serial = new QextSerialPort(devName,QextSerialPort::EventDriven);
        connect(serial,SIGNAL(readyRead()),this,SLOT(readyData()));
    }
    else if(mode == Polling)
        serial = new QextSerialPort(devName,QextSerialPort::Polling);
    QextSerialPort_init(serial);

}

ModbusManager::~ModbusManager()
{
    delete serial;
}
void ModbusManager::readyData()
{
    Modbus* mb = NULL;
    static int addr = -1;
    static int code = -1;
    static QByteArray buff;
    QByteArray tmp;
    static int start = 0, end = -1;
//    qDebug("recv >>>>>>>>>>>>>>>>>>>");
    while((tmp = serial->readAll()).length() != 0)
    {
        buff.append(tmp);
        if(0)
        {
            for(int i = 0;i<tmp.length();i++)
            {
                qDebug("%d: %x",i,(char)tmp[i]);
            }
        }
    }
    if(0)
    {
        QString s_str;
        char c_str[10];
        for(int i = 0;i < buff.length();i++)
        {
            sprintf(c_str,"%02x ",buff.data()[i]);
            s_str.append(c_str);
        }
        qDebug()<<s_str;
    }
//    qDebug("recv <<<<<<<<<<<<<<<<<<<");
    //qDebug()<<"recv :"<<buff;

    if(buff.length() < 7)
    {
        return;
    }
    addr = (int)buff.data()[0];
    if(addr != 0x03)
    {
        goto err;
    }
    code = (int)buff.data()[1];

    if(code != 0x10)
    {
        goto err;
    }
    end = (int)buff.data()[6]+8;

    if(end > 41 )
    {
        goto err;
    }
//    qDebug("end = %d",end);
//    qDebug("length=%d,start=%d ",buff.length(),start);
    if(buff.length()-start <end+1)
    {
        return;
    }
//    qDebug("end = %d 222",end);
    mb = new Modbus(buff.mid(start,end+1));
    buff.clear();
    end = -1;
    code = -1;
    addr = -1;
    if(!mb->isValid)
    {
        qDebug("Modbus not has avalid data:");
        delete mb;
        return;
    }
    //mb->print();
    if(mbList.count() > 50)
        delete mbList.takeFirst();
    mbList.append(mb);

    emit readyRead();
    return;
err:
    addr = -1;
    end = -1;
    code = -1;
    buff.clear();
    return;
}
Modbus* ModbusManager::getOneModbus()
{
    if(mbList.isEmpty())
        return NULL;
    Modbus* mb = mbList.front();
    mbList.pop_front();
    return mb;
}//返回动态生成对象 ，使用后要delete  失败返回NULL

Modbus* ModbusManager::WaitOneModbus(int msecs)
{
    QEventLoop q;
    Modbus* resMb = NULL;
    QTimer::singleShot(msecs,&q,SLOT(quit()));
    connect(this,SIGNAL(readyRead()),&q,SLOT(quit()));
    q.exec();
    resMb = getOneModbus();
    disconnect(this,SIGNAL(readyRead()),&q,SLOT(quit()));
    return resMb;
}
static int delay = 0;
static ModbusManager* modBusManager = NULL;
void ModbusManager::_setDirection( )
{
    qDebug("_setDirection ");
    qDebug("delay %d ",delay);
    ZTools::msleep(delay/2);
    modBusManager->setDirection(0);
}
//static void __setDirection()
//{

//    static bool init = true;
//    struct itimerval tick;
//    memset(&tick, 0, sizeof(tick));
//    tick.it_value.tv_sec =   delay/1000;
//    tick.it_value.tv_usec =  delay%1000*1000;

//    if(init)
//    {
//        init = false;
//        signal(SIGALRM, _setDirection);
//    }
//    if(setitimer(ITIMER_REAL, &tick, NULL) < 0)
//        qDebug("Set timer failed!\n");
//}

void ModbusManager::SendOneModbus(Modbus& mb)
{
    //static bool init = true;
    mb.generate();
    setDirection(1);

    serial->write(mb.rawData);
    int bpf = 0;//bit per byte frame
    int startBits = 1;
    bpf = serial->dataBits() + startBits+ (serial->stopBits() == STOP_1?1:2) + (serial->parity() == PAR_NONE ? 0:1);
    delay = bpf * 1000000 * mb.rawData.length() /serial->baudRate() +500;
    modBusManager = this;
    //if(init == true)
    //{
    //    init = false;
//        connect(serial,SIGNAL(bytesWritten(qint64)),this,SLOT(_setDirection(qint64)),Qt::DirectConnection);

//                connect(serial,SIGNAL(sigWriteComplete()),this,SLOT(_setDirection()),Qt::DirectConnection);
    //}

    //__setDirection();
//    qDebug("_____2 delay %d",delay);
    usleep(delay + 2000);
    //ZTools::msleep(delay);
    setDirection(0);

}

