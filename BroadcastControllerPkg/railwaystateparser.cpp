#include "railwaystateparser.h"
#include<QDebug>
#include"ztools.h"
//线路状态解析器

//    quint8 msgHead; //包起始符
//    quint8 msgLen;  //包长度  固定长度132byte
//    quint8 msgType; //包类型  固定为0x82
//    QString startStationEN;  //始发站英文版
//    QString startStationThai;  //始发站泰文版
//    QString endStationEN;  //终点站英文版
//    QString endStationThai;  //终点站泰文版
//    quint8 fcs;//累加和
RailwayStateParser::RailwayStateParser():
    BasicParser(132)
{
    dataFram.resize(FRAME_SIZE);
    msgHead = 0xf5;
    msgLen = 132;
    msgType = 0x82;
}

void RailwayStateParser::generate()
{
    QByteArray stationBytes;

    if(stationType == 1 || stationType == 2)
        stationBytes = stationName.toAscii();
    else
    {
//        if(stationType == 3 || stationType == 4)
//        {
//            qDebug("hhhhhhhhhhhhhhh");
//            QString str =  stationName;
//            qDebug()<<str;
//            for(int i = 0; i< str.length();i++)
//            {
//                qDebug("%d : %x",i,str[i].unicode());
//            }
//        }
//        qDebug("stage 1111111111111111");
//        for(int i = 0;i < stationName.length();i++)
//        {
//            qDebug("%d : %x",i,stationName[i].unicode());
//        }
        stationBytes = ZTools::str2unicode(stationName);
//        qDebug("stage 22222222222222222");
//        for(int i = 0;i < stationBytes.length();i++)
//        {
//            qDebug("%d : %x",i,stationBytes.data()[i]);
//        }
    }
    msgLen = stationBytes.length()+5;//整个包的长度
    FRAME_SIZE = msgLen;
    dataFram.resize(msgLen);
    dataFram.fill(0,msgLen);
    memcpy(dataFram.data(),&msgHead,1);
    memcpy(dataFram.data()+1,&msgLen,1);
    memcpy(dataFram.data()+2,&msgType,1);
    memcpy(dataFram.data()+3,&stationType,1);
    memcpy(dataFram.data()+4,stationBytes.data(),stationBytes.length());
    fcs = genarateCheckSum();
    memcpy(dataFram.data()+msgLen-1,&fcs,1);
//    for(int i= 0 ;i < dataFram.length();i++)
//        printf("%d--%x\n",i,dataFram.data()[i]);

}

void RailwayStateParser::print()
{
	if(stationType == 1)
		qDebug()<<"english start station";
	if(stationType == 2)
		qDebug()<<"english end station";
	if(stationType == 3)
		qDebug()<<"Thai start station";
	if(stationType == 4)
		qDebug()<<"Thai statrt station";
    qDebug()<<"stationName   : "<<stationName;
    printRaw();
}
