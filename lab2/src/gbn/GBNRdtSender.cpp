#include "Global.h"
#include "GBNRdtSender.h"

GBNRdtSender::GBNRdtSender() : nextseqnum(0), base(0), MaxNum(8), WinSize(4), packetWaitingAck(new Packet[8])
{
}

GBNRdtSender::~GBNRdtSender()
{
}

bool GBNRdtSender::getWaitingState()
{ // 发送阻塞
    // 当发送窗口满的时候 被阻塞  返回 true
    return (base + WinSize) % MaxNum == (nextseqnum) % MaxNum;
}

bool GBNRdtSender::send(const Message &message)
{
    // 如果发送窗口满了 就不发送  与 waitingState无关

    if (!getWaitingState())
    { // 发送窗口还有位置
        // 准备发送数据
        this->packetWaitingAck[nextseqnum].acknum = -1; // 忽略该字段
        this->packetWaitingAck[nextseqnum].seqnum = nextseqnum;
        this->packetWaitingAck[nextseqnum].checksum = 0;
        memcpy(this->packetWaitingAck[nextseqnum].payload, message.data, sizeof(message.data));
        this->packetWaitingAck[nextseqnum].checksum = pUtils->calculateCheckSum(this->packetWaitingAck[nextseqnum]);
        // 发送数据
        //        pUtils->printPacket("发送方发送报文", this->packetWaitingAck[nextseqnum]);
        cout << endl;
        cout << "发送前：";
        printwindow();
        cout << endl;
        // 启动计时器, 编号0
        if (this->base == nextseqnum)
        {
            pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
        }
        pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[nextseqnum]);
        nextseqnum = (nextseqnum + 1) % this->MaxNum;
        cout << endl;
        cout << "发送后：";
        printwindow();
        cout << endl;
        return true;
    }
    else
    {
        cout << "发送窗口已满" << endl;
        return false;
    }
}

void GBNRdtSender::receive(const Packet &ackPkt)
{

    // 检查校验和
    int checkSum = pUtils->calculateCheckSum(ackPkt);
    if (checkSum != ackPkt.checksum)
    {
        //        pUtils->printPacket("ACK错误,校验和不相等", ackPkt);
        ;
    }
    else
    {
        //        pUtils->printPacket("ACK信息", ackPkt);
        bool flag = false; // 标志接受到的ACK是否在窗口中
        if (base < (base + WinSize) % MaxNum)
            flag = (ackPkt.acknum >= base && ackPkt.acknum < (base + WinSize) % MaxNum);
        else
            flag = (ackPkt.acknum >= base) || (ackPkt.acknum < (base + WinSize) % MaxNum);
        if (flag)
        {
            cout << endl;
            cout << "收到前：";
            printwindow();
            cout << endl;
            pUtils->printPacket("ACK信息", ackPkt);

            base = (ackPkt.acknum + 1) % MaxNum; // base 移动
            if (base == nextseqnum)
                pns->stopTimer(SENDER, 0);
            else
            {
                pns->stopTimer(SENDER, 0);
                pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
            }
            cout << endl;
            cout << "收到后：";
            printwindow();
            cout << endl;
        }
    }
}

void GBNRdtSender::timeoutHandler(int seqNum)
{
    cout << "超时！！！！！" << endl;
    if (nextseqnum == base)
        return;
    else
    {
        pns->startTimer(SENDER, Configuration::TIME_OUT, 0);
        for (int i = base; i != nextseqnum; i = (i + 1) % MaxNum)
        {
            // 重发

            pns->sendToNetworkLayer(RECEIVER, this->packetWaitingAck[i]);
            //           pUtils->printPacket("因为超时将要重传的分组:", this->packetWaitingAck[i]);
        }
    }
}

void GBNRdtSender::printwindow()
{
    int i = 0;
    for (i = 0; i < MaxNum; i++)
    {
        if (base == i)
            cout << "[ ";
        cout << i << ' ';
        if (((i + 1) % MaxNum) == nextseqnum)
            cout << "| ";
        if (i == (base + WinSize - 1) % MaxNum)
            cout << "] ";
    }
    cout << endl;
}