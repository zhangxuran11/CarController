#ifndef ZUTILITY_H
#define ZUTILITY_H
#include<QString>
class ZUtility
{
public:
    ZUtility();
    static QString getVideoUrl(int channel);
    static QString getIpcUrl(int channel);
};

#endif // zUTILITY_H
