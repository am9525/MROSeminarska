/*
 * Router.cc
 *
 *  Created on: 12. nov. 2016
 *      Author: Aljaz
 */

#include <omnetpp.h>
#include <string.h>
#include <vector>
#include <stdlib.h>


using namespace omnetpp;
using namespace std;

#include "IP_Message_m.h"

class Router : public cSimpleModule{
public:
    Router();
    ~Router();
private:
    opp_string ipAddress;
    vector<opp_string> addTable;
    vector<int> gateTable;
    cQueue msgQueue;
    IP_Message* systemMsg;
    int queueCapacity;
    int processing;
    int resources;
    int lostPackets;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage * msg) override;
};
Define_Module(Router);
Router::Router(){
    queueCapacity = 0;
    lostPackets = 0;
    resources = 0;
    processing = 0;
    addTable.clear();
    gateTable.clear();
    msgQueue.clear();
    systemMsg = nullptr;
}
Router::~Router(){

}
void Router::initialize(){
    stringstream convert;
    int Number;
    //generiraj nakljcen IP
    for(int i = 0; i < 3; i++){
        //convert.str("");
        Number = (rand()%256);
        convert << Number << ".";
        ipAddress = convert.str();
    }
    Number = (rand()%256);
    convert << Number ;
    ipAddress = convert.str();
    //ipAddress += convert.str();
    EV << "router: " << getName() << " got IP: " << ipAddress << endl;
    //intialize queue capacity
    queueCapacity = par("queueCapacity");
    resources = par("resources");
    //create routing table
    //broadcastaj svoj naslov vsem ostalim
    systemMsg = new IP_Message("startBroadcast");
    systemMsg->setIP_source("");
    systemMsg->setIP_destination("");
    systemMsg->setContent("");
    //vsak router doda svoje sosede
    scheduleAt(simTime()+1.0,systemMsg);
    //check_and_cast<myPacket *>(msg)
}
/*
if(msg == systemMsg){
    for(int i = 0; i < gateSize("routerConn"); i++){
        Router * rout = (Router *)(gate("routerConn$o", i)->getNextGate()->getOwner());
        addTable.push_back(rout->ipAddress);
        gateTable.push_back(i);
        EV << "router: " << getName() << "added IP address" << rout->ipAddress << " and gate " << i << endl;
    }
}*/
void Router::handleMessage(cMessage * msg){
    IP_Message* packet = check_and_cast<IP_Message* >(msg);
    /*
    EV << queueCapacity << endl;
    if(queueCapacity == 0){
        EV << "losing message queue is full" << endl;
        lostPackets++;
        delete(msg);
    }
    else{
        //if queue is empty there is  not need to put it in the queue can go directly into processing
        if(msgQueue.empty()){
            packet = check_and_cast<IP_Message* >(msg);
        }
        else{
            EV << "pushing message on the queue" << endl;
            //push the message on the queue
            msgQueue.insert(msg);
            queueCapacity--;
            //take the first message from the queue
            packet = check_and_cast<IP_Message* >(msgQueue.front());
            msgQueue.pop();
        }
    }*/
    //--------------------------------------------System messages that have priority--------------------------------------
    //send initial message, this is sent from source
    if(strcmp("job", packet->getName()) == 0){
        //print routing table
        /*
        EV << getName() << " routing table" << endl;
        for(int i = 0; i < addTable.size(); i++){
            EV << addTable[i] << " gate:" << gateTable[i] << endl;

        }*/
        //select random raouter to send the message to
        int selectRouter = rand()%addTable.size();
        EV<< "index of selected router :" << selectRouter << endl;
        //create new msg
        IP_Message * personalMsg = new IP_Message("personalMessage");
        //define parameters
        personalMsg->setIP_source(ipAddress.c_str());
        personalMsg->setIP_destination(addTable[selectRouter].c_str());
        personalMsg->setContent("hello");
        EV << "sending message from: " << ipAddress << " to " << addTable[4] << endl;
        send(personalMsg, "routerConn$o", gateTable[selectRouter]);
        delete(msg);
        delete(packet);
    }


    //every router sends his address to neighbouring routers
    else if(strcmp(packet->getName(),"startBroadcast") == 0){
        EV << getName() << " is starting broadcast" << endl;
        bubble("Broadcasting my own address");
        //iterate through all the out gates and send your own ip address
        for(int i = 0; i < gateSize("routerConn$o"); i++){
            //create new broadcasting message
            IP_Message * broadcastMsg = new IP_Message("broadcast");
            //define paramters of the newly created message
            broadcastMsg ->setIP_source(ipAddress.c_str());
            broadcastMsg ->setIP_destination("");
            broadcastMsg ->setContent("");
            send(broadcastMsg , "routerConn$o", i);
        }
        delete(msg);
        delete(packet);
    }
    //if we get the broadcasting message
    else if(strcmp(packet->getName(),"broadcast") == 0){
        //check if we already have this router in our table
        bool vTabeli = false;
        for(int i = 0; i < addTable.size(); i++){
            //if we already have this address in the routing table
            if(strcmp(packet->getIP_source(), addTable[i].c_str()) == 0){
                vTabeli = true;
                break;
            }
        }
        //if we already know this router delete his message
        if(vTabeli == true || strcmp(packet->getIP_source(), ipAddress.c_str()) == 0){
            //send it to sink
            send(packet, "sinkConn");
        }
        //if we don't know this new router add it to the table
        else{

            bubble("adding new router");
            EV << getName()<< "adding: " << packet->getIP_source() << " to routing table" << endl;
            addTable.push_back(packet->getIP_source());
            cGate * gate = packet->getArrivalGate();
            gateTable.push_back(gate->getIndex());
            //we forward the message to all the neigbours
            for(int i = 0; i < gateSize("routerConn$o"); i++){
                IP_Message * broadcastMsg = new IP_Message("broadcast");
                broadcastMsg->setIP_source(packet->getIP_source());
                broadcastMsg->setIP_destination("");
                broadcastMsg->setContent("");
                send(broadcastMsg, "routerConn$o", i);
            }
            delete(msg);
            delete(packet);
        }
    }else{
        if(queueCapacity == 0){
            EV << "losing message queue is full" << endl;
            lostPackets++;
            delete(msg);
        }
        else{
            msgQueue.insert(msg);
            queueCapacity--;
        }
    }
    //-----------------------------------------------------------------------------------------------------------
    //if we have enough resources and the previous message has finished processing
    if(processing < resources && strcmp("finishedProcessing", packet->getName()) == 0){
        //we are processing one more message
        processing++;

        if(strcmp("personalMessage", packet->getName()) == 0){
            //packet arrived at destination
            if(strcmp(packet->getIP_destination(), ipAddress.c_str()) == 0){
                bubble("recieved packet, sending back acknowledgment");
                EV << "packet arrived at destination from source: " << packet->getIP_source() << endl;
                EV << packet->getIP_source() << " said " << packet->getContent();
                EV << "sending it to sink, and sending back acknowledgement" << endl;
                IP_Message * ack = new IP_Message("Acknowledgment");
                ack->setIP_source(ipAddress.c_str());
                ack->setIP_destination(packet->getIP_source());
                ack->setContent("");
                for(int i = 0; i < addTable.size(); i++){
                    //find the gate where to send
                    if(strcmp(addTable[i].c_str(), packet->getIP_source()) == 0){
                        send(ack, "routerConn$o", gateTable[i]);
                        break;
                    }
                }
                //send the recieved packet to the sink
                send(packet, "sinkConn");
                //delete(packet);
            }
            else{
                //forward message to specific port
                bool found = false;
                int i;
                for(i = 0; i < addTable.size(); i++){
                    //if we find the address in the table
                    if(strcmp(addTable[i].c_str(), packet->getIP_destination()) == 0){
                        found = true;
                        break;
                    }
                }
                //if its found forward the message
                if(found == true)
                    send(packet, "routerConn$o", gateTable[i]);
                //else send the packet to sink
                else{
                    EV << "Not valid destination: " << packet->getIP_destination() << endl;
                    bubble("don't know where to send this packet");
                    send(packet, "sinkConn");
                    //delete(packet);
                }
            }

        }
        if(strcmp("Acknowledgment", packet->getName()) == 0){
            //packet arrived ad destination
            if(strcmp(packet->getIP_destination(), ipAddress.c_str()) == 0){
                bubble("recieved acknowledgment");
                EV << "recieved acknowledgment from: " << packet->getIP_source() << endl;
                send(packet, "sinkConn");
                //delete(msg);
            }
            else{
                //forward message to specific port
                bool found = false;
                int i;
                for(i= 0; i < addTable.size(); i++){
                    //if we find the address in the table
                    if(strcmp(addTable[i].c_str(), packet->getIP_destination()) == 0){
                        found = true;
                        break;
                    }
                }
                //if its found forward the message
                if(found == true)
                    send(packet, "routerConn$o", gateTable[i]);
                //else send the packet to sink
                else{
                    EV << "Not valid destination: " << packet->getIP_destination() << endl;
                    bubble("don't know where to send this packet");
                    send(packet, "sinkConn");
                    delete(packet);
                }
            }
        }
        systemMsg = new IP_Message("finishedProcessing");
        systemMsg->setIP_source("");
        systemMsg->setIP_destination("");
        systemMsg->setContent("");
        //vsak router doda svoje sosede
        scheduleAt(simTime()+5.0,systemMsg);
    }
}

