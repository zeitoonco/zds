/*
 * ServerMediator.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#include "ServerMediator.hpp"
#include "utility/utility.hpp"
#include "CommunicationHandlerInterface.hpp"
#include "utility/jsonParser.hpp"

namespace zeitoon {
namespace utility {

ServerMediator::ServerMediator(CommunicationHandlerInterface* chi) :
		owner(chi), communication(this), setting(this), database(this) {
	net.registerReceiveFunc((void*)this,ServerMediator::dataReceivedRouter);
} //todo:#LT: Introduce auto connect constructor ( auto-detect server )

ServerMediator::ServerMediator(CommunicationHandlerInterface* chi,
		string address, int port) :
		ServerMediator(chi) {
	connect(address, port);
}

ServerMediator::~ServerMediator() {
	disconnect();
}

void ServerMediator::connect() {
	net.connect();
}
void ServerMediator::connect(string address, int port) {
	NetClient client;
	client.ip = address;
	client.port = port;
	net.connect(client);
}
void ServerMediator::disconnect() {
	net.disconnect();
}
bool ServerMediator::isConnected() {
	return net.isConnected();
}

void ServerMediator::dataReceived(string data) {
	JStruct js(data);
	string type = js["type"].getValue();
	if (!Strings::compare(type, "internal")) { //initernal
		string ctype = js["node"].getValue();
		if (!Strings::compare(type, "ping"))
			send("{\"type\" : \"internal\" , \"node\" : \"pong\" , \"id\" : \"" + js["id"].getValue() + "\"}");
	} else {		//communication
		if (!Strings::compare(type, "callback"))
			if (communication.dataReceive(data))
				return;
		owner->datareceive(data);
	}

}

void ServerMediator::sendCmd(string node, string id, string data) {
	send(CommunicationUtility::makeCommand(node, id, owner->getServiceName(), data));
}
void ServerMediator::send(string data) {
	net.send(data);
}

} /* namespace utility */
} /* namespace zeitoon */
