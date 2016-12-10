#include "zdebugcallstack.h"
#include <iterator>
#include<QTime>
#include<QFile>
#include<QDateTime>
ZDebugCallStack::ZDebugCallStack(QObject *parent) :
    QObject(parent)
{
    time.start();
    strListLen = 20;
    debugZtpm = new ZTPManager(DEBUG_PORT,QHostAddress(DEBUG_IP));
    connect(debugZtpm,SIGNAL(readyRead()),this,SLOT(procDebug()));
}
void ZDebugCallStack::procDebug()
{
    ZTPprotocol ztp;
    debugZtpm->getOneZtp(ztp);
    if(ztp.getPara("T") == "PRINT_CALL_STACK")
        print();
    else if(ztp.getPara("T") == "SAVE_CALL_STACK")
    {
        QDateTime dateTime = QDateTime::currentDateTime();
        QString fileName = QString("/tmp/call_stack%1.txt").arg(dateTime.date().year()).arg(dateTime.date().month()).arg(dateTime.date().day())
                .arg(dateTime.time().hour()).arg(dateTime.time().minute()).arg(dateTime.time().second());
        saveFile(fileName);
    }
}
void ZDebugCallStack::saveFile(const QString& fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::Append))
    {
        qDebug()<<"open file "<<fileName<<"failed:"<<file.errorString();
    }
    else
    {
        qDebug()<<"open file "<<fileName<<"success!";
    }
    file.write(makeStr().toUtf8());
    file.close();
}
#if 0
void ZDebugCallStack::appendStack(int ,QString )
{

}
#else
void ZDebugCallStack::appendStack(int threadId,QString callInfo)
{
    QDateTime dateTime = QDateTime::currentDateTime();
   QString timeStr =  QString("%1 days %2:%3:%4.%5 %6-%7 %8:%9:%10")
           .arg(time.elapsed()/1000/60/60/24)   //日
           .arg(time.elapsed()/1000/60/60%24)   //時
           .arg(time.elapsed()/1000/60%60)  //分
           .arg(time.elapsed()/1000%60) //秒
           .arg(time.elapsed()%1000)   //毫秒
           .arg(dateTime.date().month())    //当前时间
           .arg(dateTime.date().day())
           .arg(dateTime.time().hour())
           .arg(dateTime.time().minute())
           .arg(dateTime.time().second());
//    QString timeStr = QString::number(time.elapsed()/1000/60/60/24)+" days "+QString::number(time.elapsed()/1000/60/60%24)+":"+QString::number(time.elapsed()/1000/60%60)+":"+QString::number(time.elapsed()/1000%60)+"."+QString::number(time.elapsed()%1000);
    callInfo = callInfo + "---"+ QString::number(QThread::currentThreadId())+"---"+timeStr;
    if(stackMap.contains(threadId))
    {
        QStringList& strList = stackMap[threadId];
        if(strList.count() > strListLen)
            strList.pop_front();
        strList.append(callInfo);
    }
    else
    {
        map_mutex.lock();
        stackMap.insert(threadId,QStringList(callInfo));
        map_mutex.unlock();
    }
}
#endif
QString ZDebugCallStack::makeStr()//private
{
    int threadId = 1;
    QString str;
    for(QMap<int,QStringList>::Iterator it = stackMap.begin();it != stackMap.end();it++)
    {
        str.append(QString("thread %1:PWL %2\n").arg(threadId++).arg(it.key()));
        const QStringList& strList = it.value();
        for(int i = 0;i < strList.count();i++)
            str.append(strList[i]+"\n");
        str.append("\n");
    }
    str.append(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "\n");
    return str;
}
void ZDebugCallStack::print()
{
    qDebug()<<makeStr();
}
