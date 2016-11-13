/*
 * Source.cc
 *
 *  Created on: 11. nov. 2016
 *      Author: Aljaz
 */

#include <omnetpp.h>
#include "IP_Message_m.h"
using namespace omnetpp;


class Source : public cSimpleModule{
public:
    Source();
    ~Source();
private:
    cMessage * generateMsg;
    int generatedMsg;
protected:
    virtual void initialize();
    virtual void handleMessage(cMessage * msg);
};
Define_Module(Source);
Source::Source(){
    generateMsg = nullptr;
}
Source::~Source(){
    cancelAndDelete(generateMsg);
}
void Source::initialize(){
    //ustavirmo sporocilo ki si ga bomo posiljali sami sebi
    generateMsg = new cMessage("generate");
    generatedMsg = 0;
    //posljemo prvo sporocilo
    scheduleAt(simTime()+2.0, generateMsg);
}
void Source::handleMessage(cMessage * msg){
    //ce dobimo sporocilo ki smo si ga sami poslali
    if(msg == generateMsg){
        generatedMsg++;
        //ustvarimo novo sporocilo ki bo poslano cez ccas
        scheduleAt(simTime()+par("interArrivalTime").doubleValue(), generateMsg);
        //ustvarimo novo sporcilo za poslat na izhod
        IP_Message * job = new IP_Message("job");
        job->setIP_destination("");
        job->setIP_source("");
        job->setContent("");
        send(job, "out");

    }
    else{
        delete(msg);
    }
}


