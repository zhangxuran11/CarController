#ifndef CONFIG_H
#define CONFIG_H
#include "gpio.h"
#define CS1_GPIO             IMX_GPIO_NR(1, 11)
#define CS2_GPIO             IMX_GPIO_NR(1, 10)
#define CS3_GPIO             IMX_GPIO_NR(1,  8)
#define CS4_GPIO             IMX_GPIO_NR(1,  7)

#define SLM1_GPIO            IMX_GPIO_NR(7,  6)
#define SLM2_GPIO            IMX_GPIO_NR(7, 12)

#define BROADCAST_IP       "224.102.228.40"
#define BROADCAST_PORT      7789

#define ROOM_NO     6

#define IPC_SERVER_IP1   "192.168.1.64"
#define IPC_SERVER_IP2   "192.168.1.66"
#define IPC_SERVER_IP3   "192.168.1.68"
#define IPC_SERVER_IP4   "192.168.1.70"
#define IPC_NUM         4

#ifdef  __MAIN__

const char* IPC_SERVER_URL_ARR[]={
    "rtsp://admin:wyx123456@"  IPC_SERVER_IP1  ":554/h264/ch1/main/av_stream",
    "rtsp://admin:wyx123456@"  IPC_SERVER_IP2  ":554/h264/ch1/main/av_stream",
    "rtsp://admin:wyx123456@"  IPC_SERVER_IP3  ":554/h264/ch1/main/av_stream",
    "rtsp://admin:wyx123456@"  IPC_SERVER_IP4  ":554/h264/ch1/main/av_stream",
};
#else
extern const char* VIDEO_SERVER_URL_ARR[];
extern const char* IPC_SERVER_URL_ARR[];
#endif
#endif // CONFIG_H
