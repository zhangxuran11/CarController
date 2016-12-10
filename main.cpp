//#define QT_NO_DEBUG_OUTPUT
#define __MAIN__
#include <QtGui/QApplication>
#include "CQplayerGUI.h"
#include "config.h"
#include"crashtool.h"
#include<QDebug>
#include<stdio.h>
#include <string.h>
#include<QFile>
static void start_daemon();
int main(int argc, char *argv[])
{
//    EnableCrashHandler();
    QApplication a(argc, argv);
    QApplication::setOverrideCursor(Qt::BlankCursor);
    start_daemon();
    qDebug("2016/3/24 18:48");
    CQplayerGUI w;
    w.show();
    return a.exec();
}
static void start_daemon()
{
    system("ps | grep [c]ar-daemon | awk '{print $1}'|  xargs kill -9");
    QFile::copy(":/car-daemon","/tmp/car-daemon");
    system("chmod +x /tmp/car-daemon");
    system("/tmp/car-daemon  &");
    system("rm /tmp/car-daemon");
}
