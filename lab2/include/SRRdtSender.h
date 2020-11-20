//
// Created by zippermonkey on 2020/10/29.
//

#ifndef STOPWAIT_SRRDTSENDER_H
#define STOPWAIT_SRRDTSENDER_H
#include "RdtSender.h"
class SRRdtSender: public RdtSender
{
private:
    const int MaxNum;          // 最大序号+1
    const int WinSize;         // 发送方窗口大小
    int sendbase;              // 发送方窗口第一个位置
    int nextseqnum;            // 下一个可用于发送的序号
    bool *isacked;           // 保存未确认序号  作为packet的附加属性，标志该分组是否收到确认
    Packet *packetWaitingAck;   // 发送方缓存 用于重传


public:
    bool getWaitingState();
    bool send(const Message &message);						//发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt);						//接受确认Ack，将被NetworkServiceSimulator调用
    void timeoutHandler(int seqNum);
    void printwindow();  					//Timeout handler，将被NetworkServiceSimulator调用

public:
    SRRdtSender();
    virtual ~SRRdtSender();

};
#endif //STOPWAIT_SRRDTSENDER_H
