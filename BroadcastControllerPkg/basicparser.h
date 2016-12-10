#ifndef BASICPARSER_H
#define BASICPARSER_H
#include<QtGlobal>
#include<QByteArray>

//呼叫状态解析器
class BasicParser
{

    friend class BroadcastManager;
protected:

    QByteArray dataFram;
    quint8 genarateCheckSum();
    void printRaw();
    virtual void generate() = 0;
public:
    quint8 FRAME_SIZE;
    explicit BasicParser(quint8 _FRAME_SIZE):FRAME_SIZE(_FRAME_SIZE){}
    const char* getData()const{return dataFram.data();}

    virtual void print() = 0;
};
#endif // BASICPARSER_H

