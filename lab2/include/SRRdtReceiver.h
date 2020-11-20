//
// Created by zippermonkey on 2020/10/29.
//

#ifndef STOPWAIT_SRRDTRECEIVER_H
#define STOPWAIT_SRRDTRECEIVER_H

#include "RdtReceiver.h"

class SRRdtReceiver : public RdtReceiver
{
private:
    const int MaxNum;  // 序号范围 最大序号+1
    const int WinSize; // 接收窗口大小
    int recvbase;      // 接收窗口的第一个位置
    Packet *recvBuf;   // 接收缓存
    bool *isreceived;  // 之前是否接到过
    Packet ack;        // 发送的ack包

public:
    void receive(const Packet &packet); //接收报文，将被NetworkService调用
    void printwindow();

public:
    SRRdtReceiver();

    virtual ~SRRdtReceiver();
};

#endif //STOPWAIT_SRRDTRECEIVER_H
