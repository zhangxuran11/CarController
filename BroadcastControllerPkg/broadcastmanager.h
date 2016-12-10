#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H
#include"callstateparser.h"
#include"railwaystateparser.h"
#include"trainstateparser.h"

////////////////////////////////广播控制器通讯串口//////////////////////////
class QTimer;
class QextSerialPort;
class BroadcastManager : public QObject
{
    Q_OBJECT


    QTimer* trainStateTimer;
    QTimer* lineInfoTimer;
    QThread* pthread;
    QextSerialPort *broadcastPort;

    CallStateParser callState;
    bool start_f;
    bool isPwdCar;
signals:
    void readyRead();

private slots:
    void on_readyRead();
    void sendTrainState();
    void sendLineInfo();

public:
    QDateTime dateTime;
    int trainId;
    int carId;
    float speed;

    QString startStationEN;  //始发站英文版
    QString startStationThai;  //始发站泰文版
    QString endStationEN;  //终点站英文版
    QString endStationThai;  //终点站泰文版

    void setPwdFlag(bool isPwd){isPwdCar = isPwd;}
    void start();
    BroadcastManager(const QString &serialName);
    ~BroadcastManager();
    const CallStateParser& getOneCallState(){return callState;}
    void sendBroadcastPkg(BasicParser& parser);

};

#endif // SERIALTHREAD_H
