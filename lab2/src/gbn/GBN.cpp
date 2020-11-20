//
// Created by zippermonkey on 2020/10/28.
//

#include "Global.h"
#include "RdtSender.h"
#include "RdtReceiver.h"
#include "GBNRdtSender.h"
#include "GBNRdtReceiver.h"

int main() {
    RdtSender *ps = new GBNRdtSender();
    RdtReceiver *pr = new GBNRdtReceiver();
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