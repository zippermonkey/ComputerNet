//
// Created by zippermonkey on 2020/10/29.
//

#include "SRRdtSender.h"
#include "Global.h"

SRRdtSender::SRRdtSender() : MaxNum(8), WinSize(4), sendbase(0),
                             nextseqnum(0), isacked(new bool[MaxNum]), packetWaitingAck(new Packet[MaxNum])
{
    for (int i = 0; i < MaxNum; i++)
        isacked[i] = false;
}

SRRdtSender::~SRRdtSender()
{
}

bool SRRdtSender::getWaitingState()
{
    // 发送方是否被阻塞  窗口满返回true 否则返回false
    return (sendbase + WinSize) % MaxNum == (nextseqnum) % MaxNum;
}

bool SRRdtSender::send(const Message &message)
{
    if (getWaitingState())
    {
        // 发送方被阻塞
        cout << "发送方窗口已经满了" << endl;

        cout << endl;
        return false;
    }
    else
    {
        // 准备好发送数据
        packetWaitingAck[nextseqnum].acknum = -1;
        packetWaitingAck[nextseqnum].seqnum = nextseqnum;
        packetWaitingAck[nextseqnum].checksum = 0;
        memcpy(packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
        packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(packetWaitingAck[nextseqnum]);

        // 发送数据
        cout << endl;
        cout << "发送前：";
        printwindow();
        cout << endl;

        pUtils->printPacket("发送方发送报文", packetWaitingAck[nextseqnum]);
        pns->sendToNetworkLayer(RECEIVER, packetWaitingAck[nextseqnum]);

        // 为该数据包启动定时器
        pns->startTimer(SENDER, Configuration::TIME_OUT, nextseqnum);

        // 设置该数据包等待ACK
        isacked[nextseqnum] = false;

        nextseqnum = (nextseqnum + 1) % MaxNum;
        cout << endl;
        cout << "发送后：";
        printwindow();
        cout << endl;
        return true;
    }
}

void SRRdtSender::receive(const Packet &ackPkt)
{
    // 对收到的ACK的处理

    int checkSum = pUtils->calculateCheckSum(ackPkt); // 计算校验和

    if (checkSum != ackPkt.checksum)
    {
        pUtils->printPacket("ACK错误,校验和不相等", ackPkt);
    }
    else
    {
        pUtils->printPacket("ACK信息", ackPkt);
        bool flag = false; // 标志接受到的ACK是否在窗口中
        if (sendbase < (sendbase + WinSize) % MaxNum)
            flag = (ackPkt.acknum >= sendbase && ackPkt.acknum < (sendbase + WinSize) % MaxNum);
        else
            flag = (ackPkt.acknum >= sendbase) || (ackPkt.acknum < (sendbase + WinSize) % MaxNum);
        if (flag)
        {
            // 关闭计时器
            pns->stopTimer(SENDER, ackPkt.acknum);
            // 设置该包为确认收到ACK
            isacked[ackPkt.acknum] = true;
            // 可能更新sendbase
            if (sendbase == ackPkt.acknum)
            {
                cout << endl;
                cout << "更新窗口前：";
                printwindow();
                cout << endl;
                // 窗口滑动到下一个未确认的地方
                for (int i = sendbase; i != nextseqnum; i = (i + 1) % MaxNum)
                {
                    if (isacked[i] == true)
                    {
                        // 重置信息
                        isacked[i] = false;
                    }
                    if (isacked[(i + 1) % MaxNum] == false)
                    {
                        // sendbase 滑动到下一个未确认的位置
                        sendbase = (i + 1) % MaxNum;
                        break;
                    }
                }
                cout << endl;
                cout << "更新窗口后：";
                printwindow();
                cout << endl;
            }
        }
    }
    cout << endl;
}

void SRRdtSender::timeoutHandler(int seqNum)
{
    // 重发分组
    pUtils->printPacket("发送方发送报文", packetWaitingAck[seqNum]);
    pns->sendToNetworkLayer(RECEIVER, packetWaitingAck[seqNum]);
    // 重启计时器
    pns->startTimer(SENDER, Configuration::TIME_OUT, seqNum);
    cout << endl;
}

void SRRdtSender::printwindow()
{
    int i = 0;
    for (i = 0; i < MaxNum; i++)
    {
        if (sendbase == i)
            cout << "[ ";
        cout << i << ' ';
        if (((i + 1) % MaxNum) == nextseqnum)
            cout << "| ";
        if (i == (sendbase + WinSize - 1) % MaxNum)
            cout << "] ";
    }
    cout << endl;
}