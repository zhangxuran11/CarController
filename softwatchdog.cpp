#include "softwatchdog.h"
#include "globalinfo.h"
#include"ztpmanager.h"
SoftWatchdog::SoftWatchdog(QObject *ressourceObj,void (*method)(),QObject *parent ) :
    QObject(parent)
{
    deal = method;
    obj = ressourceObj;
    elapse = 5000;
    timer = new QTimer;
    timer->setInterval(elapse);
    timer->setSingleShot(true);
    timer->start();
    connect(timer,SIGNAL(timeout()),this,SLOT(cleanUp()));
}
static bool b = true;
void SoftWatchdog::cleanUp()
{
    deal();
    b = false;
//    delete obj;
 //   delete timer;
 //   delete this;
}
void SoftWatchdog::feed()
{
    if(b)
        timer->start(elapse);
}

