#include"callstateparser.h"
#include "qextserialport/src/qextserialport.h"
#include<stdio.h>
#include<QDebug>
//呼叫状态解析器
//    quint8 msgHead; //包起始符
//    quint8 msgLen;  //包长度  固定长度8byte
//    quint8 msgType; //包类型  固定为0x80
//    quint8 callMark;  //呼叫标记
//    quint8 appendInfo1;  //追加信息
//    quint8 appendInfo2;
//    quint8 appendInfo3;
//    quint8 fcs;//累加和
void CallStateParser::load(const QByteArray& pkg)
{
    dataFram.resize(FRAME_SIZE);
    memcpy(dataFram.data(),pkg.data(),FRAME_SIZE);
    memcpy(&msgHead,dataFram.data(),1);
    memcpy(&msgLen,dataFram.data()+1,1);
    memcpy(&msgType,dataFram.data()+2,1);
    memcpy(&callMark,dataFram.data()+3,1);
    memcpy(&appendInfo1,dataFram.data()+4,1);
    memcpy(&appendInfo2,dataFram.data()+5,1);
    memcpy(&appendInfo3,dataFram.data()+6,1);
    memcpy(&fcs,dataFram.data()+7,1);
    isValid = (fcs == genarateCheckSum());
    hasCall1 = callMark & 0x01;
    hasCall2 = callMark & 0x02;
    hasCall3 = callMark & 0x04;
    hasCall4 = callMark & 0x08;
    hasBroadcast = appendInfo1 & 0x01;
    hasBroadcast_lc = appendInfo1 & 0x02;
}

void CallStateParser::generate()
{
//    qDebug()<<"genarate ...";
    dataFram.resize(FRAME_SIZE);
    msgHead = 0xf5;
    msgLen = 8;
    msgType = 0x80;
    appendInfo1 = 0;
    appendInfo2 = 0;
    appendInfo3 = 0;

    callMark = 0;
    appendInfo1 = 0;
    callMark |= hasCall1 == true ? 0x01:0;
    callMark |= hasCall2 == true ? 0x02:0;
    callMark |= hasCall3 == true ? 0x04:0;
    callMark |= hasCall4 == true ? 0x08:0;
//    qDebug()<<"callMark : "<<callMark;
    appendInfo1 |= hasBroadcast == true ? 0x01:0;
    appendInfo1 |= hasBroadcast_lc == true ? 0x02:0;
//    qDebug()<<"appendInfo1 : "<<appendInfo1;
    memcpy(dataFram.data(),&msgHead,1);
    memcpy(dataFram.data()+1,&msgLen,1);
    memcpy(dataFram.data()+2,&msgType,1);
    memcpy(dataFram.data()+3,&callMark,1);
    memcpy(dataFram.data()+4,&appendInfo1,1);
    memcpy(dataFram.data()+5,&appendInfo2,1);
    memcpy(dataFram.data()+6,&appendInfo3,1);
    fcs = genarateCheckSum();
    memcpy(dataFram.data()+7,&fcs,1);

//    for(int i = 0;i < 8;i++)
//        qDebug("%2x \\",(quint8)dataFram.data()[i]);
//    qDebug("\n");
//    qDebug()<<"genarate end";

}
void CallStateParser::print()
{
    qDebug()<<"isValid          : "<<isValid;
    qDebug()<<"hasCall1         : "<<hasCall1;
    qDebug()<<"hasCall2         : "<<hasCall2;
    qDebug()<<"hasCall3         : "<<hasCall3;
    qDebug()<<"hasCall4         : "<<hasCall4;
    qDebug()<<"hasBroadcast     : "<<hasBroadcast;
    qDebug()<<"hasBroadcast_lc  : "<<hasBroadcast_lc;
    printRaw();

}




