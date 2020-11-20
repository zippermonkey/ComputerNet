//
// Created by zippermonkey on 2020/10/29.
//

#include "Global.h"
#include "SRRdtReceiver.h"

SRRdtReceiver::SRRdtReceiver() : MaxNum(8), WinSize(4), isreceived(new bool[MaxNum]),
                                 recvbase(0), recvBuf(new Packet[MaxNum])
{
    for (int i = 0; i < Configuration::PAYLOAD_SIZE; i++)
    {
        ack.payload[i] = '.';
    }
    for (int i = 0; i < MaxNum; i++)
    {
        isreceived[i] = false;
    }
}

SRRdtReceiver::~SRRdtReceiver()
{
}

void SRRdtReceiver::receive(const Packet &packet)
{
    // 检查校验和
    int cheecSum = pUtils->calculateCheckSum(packet);
    if (cheecSum != packet.checksum)
    {
        pUtils->printPacket("接收方没有正确收到发送方的报文,数据校验错误", packet);
        return;
    }
    bool flag = false; // 标志接受到的ACK是否在窗口中
    if (recvbase < (recvbase + WinSize) % MaxNum)
        flag = (packet.seqnum >= recvbase && packet.seqnum < (recvbase + WinSize) % MaxNum);
    else
        flag = (packet.seqnum >= recvbase) || (packet.seqnum < (recvbase + WinSize) % MaxNum);

    int seqnum = packet.seqnum;
    if (flag)
    { // 分组序号在[rcvbase,rcvbase+WinSize-1]内
        // 发送ACK
        ack.checksum = 0;
        ack.seqnum = -1; // 忽略该字段
        ack.acknum = seqnum;
        ack.checksum = pUtils->calculateCheckSum(ack);
        pUtils->printPacket("接收方发送确认报文", ack);
        pns->sendToNetworkLayer(SENDER, ack); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
        if (!isreceived[seqnum])
        {
            // 如果之前没有收到  将其缓存
            isreceived[seqnum] = true;
            recvBuf[seqnum] = packet;
        }
        if (seqnum == recvbase)
        {
            cout << endl;
            cout << "接收方更新窗口前：";
            printwindow();
            cout << endl;
            for (int i = recvbase, j = 0; j < 4; j++, i = (i + 1) % MaxNum)
            {
                if (isreceived[i] == true)
                {
                    // 上交分组
                    Message msg;
                    memcpy(msg.data, recvBuf[i].payload, sizeof(recvBuf[i].payload));
                    pns->delivertoAppLayer(RECEIVER, msg);
                    // 将isreceived[i]设置为false
                    isreceived[i] = false;
                    if (isreceived[(i + 1) % MaxNum] == false)
                    {
                        // 滑动recvbase
                        recvbase = (i + 1) % MaxNum;
                        break;
                    }
                }
            }
            cout << endl;
            cout << "接收方更新窗口后：";
            printwindow();
            cout << endl;
        }
    }
    else
    { // 在[rcvbase-WinSize, rcvbase-1]内 发送ACK
        ack.checksum = 0;
        ack.seqnum = -1; // 忽略该字段
        ack.acknum = seqnum;
        ack.checksum = pUtils->calculateCheckSum(ack);
        pUtils->printPacket("接收方发送确认报文", ack);
        pns->sendToNetworkLayer(SENDER, ack); //调用模拟网络环境的sendToNetworkLayer，通过网络层发送确认报文到对方
    }

    cout << endl;
}

void SRRdtReceiver::printwindow()
{
    int i = 0;
    for (i = 0; i < MaxNum; i++)
    {
        if (recvbase == i)
            cout << "[ ";
        cout << i << ' ';
        // if (((i + 1) % MaxNum) == nextseqnum)
        //     cout << "| ";
        if (i == (recvbase + WinSize - 1) % MaxNum)
            cout << "] ";
    }
    cout << endl;
}