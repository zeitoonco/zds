/*
 * ServerMediator.h
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#ifndef MEDIATOR_SERVERMEDIATOR_HPP_
#define MEDIATOR_SERVERMEDIATOR_HPP_

#include "utility/utility.hpp"
#include "network/TCPClient.hpp"
#include "DatabaseMediator.hpp"
#include "SettingMediator.hpp"
#include "CommunicationMediator.hpp"

namespace zeitoon {
namespace utility {

class CommunicationHandlerInterface;

class ServerMediator {
	friend class CommunicationMediator;
	private:
	CommunicationHandlerInterface *owner;
	TCPClient tcpc;

public:
	CommunicationMediator communication;
	SettingMediator setting;
	DatabaseMediator database;

	ServerMediator(CommunicationHandlerInterface* chi);
	ServerMediator(CommunicationHandlerInterface* chi,
			string address, int port);
	~ServerMediator();

	void connect();
	void connect(string address, int port);
	void disconnect();
	bool isConnected();

	void dataReceived(string data);
//	static void dataReceivedRouter(void* owner, string data) {
//		((ServerMediator*) owner)->dataReceived(data);
//	}

	void sendCmd(string node, string id, string data);
	void send(string data);

	string getNameAndType() {
		return "ServerMediator";
	}
};

} /* namespace utility */
} /* namespace zeitoon */

#endif /* MEDIATOR_SERVERMEDIATOR_HPP_ */
