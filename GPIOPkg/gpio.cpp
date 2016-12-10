#include "gpio.h"
#include<QFile>
#include<QDebug>
GPIO::GPIO(int gpio_nr,State bindingState,Direction _direct):
    M_GPIO_NR(gpio_nr)
{
    char cmd[60];
    char *filePath = cmd;
    sprintf(filePath,"/sys/class/gpio/gpio%d",M_GPIO_NR);
    if(!QFile::exists ( filePath ))
    {
        sprintf(cmd,"echo %d > /sys/class/gpio/export",M_GPIO_NR);
        int res = system(cmd);
        if(res == -1)
        {
            qDebug()<<"exec cmd "<<cmd<<" failed!!";
        }
    }
    _setDirection(_direct);
    bind(bindingState);
}
GPIO::GPIO(int gpio_bank,int gpio_nr,State bindingState,Direction _direct):
    M_GPIO_NR(IMX_GPIO_NR(gpio_bank,gpio_nr))
{
    char cmd[60];
    sprintf(cmd,"echo %d > /sys/class/gpio/export",M_GPIO_NR);
    int res = system(cmd);
    if(res == -1)
    {
        qDebug()<<"exec cmd "<<cmd<<" failed!!";
    }
    _setDirection(_direct);
    bind(bindingState);
}
void GPIO::_setDirection(Direction _direct)
{
    char cmd[60];

    if(_direct == IN)
        sprintf(cmd,"echo  in > /sys/class/gpio/gpio%d/direction",M_GPIO_NR);
    else if(_direct == OUT)
        sprintf(cmd,"echo out > /sys/class/gpio/gpio%d/direction",M_GPIO_NR);
    int res = system(cmd);
    if(res == -1)
    {
        qDebug()<<"exec cmd "<<cmd<<" failed!!";
    }
    direct = _direct;
}

GPIO::State GPIO::getState()
{
    char fileName[40];
    sprintf(fileName,"/sys/class/gpio/gpio%d/value",M_GPIO_NR);
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    char* data = fileName;
    file.read(data,1);
    file.close();
    return (data[0]-'0') == 0 ? L : H;
}
GPIO::State GPIO::setState(State _state)
{
    State tmp = getState();
    char cmd[60];
    if(_state == H)
        sprintf(cmd,"echo 1 > /sys/class/gpio/gpio%d/value",M_GPIO_NR);
    else if(_state == L)
        sprintf(cmd,"echo 0 > /sys/class/gpio/gpio%d/value",M_GPIO_NR);
    int res = system(cmd);
    if(res == -1)
    {
        qDebug()<<"exec cmd "<<cmd<<" failed!!";
    }
    return tmp;
}

GPIO::Direction GPIO::setDirection(Direction _direct)
{
    Direction tmp = direct;
    direct != _direct ? _setDirection(_direct) :_nop_() ;
    return tmp;
}

