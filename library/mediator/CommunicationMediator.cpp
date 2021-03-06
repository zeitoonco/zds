/*
 * CommunicationMediator.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#include "datatypes/dtmultifieldtypes.hpp"
#include "CommunicationMediator.hpp"
#include "utility/jsonParser.hpp"
#include "ServerMediator.hpp"
#include "CommunicationHandlerInterface.hpp"
#include <thread>
#include <chrono>
#include <mutex>

namespace zeitoon {
namespace utility {

void CommunicationMediator::runCommand(string name, string data, string id, string session) {
	sm->send(CommunicationUtility::makeCommand(name, id, sm->owner->getServiceName(), data, session));
}

string CommunicationMediator::runCommandSync(string name, string data, string id, string session) {
	//todo:throw error if id is empty
	idData x = {"", false};
	//try {
	std::unique_lock<mutex> lg(MtxIdList);
		idList[id] = &x;
	lg.unlock();
	sm->send(CommunicationUtility::makeCommand(name, id, sm->owner->getServiceName(), data, session));

	/*} catch (std::exception ex) {
		EXTunknownExceptionI("unable to add to id-list");
	}*/
	while (!x.set) {
		/*	if ((startTime - std::chrono::system_clock::now() > 5 ){
				todo:put a timeout for commands
			}*/

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	string dt = x.data;
	try {
		lock_guard<mutex> lg(MtxIdList);
		idList.erase(id);
	} catch (std::exception &ex) {
		EXTunknownExceptionI("unable to remove from id-list");
	}
	if (x.isException) {
		EXTrunCommandSyncFailed("Error received from Core: " + dt);
	}
	return dt;
}

void CommunicationMediator::runCommand(string name, string data, string id) {
	runCommand(name, data, id, "");
}

string CommunicationMediator::runCommandSync(string name, string data, string id) {
	return runCommandSync(name, data, id, "");
}

void CommunicationMediator::runCommand(string name, string data) {
	runCommand(name, data, utility::CommunicationUtility::getRandomID());
}

string CommunicationMediator::runCommandSync(string name, string data) {
	return runCommandSync(name, data, utility::CommunicationUtility::getRandomID());
}

void CommunicationMediator::runCallback(string name, string data, string id, bool success) {
	sm->send(CommunicationUtility::makeCallback(name, id, sm->owner->getServiceName(), data, success));
}

void CommunicationMediator::runEvent(string name, string data) {
	sm->send(CommunicationUtility::makeEvent(name, sm->owner->getServiceName(), data));
}

void CommunicationMediator::runCommand(string name, datatypes::DTStruct &data, string id) {
	this->runCommand(name, data.toString(true), id);
}

string CommunicationMediator::runCommandSync(string name, datatypes::DTStruct &data, string id) {
	return this->runCommandSync(name, data.toString(true), id);
}

void CommunicationMediator::runCommand(string name, datatypes::DTStruct &data) {
	runCommand(name, data, utility::CommunicationUtility::getRandomID());
}

string CommunicationMediator::runCommandSync(string name, datatypes::DTStruct &data) {
	return runCommandSync(name, data, utility::CommunicationUtility::getRandomID());
}

void CommunicationMediator::runCallback(string name, datatypes::DTStruct &data, string id, bool success) {
	this->runCallback(name, data.toString(true), id, success);
}

void CommunicationMediator::runEvent(string name, datatypes::DTStruct &data) {
	this->runEvent(name, data.toString(true));
}

//op
void CommunicationMediator::registerEvent(string name) {
	sm->send(CommunicationUtility::makeCommand("_core.registerEvent", "", sm->owner->getServiceName(),
	                                           "{\"names\" : [" + name + "]}"));
}

void CommunicationMediator::removeEvent(string name) {
	sm->send(CommunicationUtility::makeCommand("_core.removeEvent", "", sm->owner->getServiceName(),
	                                           "{\"names\" : [" + name + "]}"));
}

void CommunicationMediator::registerCommand(string name) {
	sm->send(CommunicationUtility::makeCommand("_core.registerCommand", "", sm->owner->getServiceName(),
	                                           "{\"names\" : [" + name + "]}"));
}

void CommunicationMediator::removeCommand(string name) {
	sm->send(CommunicationUtility::makeCommand("_core.removeCommand", "", sm->owner->getServiceName(),
	                                           "{\"names\" : [" + name + "]}"));
}

void CommunicationMediator::registerHook(string name, string session) {
	sm->send(CommunicationUtility::makeCommand("_core.registerHook", "", sm->owner->getServiceName(),
	                                           "{\"names\" : [" + name + "]}", session));
}

void CommunicationMediator::removeHook(string name) {
	sm->send(CommunicationUtility::makeCommand("_core.removeHook", "", sm->owner->getServiceName(),
	                                           "{\"names\" : [" + name + "]}"));
}

void CommunicationMediator::errorReport(std::string node, std::string id, std::string desc) {
	sm->send(CommunicationUtility::makeError(node, id, desc));
}

bool CommunicationMediator::dataReceive(string data) {
	JStruct js(data);
	string id, dt;
	try {
		id = js["id"].getValue();
		dt = js["data"].getValue();
	} catch (exceptionEx &ex) {
		lWarnig("CBID NOT FOUND. Err: " + std::string(ex.what()));
		return false;
	}
	if (idList.find(id) == idList.end()) {
		lWarnig("Callback ID not found");
			return false;
	}
	lock_guard<mutex> lg(MtxIdList);
	idData *x = idList[id];
	x->data = dt;
	x->set = true;
	return true;
}

bool CommunicationMediator::errorReceive(string data) {
	JStruct js(data);
	string id, dt;
	try {
		id = js["id"].getValue();
		dt = js["data"]["description"].getValue();
	} catch (exceptionEx &ex) {
		return false;
	}
	if (idList.find(id) == idList.end())
		return false;
	lock_guard<mutex> lg(MtxIdList);
	idData *x = idList[id];
	x->data = dt;
	x->set = true;
	x->isException = true;
	return true;

}


} /* namespace utility */
} /* namespace zeitoon */
