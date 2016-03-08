/*
 * ServerMediator.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#include "ServerMediator.hpp"
#include "CommunicationHandlerInterface.hpp"

namespace zeitoon {
namespace utility {

ServerMediator::ServerMediator(CommunicationHandlerInterface *chi) :
		owner(chi), communication(this), setting(this), database(this) {
	tcpc.registerOnMessageCB(std::bind(&ServerMediator::dataReceived,this,placeholders::_1));
	tcpc.registerOnClientConnectCB(std::bind(&ServerMediator::onNetConnect,this));
	tcpc.registerOnClientDisconnectCB(std::bind(&ServerMediator::onNetDisconnect,this));
} //todo:#LT: we need auto connect constructor ( auto-detect server )

ServerMediator::ServerMediator(CommunicationHandlerInterface *chi,
                               string address, int port) :
		ServerMediator(chi) {
	connect(address, port);
}

ServerMediator::~ServerMediator() {
	disconnect();
}

void ServerMediator::connect() {
	tcpc.connect();
}

void ServerMediator::connect(string address, int port) {
	tcpc.connect(address,port);
}

void ServerMediator::disconnect() {
	tcpc.disconnect();
}

bool ServerMediator::isConnected() {
	return tcpc.isConnected();
}

void ServerMediator::dataReceived(string data) {
	JStruct js(data);
	string type = js["type"].getValue();
	string node = js["node"].getValue();
	if (!Strings::compare(type, "internal") && !Strings::compare(node, "ping")) {
		send("{\"type\" : \"internal\" , \"node\" : \"pong\" , \"id\" : \"" + js["id"].getValue() + "\"}");
	} else {
		if (!Strings::compare(type, "callback")) if (communication.dataReceive(data))
			return;
		owner->datareceive(data);
	}

}

void ServerMediator::sendCmd(string node, string id, string data) {
	send(CommunicationUtility::makeCommand(node, id, owner->getServiceName(), data));
}

void ServerMediator::send(string data) {
	tcpc.send(data);
}

void ServerMediator::onNetConnect(){
	owner->onConnect();
}
void ServerMediator::onNetDisconnect(){
	owner->onDisconnect();
}

} /* namespace utility */
} /* namespace zeitoon */
