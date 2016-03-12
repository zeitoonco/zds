/*
 * CommunicationManager
 *
 *  Created on: Sep 1, 2015
 *      Author: ajl
 */

#ifndef COMMUNICATIONMANAGER_HPP
#define COMMUNICATIONMANAGER_HPP

#include <string>
#include <mutex>
#include <thread>
#include <unordered_map>
#include "Profiles.hpp"
#include <functional>
#include <vector>
#include "utility/utility.hpp"
#include "utility/jsonParser.hpp"


using namespace std;
using namespace zeitoon::utility;

namespace zeitoon {
namespace _core {

class CommunicationManager {

public:
	typedef std::function<void(string,string,string,string&,MessageTypes::MessageTypes_,string)> sendFuncDLG;
	sendFuncDLG sendFunc;

	unordered_map<string, EventProfile> eventList;
	unordered_map<string, HookProfile> hookList;
	unordered_map<string, CommandProfile> commandList;
	unordered_map<string, vector<string>> hookedEvents;
	unordered_map<string, CallbackProfile> callbackList;
	mutex mutexes[5];

	enum mutexNames {
		MTXEventList, MTXHookList, MTXCommandList, MTXHookEvent, MTXCallback
	};
public:
	CommunicationManager(sendFuncDLG ptr) {
		sendFunc=ptr;
		string localcmd[] = { "_core.registerCommand", "_core.registerEvent", "_core.registerHook", "_core.removeCommand",
				"_core.removeEvent", "_core.removeHook" }; //todo:automate this stupid process
		for (int i = 0; i < 6; i++)
			registerCommand( CommandProfile("_core", localcmd[i]));
	}

	void registerCommand(CommandProfile profile);
	bool removeCommand(CommandProfile profile);
	bool removeCommand(string name);

	void registerEvent(EventProfile profile);
	bool removeEvent(EventProfile profile);
	bool removeEvent(string name);

	void registerHook(HookProfile profile);
	bool removeHook(string name);
	bool removeHook(HookProfile profile);

	CommandProfile getCommand(string name);
	CallbackProfile getCallback(string name);
	EventProfile getEvent(string name);
	HookProfile getHook(string name);

	void fireEvent(string eventName, string &data, string from);
	void callCommand(string cmdName, string &data, string from, string id);
	void callCallback(string clbName, string &data, string from, string id);

	void cleanup(ExtensionProfile *ext);

	inline string getNameAndType() const{
		return "CommunicationManager";
	}

};

}
}

#endif /* COMMUNICATIONMANAGER_HPP */
