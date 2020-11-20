//
// Created by zippermonkey on 2020/10/30.
//

#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "TCPRdtSender.h"
#include "TCPRdtReceiver.h"

int main() {
    RdtSender *ps = new TCPRdtSender();
    RdtReceiver *pr = new TCPRdtReceiver();
    pns->setRunMode(0); // VERBOS
//    pns->setRunMode(1); // 安静模式
    pns->init();
    pns->setRtdSender(ps);
    pns->setRtdReceiver(pr);
    pns->setInputFile("input.txt");
    pns->setOutputFile("output.txt");

    pns->start();

    delete ps;
    delete pr;
    delete pUtils;
    delete pns;
    return 0;
}
