//
// Created by zippermonkey on 2020/10/30.
//

#ifndef LAB2_TCPRDTRECEIVER_H
#define LAB2_TCPRDTRECEIVER_H

#include "RdtReceiver.h"

class TCPRdtReceiver : public RdtReceiver {
private:
    int expectSequenceNumberRcvd;    // 期待收到的下一个报文序号
    Packet lastAckPkt;                //上次发送的确认报文
    const static int MaxNum = 8;      // 序号范围 最大序号+1

public:
    TCPRdtReceiver();

    virtual ~TCPRdtReceiver();

public:
    void receive(const Packet &packet);   //接收报文，将被NetworkService调用
};

#endif //LAB2_TCPRDTRECEIVER_H
