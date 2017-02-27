#include "CQplayerGUI.h"
#include "ui_CQplayerGUI.h"
#include "config.h"
#include "unistd.h"
#include <iostream>
#include<QFile>
#include<QtCore>
#include <QTimer>
#include<QMutex>
#include<QFrame>
#include<QFile>
#include "sdpsrcparser.h"
#include"globalinfo.h"
#include"CQmedia.h"
#include"CQipc.h"
#include "map_panel.h"
#include"zutility.h"
#include"ztools.h"
using namespace std;
bool CQplayerGUI::play(int _channel,CQplayerGUI::PlayMode mode)
{
    if(mode == CQplayerGUI::M_IPC )
    {
        qDebug()<<"Play ipc  url :  "<<ZUtility::getIpcUrl(_channel);
        if(!ipc->play(ZUtility::getIpcUrl(_channel)))
        {
            qDebug()<<"Play ipc  url :  "<<ZUtility::getIpcUrl(_channel) <<" failed.";
            return false;
        }
        currentPlayMode = CQplayerGUI::M_IPC;
        currentIPCChannel = _channel;
    }
    else if(mode == CQplayerGUI::M_VIDEO)
    {
        if(_channel == 0)
        {
            qDebug()<<"Play GPS INFO.";
            stackPanel->show();
        }
        else
        {
            QString videoUrl = ZUtility::getVideoUrl(_channel);
            qDebug()<<videoUrl;
            if(videoUrl == "")
            {
                qDebug("Get video url is null");
                return false;
            }
            if(!media->play(videoUrl))
                return false;
        }

        currentPlayMode = CQplayerGUI::M_VIDEO;
        currentVideoChannel = _channel;
    }
    return true;
}
void CQplayerGUI::stop()
{
    media->stop();
    ipc->stop();
    stackPanel->hide();
    currentPlayMode = M_NULL;
}

CQplayerGUI::CQplayerGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CQplayerGUI)
{
    translator = new MyTranslator(":/language_thai.ini");
    switchLanguage = new QTimer;
    switchLanguage->setSingleShot(false);
    switchLanguage->setInterval(10000);
    connect(switchLanguage,SIGNAL(timeout()),this,SLOT(refresh()));
    switchLanguage->start();
    cqAudio = NULL;
    versionCtrl = new VersionSender("CarController",1,1,12);
    ui->setupUi(this);

    ztpmForTest = new ZTPManager(8319,QHostAddress("224.102.228.40"));
    connect(ztpmForTest,SIGNAL(readyRead()),this,SLOT(slot_procTestZtp()));

     ioDevice = new IODeviceListener;
    setWindowFlags(Qt::FramelessWindowHint|(windowFlags() & (~Qt::WindowCloseButtonHint)));

    stackPanel = new StackPanel (translator,ui->frameMovie);

    stackPanel->hide();

    cqAudio = new CQAudio(ui->frameMovie,stackPanel,0);
    workThread.start();
    currentPlayMode = M_NULL;

    media = new CQMedia(ui->frameMovie->winId(),0,CQMedia::CAR);
    ipc = new CQipc(ui->frameMovie->winId());
    GlobalInfo::getInstance()->player = this;


    updateTimer = new QTimer;
    updateTimer->setSingleShot(false);
    connect(updateTimer,SIGNAL(timeout()),this,SLOT(updateTime()));
    updateTimer->setInterval(1000);
    updateTimer->start();

    updateDaemonTimer = new QTimer;
//    updateDaemonTimer->setSingleShot(false);
//    connect(updateDaemonTimer,SIGNAL(timeout()),this,SLOT(updateDaemon()));
//    updateDaemonTimer->setInterval(30000);
//    updateDaemonTimer->start();

}


CQplayerGUI::~CQplayerGUI()
{
    delete ui;
    delete media;
    delete stackPanel;
    delete ipc;
    gst_deinit();
}

void CQplayerGUI::updateTime()
{
    ui->dateTimeLabel->setStyleSheet("font: 900 14pt \"Ubuntu\";");
    ui->dateTimeLabel->setText(QDateTime::currentDateTime().toString("20yy-MM-dd hh:mm:ss"));
    //ZTools::singleShot(1000,updateTime);
}


void CQplayerGUI::updateDaemon()
{
    system("ps | grep [c]ar-daemon | awk '{print $1}'|  xargs kill -9");
//    QFile::copy(":/car-daemon","/tmp/car-daemon");
//    system("chmod +x /tmp/car-daemon");
    system("/tmp/car-daemon   &");
//    system("rm /tmp/car-daemon");
}
void CQplayerGUI::slot_procTestZtp()
{
    ZTPprotocol ztp;
    ztpmForTest->getOneZtp(ztp);
    if(ztp.getPara("T") == "TEST" && ztp.getPara("CARID").toInt() == ZTools::getCarID() &&  ztp.getPara("DEV") == "CAR"
            &&  ztp.getPara("CMD") == "EXIT_WORKTHREAD")
    {
        versionCtrl->setTestOffline();
//        workThread.exit(0);
    }
}

void CQplayerGUI::refresh()
{

    if(translator->curLan == "En")
    {
        translator->curLan = "Th";
    }
    else
    {
        translator->curLan = "En";
    }
    stackPanel->updateLan();

}
