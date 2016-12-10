#ifndef ZDEBUGCALLSTACK_H
#define ZDEBUGCALLSTACK_H

#include <QObject>
#include<QMap>
#include<QStringList>
#include<ztpmanager.h>
#include<QMutex>
#include<QTime>
#define DEBUG_PORT 9999
#define DEBUG_IP       "224.102.228.49"
class ZDebugCallStack : public QObject
{
    Q_OBJECT
    QTime time;
    QMutex map_mutex;
    QMap<int,QStringList> stackMap;
    QString makeStr();
    ZTPManager* debugZtpm;
    int strListLen;
public:
    void setListLen(int _len){strListLen = _len;}
    explicit ZDebugCallStack(QObject *parent = 0);
    void appendStack(int threadId,QString callInfo);
    void print();
    void saveFile(const QString& fileName);
signals:

public slots:

private slots:
    void procDebug();

};

#endif // ZDEBUGCALLSTACK_H
