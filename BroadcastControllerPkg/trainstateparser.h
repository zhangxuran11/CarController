#ifndef TRAINSTATEPARSER_H
#define TRAINSTATEPARSER_H
#include<QtGlobal>
#include<QDateTime>
#include"basicparser.h"
//车辆状态解析器
class TrainStateParser :public BasicParser
{
private:
    quint8 msgHead; //包起始符
    quint8 msgLen;  //包长度  固定长度18byte
    quint8 msgType; //包类型  固定为0x81
    quint8 fcs;//累加和
    void generate();
//    friend class BroadcastManager;
public:
    QDateTime systemDateTime;
    quint32 trainID;//列车编号
    quint16 carID;//车厢号
    quint16 speed;//列车速度

    TrainStateParser();
    void print();
};

#endif // TRAINSTATEPARSER_H
