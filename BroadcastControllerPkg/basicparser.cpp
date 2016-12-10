#include"basicparser.h"
#include<QDebug>
quint8 BasicParser::genarateCheckSum()
{
    quint8 sum = 0;
    for(int i = 0;i < FRAME_SIZE - 1;i++)
        sum += (quint8)dataFram[i];
    return sum;
}
void BasicParser::printRaw()
{
    qDebug()<<"raw begin ...";
    for(int i = 0;i < dataFram.length();i++)
        qDebug("%2x ",(quint8)dataFram.data()[i]);
    qDebug()<<"raw end .";
}
