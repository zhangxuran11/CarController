#ifndef SERIALAC_H
#define SERIALAC_H
#include<QDateTime>
#include<QList>
#include"basicparser.h"
//呼叫状态解析器
class CallStateParser :public BasicParser
{
private:
    quint8 msgHead; //包起始符
    quint8 msgLen;  //包长度  固定长度8byte
    quint8 msgType; //包类型  固定为0x80
    quint8 callMark;  //呼叫标记
    quint8 appendInfo1;  //追加信息
    quint8 appendInfo2;
    quint8 appendInfo3;
    quint8 fcs;//累加和
    void generate();


//    friend class BroadcastManager;
public:
    bool isValid; //数据是否有效
    bool hasCall1;//呼叫1是否发生
    bool hasCall2;//呼叫2是否发生
    bool hasCall3;//呼叫3是否发生
    bool hasCall4;//呼叫4是否发生
    bool hasBroadcast;//是否有广播信号
    bool hasBroadcast_lc;//是否有本地广播信号

public:
    CallStateParser():BasicParser(8){}
    explicit CallStateParser(QByteArray pkg):
        BasicParser(8){load(pkg);}
    void load(const QByteArray& pkg);
    void print();
};

#endif // SERIALAC_H
