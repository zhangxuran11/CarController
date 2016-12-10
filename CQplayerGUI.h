#ifndef CQPLAYERGUI_H
#define CQPLAYERGUI_H

#include <QDialog>

#include <iostream>
#include <QString>
#include <ui_CQplayerGUI.h>
#include <QFileDialog>
#include <QMutex>
#include "workthread.h"
#include"versionsender.h"
#include"stackpanel.h"
#include"cqaudio.h"
#include"mytranslator.h"
class CQMedia;
class CQipc;
class QTimer;
class SerialThread;
class GstThread;
class NetListener;
class DeviceListener;
class CQplayerGUI : public QDialog
{
    Q_OBJECT
    MyTranslator* translator;
    QTimer* switchLanguage;
    QTimer *updateTimer;
    QTimer* updateDaemonTimer;
    IODeviceListener* ioDevice;

public:
    VersionSender* versionCtrl;
    enum PlayMode{M_IPC,M_VIDEO,M_NULL};
    SerialThread* serial;
public:


    bool play(int channel,PlayMode mode);
    WorkThread workThread;

public:
    ZTPManager* ztpmForTest;
    Ui::CQplayerGUI* ui;
    CQMedia *media;
    CQAudio* cqAudio;
    CQipc *ipc;
    StackPanel* stackPanel;

    explicit CQplayerGUI(QWidget *parent = 0);
    ~CQplayerGUI();
    void stop();
    PlayMode getCurrentMode()const{return currentPlayMode;}
    int getCurrentChannel(){return currentPlayMode == M_IPC?currentIPCChannel:currentPlayMode == M_VIDEO?currentVideoChannel:-1;}

private:
    PlayMode currentPlayMode;
    int currentVideoChannel;
    int currentIPCChannel;
private slots:
    void updateTime();

    void updateDaemon();
    void slot_procTestZtp();
    void refresh();
};

#endif // CQPLAYERGUI_H
