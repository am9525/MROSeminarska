/*
 * Sink.cc
 *
 *  Created on: 11. nov. 2016
 *      Author: Aljaz
 */

#include <omnetpp.h>

using namespace omnetpp;

class Sink : public cSimpleModule {
public:

private:
    int sinkedMsg;
protected:
    virtual void initalize();
    virtual void handleMessage(cMessage * msg);
};

Define_Module(Sink);

void Sink::initalize(){
    sinkedMsg = 0;
}
void Sink::handleMessage(cMessage * msg){
    EV << "deleting message" << endl;
    sinkedMsg++;
    delete(msg);
}

