//
// Created by zippermonkey on 2020/10/29.
//

#include "Global.h"
#include "RdtReceiver.h"
#include "RdtSender.h"
#include "SRRdtReceiver.h"
#include "SRRdtSender.h"

int main()
{
    RdtSender *ps = new SRRdtSender();
    RdtReceiver *pr = new SRRdtReceiver();


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