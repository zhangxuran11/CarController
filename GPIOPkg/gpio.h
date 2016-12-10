#ifndef GPIO_H
#define GPIO_H
#include"cstdio"
#define IMX_GPIO_NR(bank, nr)                (((bank) - 1) * 32 + (nr))


class GPIO
{
public:
    enum Direction{IN,OUT};
    enum State{H,L};
private:
    const int M_GPIO_NR;
    State validState;
    Direction direct;
    void _setDirection(Direction _direct);
    void _nop_()const{}
public:

    GPIO(int gpio_nr,State bindingState = H,Direction _direct = IN);
    GPIO(int gpio_bank,int gpio_nr,State bindingState = H,Direction _direct = IN);
    void bind(State bindingState){validState = bindingState;}  //将有效的电平状态与高或低电平绑定
    bool isValid(){return getState() == validState ? true : false;}
    Direction setDirection(Direction _direct);
    Direction getDirection()const{return direct;}
    State getState();
    State setState(State _state);
};

#endif // GPIO_H
