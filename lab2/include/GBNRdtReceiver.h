//
// Created by zippermonkey on 2020/10/28.
//

#ifndef STOPWAIT_GBNRDTRECIVER_H
#define STOPWAIT_GBNRDTRECIVER_H
#include "RdtReceiver.h"
class GBNRdtReceiver : public RdtReceiver
{
private:
    int expectSequenceNumberRcvd;	// 期待收到的下一个报文序号
    Packet lastAckPkt;				//上次发送的确认报文
    const static int MaxNum = 8;

public:
    GBNRdtReceiver();
    virtual ~GBNRdtReceiver();

public:

    void receive(const Packet &packet);	//接收报文，将被NetworkService调用
};

#endif //STOPWAIT_GBNRDTRECIVER_H
