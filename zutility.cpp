#include "zutility.h"
#include"globalinfo.h"
#include<stdio.h>
#include"ztools.h"
ZUtility::ZUtility()
{

}
 QString ZUtility::getVideoUrl(int channel)
{
    return QString("udp://225.0.0.1:200%1").arg(channel);
}

 QString ZUtility::getIpcUrl(int channel)
{
     char buf[100];
     int ipcNr = 0;
     if(channel == 1)
         ipcNr = 1;
     else
         ipcNr = 4;
     int x;
     if(ZTools::isForward())
         x = ZTools::getCarID();
     else
         x = 14 - ZTools::getCarID();
     sprintf(buf,"rtsp://admin:srt123456@192.168.%d.%d:554/h264/ch1/main/av_stream",x,ipcNr);
    return QString(buf);
}
