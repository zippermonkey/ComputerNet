//
// Created by zippermonkey on 2020/10/28.
//

#ifndef STOPWAIT_GBNRDTSENSER_H
#define STOPWAIT_GBNRDTSENSER_H
#include "RdtSender.h"
class GBNRdtSender : public RdtSender
{
private:
    const int MaxNum;             // 最大+1
    const int WinSize;      // 窗口大小
    int nextseqnum;	      // 下一个发送序号 nextseqnum
    Packet* packetWaitingAck;		//已发送并等待Ack的数据包
    int base;                      // base




public:

    bool getWaitingState();
    bool send(const Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用
    void timeoutHandler(int seqNum);					//Timeout handler，将被NetworkServiceSimulator调用
    void printwindow();                                  // 打印发送窗口

public:
    GBNRdtSender();
    virtual ~GBNRdtSender();
};


#endif //STOPWAIT_GBNRDTSENSER_H
