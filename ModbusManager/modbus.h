#ifndef MODBUS_H
#define MODBUS_H
#include<QList>
#include<QByteArray>
class Modbus
{
public:

    int addr;
    int code;//功能码



    int regAddr;
    QList<int> datList;
    QByteArray rawData;
    int datCount;//数据数量
    bool isValid; //数据是否有效

private:
    quint16 LRC;

    quint16 calCLR();//接收时候调用
    void load(QByteArray pkg);


    quint16 generateCLR();

public:
    Modbus();
    void generate();
    explicit Modbus(QByteArray pkg){load(pkg);}
    void addDat(int dat){datList.append(dat);datCount = datList.length();}
    void print();

};

#endif // MODBUS_H
