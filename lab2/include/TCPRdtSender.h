//
// Created by zippermonkey on 2020/10/30.
//

#ifndef LAB2_TCPRDTSENDER_H
#define LAB2_TCPRDTSENDER_H
#include "RdtSender.h"
class TCPRdtSender : public RdtSender
{
private:
    const int MaxNum;         // 最大+1
    const int WinSize;        // 窗口大小
    int nextseqnum;           // 下一个发送序号 nextseqnum
    Packet *packetWaitingAck; //已发送并等待Ack的数据包
    int base;                 // base
    int acktimes;             // 冗余ack的数目
    int lastacknum;           // 上次收到的ACK号

public:
    bool getWaitingState();
    bool send(const Message &message);  //发送应用层下来的Message，由NetworkServiceSimulator调用,如果发送方成功地将Message发送到网络层，返回true;如果因为发送方处于等待正确确认状态而拒绝发送Message，则返回false
    void receive(const Packet &ackPkt); //接受确认Ack，将被NetworkServiceSimulator调用
    void timeoutHandler(int seqNum);    //Timeout handler，将被NetworkServiceSimulator调用
    void printwindow();

public:
    TCPRdtSender();
    virtual ~TCPRdtSender();
};
#endif //LAB2_TCPRDTSENDER_H
