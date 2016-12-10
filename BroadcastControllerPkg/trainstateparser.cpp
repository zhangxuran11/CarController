#include "trainstateparser.h"
#include<QDebug>
//车辆状态解析器
//quint8 msgHead; //包起始符
//quint8 msgLen;  //包长度  固定长度18byte
//quint8 msgType; //包类型  固定为0x81
//QDateTime systemDateTime;
//quint32 trainID;//列车编号
//quint16 carID;//车厢号
//quint16 speed;//列车速度
//quint8 fcs;//累加和
//QByteArray dataFram;
TrainStateParser::TrainStateParser():BasicParser(18)
{
    dataFram.resize(FRAME_SIZE);
    msgHead = 0xf5;
    msgLen = 18;
    msgType = 0x81;
}

void TrainStateParser::generate()
{
    dataFram.resize(FRAME_SIZE);

    memcpy(dataFram.data(),&msgHead,1);
    memcpy(dataFram.data()+1,&msgLen,1);
    memcpy(dataFram.data()+2,&msgType,1);
    dataFram[3] = 0;
    dataFram.data()[3] |= systemDateTime.date().year()%100/10<<4;
    dataFram.data()[3] |= systemDateTime.date().year()%10;
    dataFram[4] = 0;
    dataFram.data()[4] |= systemDateTime.date().month()%100/10<<4;
    dataFram.data()[4] |= systemDateTime.date().month()%10;
    dataFram[5] = 0;
    dataFram.data()[5] |= systemDateTime.date().day()%100/10<<4;
    dataFram.data()[5] |= systemDateTime.date().day()%10;
    dataFram[6] = 0;
    dataFram.data()[6] |= systemDateTime.time().hour()%100/10<<4;
    dataFram.data()[6] |= systemDateTime.time().hour()%10;
    dataFram[7] = 0;
    dataFram.data()[7] |= systemDateTime.time().minute()%100/10<<4;
    dataFram.data()[7] |= systemDateTime.time().minute()%10;
    dataFram[8] = 0;
    dataFram.data()[8] |= systemDateTime.time().second()%100/10<<4;
    dataFram.data()[8] |= systemDateTime.time().second()%10;
    memcpy(dataFram.data()+9,&trainID,4);
    memcpy(dataFram.data()+13,&carID,2);
    memcpy(dataFram.data()+15,&speed,2);
    fcs = genarateCheckSum();
    memcpy(dataFram.data()+17,&fcs,1);
}

void TrainStateParser::print()
{
    qDebug()<<"trainID  : "<<trainID;
    qDebug()<<"carID    : "<<carID;
    qDebug()<<"speed    : "<<speed;
    qDebug()<<"time    : "<<systemDateTime.toString();
    printRaw();
}
