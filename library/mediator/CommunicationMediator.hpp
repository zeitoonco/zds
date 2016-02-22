/*
 * CommunicationMediator.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#ifndef MEDIATOR_COMMUNICATIONMEDIATOR_HPP_
#define MEDIATOR_COMMUNICATIONMEDIATOR_HPP_

#include <map>
#include "utility/exceptionex.hpp"
#include <mutex>

namespace zeitoon {
namespace utility {

class ServerMediator;

class CommunicationMediator {
private:
	ServerMediator *sm;
	struct idData {
		string data;
		bool set;
	};
	string serviceName;
	map<string, idData*> idList;
	mutex MtxIdList;
	public:
	CommunicationMediator(ServerMediator *ism) :
			sm(ism) {
	}
	virtual ~CommunicationMediator() {
	}

	void runCommand(string name, string data, string id);
	string runCommandSync(string name, string data, string id);
	void runCommand(string name, string data);
	string runCommandSync(string name, string data);
	void runCallback(string name, string data, string id);
	void runEvent(string name, string data);
	void registerEvent(string name);
	void removeEvent(string name);
	void registerCommand(string name);
	void removeCommand(string name);
	void registerHook(string name);
	void removeHook(string name);
	void errorReport(std::string node, std::string id, std::string desc);
	bool dataReceive(string data);

	string getNameAndType() {
		return "CommunicationMediator";
	}
};

} /* namespace utility */
} /* namespace zeitoon */

#endif /* MEDIATOR_COMMUNICATIONMEDIATOR_HPP_ */
