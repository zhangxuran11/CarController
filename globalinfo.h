#ifndef GLOBALINFO_H
#define GLOBALINFO_H

#include <QObject>
#include "zdebugcallstack.h"
#include "vserverctrl.h"
class ZTPManager;
class CQMedia;
class CQplayerGUI;
class ModbusManager;
class WorkThread;
class GlobalInfo : public QObject
{
    Q_OBJECT
    static GlobalInfo* _instance;
    explicit GlobalInfo(QObject *parent = 0);
public:
    VServerCtrl* vServerCtrl;
    QString sysCar;
    ZDebugCallStack debugStack;
    WorkThread* workThread;
    int train_id;
    int carId;
    QString VServerIP;
    QString sysCtlIp;
    static GlobalInfo* getInstance();
//    int carNu;//车厢号  -1表示车厢好获取异常
    CQplayerGUI* player;
    
signals:
    
public slots:
    
};

#endif // GLOBALINFO_H
