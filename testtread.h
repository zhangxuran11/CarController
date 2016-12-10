#ifndef TESTTREAD_H
#define TESTTREAD_H
#include<QThread>

class TestTread : public QThread
{

    Q_OBJECT
    void testloop();
public:
    TestTread();
private slots:
    void run();
};

#endif // TESTTREAD_H
