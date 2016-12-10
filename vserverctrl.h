#ifndef VSERVERCTRL_H
#define VSERVERCTRL_H
#include <QObject>
#include "ztpmanager.h"
class VServerCtrl :public QObject
{
Q_OBJECT
    QString cmd;
    QString from;
    ZTPManager* ztpm;
    QTimer timerSendVCtrl;
public:
    VServerCtrl();
    void pausedServer(QString from);
    void playServer();
private slots:
    void slot_recvReqFromVideo();
    void slot_sendVCtrl();
};

#endif // VSERVERCTRL_H
