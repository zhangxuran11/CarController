#include "globalinfo.h"
#include "ztools.h"
#include"CQmedia.h"
#include"modbusmanager.h"
#include<QFile>
GlobalInfo* GlobalInfo::_instance = NULL;
GlobalInfo::GlobalInfo(QObject *parent) :
    QObject(parent)
{
    if(QFile::exists("/etc/syscar.txt"))
    {
        QFile f("/etc/syscar.txt");
        f.open(QFile::ReadOnly);
        sysCar = f.readAll();
        f.close();
    }
    else
    {
        sysCar = "1";
    }
    train_id = -1;
    VServerIP = "";
    sysCtlIp = "";
    vServerCtrl = new VServerCtrl;
}
GlobalInfo* GlobalInfo::getInstance()
{
    if(_instance == NULL)
        _instance = new GlobalInfo();
    return _instance;
}
