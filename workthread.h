#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include <QThread>
#include <QTimerEvent>
#include"modbusmanager.h"
#include"iodevicelistener.h"
class ZTPManager;
class BroadcastManager;
class SoftWatchdog;
class WorkThread : public QThread
{
    Q_OBJECT
    ModbusManager*  modbusManager;
    SoftWatchdog* modbusDog;
    SoftWatchdog* comSoftDog;
    ZTPManager *comZtpManager;
    ZTPManager *ztpBroadcastManager;
    QTimer* recvCarrierHeartTimer;

public:
    ZTPManager* ztpmGetSysIp;
    ZTPManager* ztpmCarrierHeart;
    BroadcastManager* comBroadcastManager;
    explicit WorkThread(QObject *parent = 0);
    ~WorkThread();
private slots:
    void test();
    void run();
    void ztpBroadcastProc();
    void comBroadcastProc();
    void OnRecvCarrierHeart();
    void recvModbus();
    void recvSysIp();
    void recvCarrierHeartTimeout();
protected:
    void timerEvent(QTimerEvent * event );



};

#endif // WORKTHREAD_H
