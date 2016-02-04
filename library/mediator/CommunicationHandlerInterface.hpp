/*
 * CommunicationHandlerInterface.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#ifndef MEDIATOR_COMMUNICATIONHANDLERINTERFACE_HPP_
#define MEDIATOR_COMMUNICATIONHANDLERINTERFACE_HPP_

#include "ServerMediator.hpp"
#include <chrono>

namespace zeitoon {
namespace utility {

class CommunicationHandlerInterface {
private:
	ServerMediator sm;


public:
	CommunicationHandlerInterface(CommunicationHandlerInterface *owner, string serverIP, int serverPort) :
			sm(owner, serverIP, serverPort) {
	}
	virtual ~CommunicationHandlerInterface() {
	}

	virtual void ping() {
		sm.send("{\"type\" : \"internal\" , \"node\" : \"ping\" , \"id\" : \"" + CommunicationUtility::getRandomID() + "\"}");
		//todo:#LT: ping
	}

	virtual void runCommand(string name, string data, string id) = 0;
	virtual void runCallback(string name, string data, string id) = 0;
	virtual void runHook(string name, string data) = 0;
	virtual void onInstall() = 0;
	virtual void onEnable() = 0;
	virtual void onDisable() = 0;
	virtual void onUninstall() = 0;
	virtual void onConnect() = 0;
	virtual void onDisconnect() = 0;
	virtual string getServiceInfo() = 0;
	virtual string getServiceName()=0;
	virtual bool datareceive(string data) = 0;

	string getNameAndType() {
		return "CommunicationHandlerInterface";
	}
};

} /* namespace utility */
} /* namespace zeitoon */

#endif /* MEDIATOR_COMMUNICATIONHANDLERINTERFACE_HPP_ */
