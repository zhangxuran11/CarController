#ifndef DEVICELISTENER_H
#define DEVICELISTENER_H
#include<QObject>
#include<QString>
#include<QTimerEvent>
#include "gpio.h"
class QTimer;
class IODeviceListener :public QObject
{
    Q_OBJECT
private:
    GPIO cs1;   //有人－－高电平－－有效
    GPIO cs2;
    GPIO cs3;
    GPIO cs4;

    GPIO slm1;
    GPIO slm2;  //打开－－高电平－－有效
    //QTimer* cs_slm_Timer;
    int cs_slm_timer_id;

protected:
    void timerEvent(QTimerEvent *event);
public:
    explicit IODeviceListener(QObject *parent = 0);
    ~IODeviceListener();
    QString getCs(int ind);
    QString getSlm(int ind);
    
private slots:
    void onCsSlmTimeout();
    
};

#endif // DEVICELISTENER_H
