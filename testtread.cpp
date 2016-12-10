#include "testtread.h"
#include <QUdpSocket>
#include <QEventLoop>
#include<QTimer>
TestTread::TestTread()
{
    start();
}
void TestTread::testloop()
{
    QString ip = "225.0.0.1";
    int port = 2001;
    QUdpSocket udp;
    udp.bind(port,QUdpSocket::ShareAddress);
    udp.joinMulticastGroup(QHostAddress(ip));
    QEventLoop q;
    QTimer::singleShot(1000,&q,SLOT(quit()));

    connect(&udp,SIGNAL(readyRead()),&q,SLOT(quit()));
    q.exec();

}
void TestTread::run()
{
    int c = 0;
    while(1)
    {
        qDebug("%d",c++);
        testloop();
    }
}


