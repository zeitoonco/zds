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
	tcpc.registerOnMessageCB(std::bind(&ServerMediator::dataReceived, this, placeholders::_1));
	tcpc.registerOnClientConnectCB(std::bind(&ServerMediator::onNetConnect, this));
	tcpc.registerOnClientDisconnectCB(std::bind(&ServerMediator::onNetDisconnect, this));
} //todo:#LT: we need auto connect constructor ( auto-detect server )

/*ServerMediator::ServerMediator(CommunicationHandlerInterface *chi,
                               string address, int port) :
		ServerMediator(chi) {
	connect(address, port);
}*/

ServerMediator::~ServerMediator() {
	//disconnect(); //fixme: pure func error, see line 74
}

void ServerMediator::connect() {
	tcpc.connect();
}

void ServerMediator::connect(string address, int port) {
	tcpc.connect(address, port);
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

	//todo: if type call  && node error ->> errReceived()
	if (streq(type, "internal") && streq(node, "ping")) {
		send("{\"type\" : \"internal\" , \"node\" : \"pong\""+ (js.contains("id") ? ", \"id\" : \"" + js["id"].getValue() + "\"" :"" )+"}");
	} else {
		if (!Strings::compare(type, "callback")) if (communication.dataReceive(data))
			return;
		if (!Strings::compare(type, "call") && streq(node, "error")) if (communication.errorReceive(data))
			return;
		owner->datareceive(js);
	}

}

void ServerMediator::sendCmd(string node, string id, string data) {
	send(CommunicationUtility::makeCommand(node, id, owner->getServiceName(), data));
}

void ServerMediator::send(string data) {
	tcpc.send(data);
}

void ServerMediator::onNetConnect() {
	owner->onConnect();
}

void ServerMediator::onNetDisconnect() {
	owner->onDisconnect();//todo: called after destruction of owner! sigabrt
}

void ServerMediator::setNetReconnectInterval(std::string jsonArray) {
	if (jsonArray.size() > 2){
		tcpc.setReconnectInterval(jsonArray);
	}
//	tcpc.setReconnectInterval();

}

std::string ServerMediator::getNetReconnectInterval() {
	return tcpc.getReconnectInterval();
}
std::string ServerMediator::getDefaultNetReconnect(){
	return tcpc.defaultReconnInterval();
}


} /* namespace utility */
} /* namespace zeitoon */
