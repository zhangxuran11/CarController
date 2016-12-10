#include "workthread.h"
#include "config.h"
#include"CQmedia.h"
#include "globalinfo.h"
#include"ztpmanager.h"
#include "ztpprotocol.h"
#include"config.h"
#include"CQplayerGUI.h"
#include"broadcastmanager.h"
#include"iodevicelistener.h"
#include"modbusmanager.h"
#include"ztools.h"
#include<unistd.h>
#include"softwatchdog.h"
static int debug = 0;
static int sendTempTimerId = 0;
static float temp = 0;
//static float outTemp = 0;
static void comDeal();
static void proBroadSignal(bool broadSignal);
WorkThread::WorkThread(QObject *parent) :
    QThread(parent)
{
    comBroadcastManager = NULL;
    GlobalInfo::getInstance()->workThread = this;
}
static bool switch_channel_f = false;
static void opt_switch_channel_f()
{
    switch_channel_f = false;
}
static void procMediaChannel(const ZTPprotocol& ztp,int _channel)
{
//    qDebug("entry file:%s line:%d func:%s thread:%ld",__FILE__,__LINE__,__FUNCTION__,QThread::currentThreadId());
    qDebug()<<"recv Video Channel "<<_channel;
    CQplayerGUI* player = GlobalInfo::getInstance()->player;
    QString ch = "Ch" + ztp.getPara("ChId");
    //qDebug()<<QString("procMediaChannel(%1)").arg(_channel);
    if(player->getCurrentMode() == CQplayerGUI::M_IPC)
    {
        qDebug()<<"curent play mode is M_IPC , ignore this channel number.";
//        qDebug("entry file:%s line:%d func:%s thread:%ld",__FILE__,__LINE__,__FUNCTION__,QThread::currentThreadId());
        return;
    }
    if(player->media->getPlayingState()!=CQGstBasic::STOPPED && _channel == player->getCurrentChannel() &&  ztp.getPara(ch) != "paused")
    {
//        qDebug("leave file:%s line:%d func:%s thread:%ld",__FILE__,__LINE__,__FUNCTION__,QThread::currentThreadId());
        qDebug()<<"Current channel number is playing.";
        return;
    }
    if(switch_channel_f)
    {
        qDebug("The switch of channel is too frequent.");
//        qDebug("leave file:%s line:%d func:%s thread:%ld",__FILE__,__LINE__,__FUNCTION__,QThread::currentThreadId());
        return;
    }

    GlobalInfo::getInstance()->player->stop();
    if(ztp.getPara("ChId").toInt() ==  -1 ) {
        qDebug()<<"current ChId is invalid.";
        return;
    }
    if(ztp.getPara("ChId") != "0" && ztp.getPara(ch) != "play"){
        qDebug()<<"Current ChId is not play...";
        return;
    }
    if(ztp.getPara("ChId") == "0" &&GlobalInfo::getInstance()->player->stackPanel->s_off)
        return;
    GlobalInfo::getInstance()->player->play(_channel,CQplayerGUI::M_VIDEO);

    switch_channel_f = true;
    ZTools::singleShot(3000,opt_switch_channel_f);
    //    qDebug("leave file:%s line:%d func:%s thread:%ld",__FILE__,__LINE__,__FUNCTION__,QThread::currentThreadId());
}
void WorkThread::ztpBroadcastProc()
{
    GlobalInfo::getInstance()->debugStack.appendStack(QThread::currentThreadId(),"ready entry ztpBroadcastProc");
//    qDebug()<<"ztpBroadcastManager recv data";
    ZTPprotocol ztp;
    ZTPManager* ztpm = dynamic_cast<ZTPManager*>(sender());
    if(ztpm == NULL)
        return;
    ztpm->getOneZtp(ztp);//ZZZZZZZZZZZZZZ
//    ztpBroadcastManager->getOneZtp(ztp);

//    qDebug()<<"T :"<<ztp.getPara("T");
    if(ztp.getPara("T")=="T_ChId")  //ZZZZZZZZZZZZZZZZZZZZ
    {

//        qDebug()<<"ChId :"<<ztp.getPara("ChId");
//        ztp.print();

        procMediaChannel(ztp,ztp.getPara("ChId").toInt());
    }
    else if(ztp.getPara("T") == "IPaddress1")
    {
        if(GlobalInfo::getInstance()->VServerIP != ztp.getPara("mainvideoserver"))
            GlobalInfo::getInstance()->VServerIP = ztp.getPara("mainvideoserver");
        if(GlobalInfo::getInstance()->sysCtlIp != ztp.getPara("syscontroller"))
            GlobalInfo::getInstance()->sysCtlIp = ztp.getPara("syscontroller");

        if(GlobalInfo::getInstance()->VServerIP != NULL &&
            GlobalInfo::getInstance()->sysCtlIp != NULL)
            delete ztpm;
    }
    else if(ztp.getPara("T") == "G_INFO")
    {
        GlobalInfo::getInstance()->player->stackPanel->setSecsOff(ztp.getPara("SECS_OFF").toInt());
        int train_id = ztp.getPara("TrainNum").toInt();
        if(GlobalInfo::getInstance()->train_id != train_id)
        {
            GlobalInfo::getInstance()->train_id = train_id;
            GlobalInfo::getInstance()->player->stackPanel->setTrainId(train_id);
            GlobalInfo::getInstance()->player->stackPanel->loadTimeTable(train_id);
            GlobalInfo::getInstance()->player->stackPanel->OnUpdateMap();
        }
        if(comBroadcastManager != NULL)
        {
            static unsigned int cnt = 0;
            cnt++;
            comBroadcastManager->dateTime = QDateTime::fromString(ztp.getPara("DateTime"),"yy-MM-dd hh:mm:ss");
            comBroadcastManager->speed = ztp.getPara("Speed").toFloat();
            comBroadcastManager->trainId = ztp.getPara("TrainNum").toInt();
            comBroadcastManager->carId = ZTools::getCarID();
            if(1 || cnt > 10) {
                comBroadcastManager->startStationEN = ztp.getPara("StartStation");
                comBroadcastManager->endStationEN = ztp.getPara("EndStation");
                comBroadcastManager->startStationThai = ztp.getPara("StartStation_th");
                comBroadcastManager->endStationThai = ztp.getPara("EndStation_th");
            }
            comBroadcastManager->start();

           if(debug)
           {
                QString str =  ztp.getPara("StartStation_th");
                qDebug()<<str;
                for(int i = 0; i< str.length();i++)
                 {
                      qDebug("%d : %x",i,str[i].unicode());
                 }
           }
        }
    }
    else if(ztp.getPara("T") == "BroadCast")
    {
        bool broadSignal = false;
        if(ztp.getPara("HasBroadcast") == "Y" ||
                (ztp.getPara("HasBroadcast_lc") == "Y" &&
                 ztp.getPara("CarId").toInt() == GlobalInfo::getInstance()->carId ))
        {
           broadSignal = true;
        }
        proBroadSignal(broadSignal);
    }
    GlobalInfo::getInstance()->debugStack.appendStack(QThread::currentThreadId(),"leave ztpBroadcastProc");
//    qDebug("leave file:%s line:%d func:%s thread:%ld",__FILE__,__LINE__,__FUNCTION__,QThread::currentThreadId());
}

static void proBroadSignal(bool broadSignal)
{
    CQplayerGUI* player = GlobalInfo::getInstance()->player;
    if(broadSignal && !player->media->isSilence())
        player->media->setSilence(true);
    else if(!broadSignal && player->media->isSilence())
        player->media->setSilence(false);
}
static void procCall(int ipcChannel)
{
    CQplayerGUI* player = GlobalInfo::getInstance()->player;
    if(ipcChannel == 0 )
    {
        if( player->getCurrentMode() == CQplayerGUI::M_IPC){
            player->stop();
            if(GlobalInfo::getInstance()->player->cqAudio != NULL)
                GlobalInfo::getInstance()->player->cqAudio->disableUpateImage = false;
                GlobalInfo::getInstance()->player->ui->frameMovie->setStyleSheet(QString::fromUtf8("border-image: url(:/images/welcome.png);"));
        }
        return;
    }
    if(player->getCurrentMode() == CQplayerGUI::M_IPC && player->getCurrentChannel() == ipcChannel)
        return;
    player->stop();
    if(GlobalInfo::getInstance()->player->cqAudio != NULL)
        GlobalInfo::getInstance()->player->cqAudio->disableUpateImage = true;
    player->play(ipcChannel,CQplayerGUI::M_IPC);

    static ZTPManager* ztpm = new ZTPManager;
    ZTPprotocol ztp;
    ztp.addPara("T","T_UCall");
    int x = 0;
    if(ZTools::isForward())
    {
        x = ZTools::getCarID();
    }
    else
    {
        x = 14 - ZTools::getCarID();
    }
    QString ip = QString("192.168.%1.%2").arg(x).arg(ipcChannel);
    ztp.addPara("CamIp",ip);
//    ztp.addPara("CarId",QString::number(GlobalInfo::getInstance()->carId));
//    ztp.addPara("ChId",QString::number(ipcChannel));
    ztpm->SendOneZtp(ztp,QHostAddress("224.102.228.40"),12350);
}
void WorkThread::comBroadcastProc()
{
//    GlobalInfo::getInstance()->debugStack.appendStack(QThread::currentThreadId(),"ready entry comBroadcastProc");
    static QString preBroadcast = "N";
    comSoftDog->feed();
    CallStateParser call = comBroadcastManager->getOneCallState();
//    call.print();
    int ipcChannel = call.hasCall1?1:call.hasCall2?2:call.hasCall3?3:call.hasCall4?4:0;//hasCall1优先级最高
    static ZTPManager* ztpm = new ZTPManager;
    ZTPprotocol ztp;
    QString tmp;
    ztp.addPara("T","BroadCast");
    if(call.hasBroadcast)
        tmp = "Y";
    else
        tmp = "N";
    if(preBroadcast == "N" && tmp == "Y")
        GlobalInfo::getInstance()->vServerCtrl->pausedServer("PA");
    if(preBroadcast == "Y" && tmp == "N")
        GlobalInfo::getInstance()->vServerCtrl->playServer();
    preBroadcast = tmp;
    ztp.addPara("HasBroadcast",tmp);
    if(call.hasBroadcast_lc)
        tmp = "Y";
    else
        tmp = "N";
    ztp.addPara("HasBroadcast_lc",tmp);
    ztp.addPara("CarId",QString::number(GlobalInfo::getInstance()->carId));
    ztpm->SendOneZtp(ztp,QHostAddress(BROADCAST_IP),8310);
//    qDebug()<<"==========================  IPC  "<<ipcChannel;
    procCall(ipcChannel);
//    GlobalInfo::getInstance()->debugStack.appendStack(QThread::currentThreadId(),"leave comBroadcastProc");
}
/***************************************
static void queryTemperature()
{
    static bool hasError = false;
    Modbus ac;
    ac.addr = 1;
    ac.code = 3;
    ac.regAddr = 0x80;
    ac.datCount = 8;

    int tryCnt = 3;
    while(tryCnt--)
    {
        GlobalInfo::getInstance()->modbusManager->SendOneModbus(ac);
        ac.print();
        Modbus* _ac = GlobalInfo::getInstance()->modbusManager->WaitOneModbus(300);
        if(_ac == NULL)
        {
            qDebug("air ctrl error");
//            globalInfo->carNu = -1;
            //空调控制器异常
        }
        else
        {
            ZTPManager* ztpm = new ZTPManager;
            ZTPprotocol ztp;
            ztp.addPara("T","CarTemperature");
            ztp.addPara("VALUE",QString::number(_ac->datList[4]*0.1-50));
            ztp.addPara("VALUE_OUT",QString::number(_ac->datList[5]*0.1-50));
            ztp.addPara("CAR_ID",QString::number(ZTools::getCarID()));
            ztpm->SendOneZtp(ztp,QHostAddress(BROADCAST_IP),BROADCAST_PORT);
            ztp.print();

            delete _ac;
            delete ztpm;
            break;
        }
    }
    if(tryCnt == -1)
    {
        if(hasError == true)
            goto _end;
        if(GlobalInfo::getInstance()->VServerIP != "")
        {
            hasError = true;
            ZTPManager* ztpm = new ZTPManager;
            int _tryCnt = 3;
            while(_tryCnt--)
            {
                ZTPprotocol ztp;
                ztp.addPara("T","Error");
                ztp.addPara("Code","103");
                ztpm->SendOneZtp(ztp,QHostAddress(GlobalInfo::getInstance()->VServerIP),8881);
                ZTPManager::ResultState res =  ztpm->waitOneZtp(ztp);
                if(res == ZTPManager::SUCCESS)
                {
                    qDebug("Send Error 103 success!");
                    break;
                }
            }
            delete ztpm;
        }
    }
    else
        hasError = false;
_end:
    ZTools::singleShot(3000,queryTemperature);
    return;
}
*****************************************/
static void comDeal()
{
    if(GlobalInfo::getInstance()->VServerIP != "")
    {
        static ZTPManager* ztpm = new ZTPManager;
        int _tryCnt = 3;
        while(_tryCnt--)
        {
            ZTPprotocol ztp;
            ztp.addPara("T","Error");
            ztp.addPara("Code","102");
            ztpm->SendOneZtp(ztp,QHostAddress(GlobalInfo::getInstance()->VServerIP),8881);
            ZTPManager::ResultState res =  ztpm->waitOneZtp(ztp);
            if(res == ZTPManager::SUCCESS)
            {
                qDebug("Send Error 102 success!");
                break;
            }
        }
    }
    //GlobalInfo::getInstance()->workThread->comBroadcastManager = NULL;
    qDebug("cleanUp with comDeal");
}

static void modbusDeal()
{
    temp = 0;
//    outTemp = 0;
    if(GlobalInfo::getInstance()->VServerIP != "")
    {
        static ZTPManager* ztpm = new ZTPManager;
        int _tryCnt = 3;
        while(_tryCnt--)
        {
            ZTPprotocol ztp;
            ztp.addPara("T","Error");
            ztp.addPara("Code","103");
            ztpm->SendOneZtp(ztp,QHostAddress(GlobalInfo::getInstance()->VServerIP),8881);
            ZTPManager::ResultState res =  ztpm->waitOneZtp(ztp);
            if(res == ZTPManager::SUCCESS)
            {
                qDebug("Send Error 103 success!");
                break;
            }
        }
    }
    //GlobalInfo::getInstance()->workThread->comBroadcastManager = NULL;
    qDebug("cleanUp with modbusDeal");
}
void WorkThread::test()
{
    int ipcChannel = 4;
    static ZTPManager* ztpm = new ZTPManager;
    ZTPprotocol ztp;
    ztp.addPara("T","T_UCall");
    ztp.addPara("CarId",QString::number(GlobalInfo::getInstance()->carId));
    ztp.addPara("ChId",QString::number(ipcChannel));
    ztpm->SendOneZtp(ztp,QHostAddress("224.102.228.40"),12350);
    ztp.print();

    QTimer::singleShot(1000,this,SLOT(test()));
}

static void sendTemp()
{
    static ZTPManager* ztpm = new ZTPManager;
    ZTPprotocol ztp;
    ztp.addPara("T","CarTemperature");
    ztp.addPara("VALUE",    QString::number(temp));
//    ztp.addPara("OUT VALUE",QString::number(outTemp));
//    ztp.addPara("VALUE_OUT",QString::number(_ac->datList[5]*0.1-30));
    ztp.addPara("CAR_ID",QString::number(ZTools::getCarID()));
//    ztp.addPara("CAR_ID",QString::number(_ac->datList[0]));
    ztpm->SendOneZtp(ztp,QHostAddress(BROADCAST_IP),8312);
    ztp.print();
}
void WorkThread::timerEvent ( QTimerEvent * event )
{
    if(event->timerId() == sendTempTimerId)
    {
        sendTemp();
    }
}
void WorkThread::recvModbus()
{
//    GlobalInfo::getInstance()->debugStack.appendStack(QThread::currentThreadId(),"ready entry recvModbus");
    modbusDog->feed();
    Modbus* _ac = modbusManager->getOneModbus();
    if(debug)
    {
        for(int i =0; i < _ac->datList.length();i++)
        {
            qDebug()<<"datList "<<i<<" : "<<_ac->datList[i];
        }
	_ac->print();
    }
    if(_ac->datList.length() == 8)
    {
        temp = _ac->datList[4]*0.1-30;
//        outTemp = _ac->datList[5]*0.1-30;
//        qDebug()<<"out temp = "<<outTemp;
    }
    Modbus acr;
    acr.addr = _ac->addr;
    acr.code = _ac->code;
    acr.regAddr = _ac->regAddr;
    acr.datCount = _ac->datCount;
    usleep(20000);
//    _ac->print();
    modbusManager->SendOneModbus(acr);
//    qDebug()<<"regAddr : "<<acr.regAddr;
    delete _ac;
    //delete ztpm;
//    GlobalInfo::getInstance()->debugStack.appendStack(QThread::currentThreadId(),"leave recvModbus");
}
void WorkThread::run()
{
    ztpmGetSysIp= new ZTPManager(8866,QHostAddress("224.102.228.40"));
    connect(ztpmGetSysIp,SIGNAL(readyRead()),this,SLOT(recvSysIp()));
    ztpmCarrierHeart = new ZTPManager(8317,QHostAddress("224.102.228.40"));
    connect(ztpmCarrierHeart,SIGNAL(readyRead()),this,SLOT(OnRecvCarrierHeart()));

    recvCarrierHeartTimer = new QTimer;
    recvCarrierHeartTimer->setInterval(10000);
    recvCarrierHeartTimer->setSingleShot(false);
    connect(recvCarrierHeartTimer,SIGNAL(timeout()),this,SLOT(recvCarrierHeartTimeout()));
    recvCarrierHeartTimer->start();

    ZTPManager* broadCast = new ZTPManager(8310,QHostAddress("202.102.228.40"));
    connect(broadCast,SIGNAL(readyRead()),this,SLOT(ztpBroadcastProc()));
    ZTPManager* g_infoZtpm = new ZTPManager(8311,QHostAddress(BROADCAST_IP));
    connect(g_infoZtpm,SIGNAL(readyRead()),this,SLOT(ztpBroadcastProc()));//由于槽中操作了stackPanel，故放入主线程。
    sendTempTimerId = startTimer(2000);
    //QTimer::singleShot(1000,this,SLOT(test()));
    //ZTools::singleShot(3000,queryTemperature);
    modbusManager = new ModbusManager("/dev/ttyUSB1",4);
    modbusDog = new SoftWatchdog(modbusManager,modbusDeal);
    connect(modbusManager,SIGNAL(readyRead()),this,SLOT(recvModbus()),Qt::DirectConnection);//子线程调槽
    bool res = modbusManager->open();
    if(res == true)
    {
        qDebug("open modbusManager success.");
    }
    else
        qDebug("open modbusManager failed.");

    qDebug()<<"work thread start run ...";
    ztpBroadcastManager = new ZTPManager(BROADCAST_PORT,QHostAddress(BROADCAST_IP));
    connect(ztpBroadcastManager,SIGNAL(readyRead()),this,SLOT(ztpBroadcastProc()));//主线曾调槽
    ztpBroadcastManager = new ZTPManager;//专用于发送

    ZTPManager* chIdZtpm = new ZTPManager(8315,QHostAddress(BROADCAST_IP));//多媒体频道广播
    connect(chIdZtpm,SIGNAL(readyRead()),this,SLOT(ztpBroadcastProc()));//主线曾调槽
    comBroadcastManager = new BroadcastManager("/dev/ttymxc2");
    comSoftDog = new SoftWatchdog(comBroadcastManager,comDeal);
    connect(comBroadcastManager,SIGNAL(readyRead()),this,SLOT(comBroadcastProc()));//主线程调槽

    comZtpManager = new ZTPManager;
    exec();
}
WorkThread::~WorkThread()
{
    delete comZtpManager;
    delete ztpBroadcastManager;
//    delete ioDevice;
}
#include"globalinfo.h"
void WorkThread::OnRecvCarrierHeart()
{
    ZTPprotocol ztp;
    ztpmCarrierHeart->getOneZtp(ztp);
    if(ztp.getPara("T") != "CarrierHeart")
        return;
    GlobalInfo::getInstance()->player->versionCtrl->setOnline(true);
    recvCarrierHeartTimer->start();
    if(!QFile::exists("/tmp/DevExist"))
        system("touch /tmp/DevExist");
}
void WorkThread::recvCarrierHeartTimeout()
{
    GlobalInfo::getInstance()->player->versionCtrl->setOnline(false);
}
void WorkThread::recvSysIp()
{
    ZTPprotocol ztp;
    ztpmGetSysIp->getOneZtp(ztp);
    QString new_sysCar = ztp.getPara("TT1") ;
    if(new_sysCar == "")
        return;
    qDebug()<<" SYS CAR NU "<<new_sysCar;
    if(!QFile::exists("/etc/syscar.txt") || GlobalInfo::getInstance()->sysCar != new_sysCar)
    {
        GlobalInfo::getInstance()->sysCar = new_sysCar;
        QString cmd = QString("echo %1 > /etc/syscar.txt && sync").arg(new_sysCar);
        system(cmd.toAscii().data());
    }
}
