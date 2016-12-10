#ifndef RAILWAYSTATEPARSER_H
#define RAILWAYSTATEPARSER_H
#include"basicparser.h"
#include<QString>
//线路状态解析器
class RailwayStateParser :public BasicParser
{
private:
    quint8 msgHead; //包起始符
    quint8 msgLen;  //包长度  固定长度132byte
    quint8 msgType; //包类型  固定为0x82
    quint8 fcs;//累加和
    void generate();

//    friend class BroadcastManager;
public:
    quint8 stationType;//线路类型 1-英文始发站，2-英文终点站，3-泰文始发站，4-泰文终点站
    RailwayStateParser();

    QString stationName;
    void print();
};

#endif // RAILWAYSTATEPARSER_H
