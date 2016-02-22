/*
 * CommunicationMediator.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#include "CommunicationMediator.hpp"
#include "utility/jsonParser.hpp"
#include "ServerMediator.hpp"
#include "CommunicationHandlerInterface.hpp"
#include <thread>
#include <chrono>
#include <mutex>

namespace zeitoon {
namespace utility {

void CommunicationMediator::runCommand(string name, string data, string id) {
	sm->send(CommunicationUtility::makeCommand(name, id, sm->owner->getServiceName(), data));
}
string CommunicationMediator::runCommandSync(string name, string data, string id) {
	sm->send(CommunicationUtility::makeCommand(name, id, sm->owner->getServiceName(), data));
	idData x = { "", false };
	try {
		lock_guard<mutex> lg(MtxIdList);
		idList[id] = &x;
	} catch (std::exception& ex) {
		EXTunknownExceptionI("unable to add to id-list", ex);
	}
	while (!x.set) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	string dt = x.data;
	try {
		lock_guard<mutex> lg(MtxIdList);
		idList.erase(id);
	} catch (std::exception& ex) {
		EXTunknownExceptionI("unable to remove from id-list", ex);
	}
	return dt;
}
void CommunicationMediator::runCommand(string name, string data) {
	runCommand(name, data, "");
}
string CommunicationMediator::runCommandSync(string name, string data) {
	return runCommandSync(name, data, "");
}

void CommunicationMediator::runCallback(string name, string data, string id) {
	sm->send(CommunicationUtility::makeCallback(name, id, sm->owner->getServiceName(), data));
}

void CommunicationMediator::runEvent(string name, string data) {
	sm->send(CommunicationUtility::makeEvent(name, sm->owner->getServiceName(), data));
}
void CommunicationMediator::registerEvent(string name) {
	sm->send(CommunicationUtility::makeCommand("registerEvent", "", sm->owner->getServiceName(), "{\"names\" : [\"" + name + "\"]}"));
}
void CommunicationMediator::removeEvent(string name) {
	sm->send(CommunicationUtility::makeCommand("removeEvent", "", sm->owner->getServiceName(), "{\"names\" : [\"" + name + "\"]}"));
}
void CommunicationMediator::registerCommand(string name) {
	sm->send(CommunicationUtility::makeCommand("registerCommand", "", sm->owner->getServiceName(), "{\"names\" : [\"" + name + "\"]}"));
}
void CommunicationMediator::removeCommand(string name) {
	sm->send(CommunicationUtility::makeCommand("removeCommand", "", sm->owner->getServiceName(), "{\"names\" : [\"" + name + "\"]}"));
}
void CommunicationMediator::registerHook(string name) {
	sm->send(CommunicationUtility::makeCommand("registerHook", "", sm->owner->getServiceName(), "{\"names\" : [\"" + name + "\"]}"));
}
void CommunicationMediator::removeHook(string name) {
	sm->send(CommunicationUtility::makeCommand("removeHook", "", sm->owner->getServiceName(), "{\"names\" : [\"" + name + "\"]}"));
}

void zeitoon::utility::CommunicationMediator::errorReport(std::string node, std::string id, std::string desc) {
	sm->send(
			"{\"type\" : \"call\" , \"node\" : \"error\" , \"data\" : {\"node\" : \"" + node + "\" , \"id\" : \"" + id
					+ "\" , \"description\" : \"" + desc + "\} }");

}

bool CommunicationMediator::dataReceive(string data) {
	JStruct js(data);
	string id, dt;
	try {
		id = js["id"].getValue();
		dt = js["data"].getValue();
	} catch (exceptionEx *ex) {
		return false;
	}
	if (idList.find(id) == idList.end())
		return false;
	lock_guard<mutex> lg(MtxIdList);
	idData *x = idList[id];
	x->data = dt;
	x->set = true;
	return true;
}

} /* namespace utility */
} /* namespace zeitoon */
