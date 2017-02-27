#include "vserverctrl.h"

VServerCtrl::VServerCtrl()
{
    cmd ="";
    from = "";
    timerSendVCtrl.setSingleShot(false);
    timerSendVCtrl.setInterval(1000);
    connect(&timerSendVCtrl,SIGNAL(timeout()),this,SLOT(slot_sendVCtrl()));
    ztpm = new ZTPManager(8321,QHostAddress("224.102.228.40"));
    connect(ztpm,SIGNAL(readyRead()),this,SLOT(slot_recvReqFromVideo()));
}
static int send_cnt = 0;
void VServerCtrl::slot_sendVCtrl()
{
    if(--send_cnt >= 0) //允许失败三次
    {
        timerSendVCtrl.stop();
        return;
    }
    ZTPprotocol ztp;
    ztp.addPara("T","VideoCtrl");
    ztp.addPara("CMD",cmd);
    ztp.addPara("FROM",from);
    ztpm->SendOneZtp(ztp,QHostAddress("224.102.228.40"),8321);
//    ztp.print();
}
void VServerCtrl::slot_recvReqFromVideo()
{
    ZTPprotocol ztp;
    ztpm->getOneZtp(ztp);
    if(ztp.getPara("T") == "VideoOK")
        timerSendVCtrl.stop();
}
void VServerCtrl::pausedServer(QString from)
{
    send_cnt = 3;
    this->cmd="V-PAUSED";
    this->from = from;
    slot_sendVCtrl();
    timerSendVCtrl.setInterval(1000);
    timerSendVCtrl.start();
}
void VServerCtrl::playServer()
{
    send_cnt = 3;
    cmd="V-PLAY";
    slot_sendVCtrl();
    timerSendVCtrl.setInterval(1000);
    timerSendVCtrl.start();
}

