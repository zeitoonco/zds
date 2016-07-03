/*
 * CommunicationManager.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: ajl
 */

#include "Profiles.hpp"
#include "CommunicationManager.hpp"
#include <utility/exceptions.hpp>
#include "exceptions.hpp"
#include <utility/utility.hpp>
#include <algorithm>
#include <string>
#include <mutex>
#include <thread>
#include <unordered_map>

namespace zeitoon {
namespace _core {

using namespace zeitoon::utility;
using namespace std;

#define lockg(m) unique_lock<mutex> lock(mutexes[m])
#define lockgc(n, m) unique_lock<mutex> n(mutexes[m])

void CommunicationManager::registerCommand(CommandProfile profile) {
	lockg(MTXCommandList);
	auto res = commandList.emplace(profile.name, profile);
	if (!res.second)
		EXTduplicateName("a Command with name '" + profile.name + "' exist");
}

bool CommunicationManager::removeCommand(CommandProfile profile) {
	return removeCommand(profile.name);
}

bool CommunicationManager::removeCommand(string name) {
	lockg(MTXCommandList);
	return commandList.erase(name);
}

void CommunicationManager::registerEvent(EventProfile profile) {
	lockg(MTXEventList);
	auto res = eventList.emplace(profile.name, profile);
	if (!res.second)
		EXTduplicateName("a Event with name '" + profile.name + "' exist");
	{
		lockg(MTXHookEvent);
		//new entry in hookedEvents list
		vector<string> temp;
		auto res2 = hookedEvents.emplace(profile.name, temp);
		if (!res2.second)
			EXTunknownException("inserting into hookedEvents list failed");
		//check if any hook need this
		for (auto &hook : hookList)
			if (hook.second.eventName.compare(profile.name) == 0)
				hookedEvents.at(profile.name).push_back(hook.second.uniqString());
	}
}

bool CommunicationManager::removeEvent(EventProfile profile) {
	return removeEvent(profile.name);
}

bool CommunicationManager::removeEvent(string name) {
	lockg(MTXEventList);
	{
		lockg(MTXHookEvent);
		hookedEvents.erase(name);
	}
	return eventList.erase(name);
}

void CommunicationManager::registerHook(HookProfile profile) {
	lockg(MTXHookList);
	auto res = hookList.emplace(profile.uniqString(), profile);
	if (!res.second)
		EXTduplicateName(
				"a Hook with name '" + profile.extension + "' attached to '"
				+ profile.eventName + "' exist");
	{
		lockg(MTXHookEvent);
		//new entry in hookedEvents list
		auto event = hookedEvents.find(profile.eventName);
		if (event != hookedEvents.end())
			event->second.push_back(profile.uniqString());
	}
}

bool CommunicationManager::removeHook(HookProfile profile) {
	return removeHook(profile.uniqString());
}

bool CommunicationManager::removeHook(string name) {
	lockg(MTXHookList);
	auto profile = hookList.find(name);
	if (profile != hookList.end()) {
		lockg(MTXHookEvent);
		//remove entry in hookedEvents list
		auto event = hookedEvents.find(profile->second.eventName);
		if (event != hookedEvents.end()) {
			auto hook = find(event->second.begin(), event->second.end(), profile->second.uniqString());
			if (hook != event->second.end())
				event->second.erase(hook);
		}
	} else
		return false;
	return hookList.erase(name);
}

void CommunicationManager::fireEvent(string eventName, string &data, string from) {
	lockg(MTXHookEvent);
	auto event = hookedEvents.find(eventName);
	if (event != hookedEvents.end()) {
		for (auto &hookId : event->second) {
			try {
				HookProfile &hook = hookList.at(hookId);
				lock.unlock();
				sendFunc(hook.extension, from, hook.eventName, data, MessageTypes::MTFire, "", "");
			} catch (...) {
				//todo: log fire fail
				lError("LOG:Firing hook failed. "+eventName+ ">"+hookId);
			}
		}
	} else
		EXTnamesMismatch("No event named '" + eventName + "' is registered");
}

string CommunicationManager::callCommandSync(string cmdName, string &data, string from, string id, string sessionid) {
	string cid = callCommand(cmdName, data, from, id, sessionid);

	idData x = {"", false};
	try {
		lock_guard<mutex> lg(MtxIdList);
		idList[cid] = &x;
	} catch (std::exception ex) {
		EXTunknownExceptionI("unable to add to id-list");
	}
	while (!x.set) {
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}
	string dt = x.data;
	try {
		lock_guard<mutex> lg(MtxIdList);
		idList.erase(cid);
	} catch (std::exception &ex) {
		EXTunknownExceptionI("unable to remove from id-list");
	}
	return dt;
}

string CommunicationManager::callCommand(string cmdName, string &data, string from, string id, string sessionid) {
	lockg(MTXCommandList);
	auto cmd = commandList.find(cmdName); //todo:case insensitive in command name search?! how?
	if (cmd != commandList.end()) {
		CallbackProfile cbp(cmd->second, from, id);
		if (id.length() > 0) { // no id, no callback
			lockg(MTXCallback);
			auto res = callbackList.emplace(cbp.uniqId(), cbp);
			if (!res.second) {//exist, rewrite!
				callbackList.erase(cbp.uniqId());
				res = callbackList.emplace(cbp.uniqId(), cbp);
				if (!res.second)
					EXTunknownException("Cant store callback, failed on rewrite!");
			}
		}
		lock.unlock();
		sendFunc(cmd->second.extension, from, cmd->second.name, data, MessageTypes::MTCall, cbp.uniqId(), sessionid);
		return cbp.uniqId();
	} else
		EXTnamesMismatch("No command named '" + cmdName + "' is registered");
}

void CommunicationManager::callCallback(string id, string &data, string from) {
	lockg(MTXCallback);
	auto clb = callbackList.find(id);
	if (clb != callbackList.end()) {
		//in sync list?
		if (idList.find(id) != idList.end()) {
			lock_guard<mutex> lg(MtxIdList);
			idData *x = idList[id];
			x->data = data;
			x->set = true;
		} else {            //else, send it
			sendFunc(clb->second.extension, from, clb->second.command.name, data, MessageTypes::MTCallback,
			         clb->second.identity, "");
		}
		callbackList.erase(id);
	} else
		EXTnamesMismatch("No callback with id '" + id + "' is registered");
}

void CommunicationManager::callCallbackError(string &data, string from) {
	JStruct jdata(data);
	string id = jdata["id"].getValue();
	lockg(MTXCallback);
	auto clb = callbackList.find(id);
	if (clb != callbackList.end()) {
		//in sync list?
		if (idList.find(id) != idList.end()) {
			lock_guard<mutex> lg(MtxIdList);
			idList.erase(id);//todo:check mediator. does meditor erase cb on error?
			EXTexceptionRedirect("Command failed.\n" + jdata["description"].getValue());
		} else {            //else, send it
			((JVariable &) jdata["id"]).setValue(clb->second.identity);
			string rdata = jdata.toString();
			sendFunc(clb->second.extension, from, "error", rdata, MessageTypes::MTCall,
			         clb->second.identity, "");
		}
		callbackList.erase(id);
	} else
		EXTnamesMismatch("No callback with id '" + id + "' is registered");
}

CommandProfile CommunicationManager::getCommand(string name) {
	lockg(MTXCommandList);
	auto cmd = commandList.find(name);
	if (cmd != commandList.end())
		return cmd->second;
	else
		EXTnamesMismatch("No command named '" + name + "' is registered");
}

CallbackProfile CommunicationManager::getCallback(string name) {
	lockg(MTXCallback);
	auto cmd = callbackList.find(name);
	if (cmd != callbackList.end())
		return cmd->second;
	else
		EXTnamesMismatch("No callback with id '" + name + "' is registered");
}

EventProfile CommunicationManager::getEvent(string name) {
	lockg(MTXEventList);
	auto cmd = eventList.find(name);
	if (cmd != eventList.end())
		return cmd->second;
	else
		EXTnamesMismatch("No Event named '" + name + "' is registered");
}

HookProfile CommunicationManager::getHook(string name) {
	lockg(MTXHookList);
	auto cmd = hookList.find(name);
	if (cmd != hookList.end())
		return cmd->second;
	else
		EXTnamesMismatch("No Hook named '" + name + "' is registered");
}

void CommunicationManager::cleanup(ExtensionProfile *ext) {
	vector<string> names;
	if (commandList.size() > 0) {
		lockg(MTXCommandList);
		for (auto &i:commandList)
			if (streq(i.second.extension, ext->serviceInfo.name.getValue()))
				names.push_back(i.first);
		for (string n:names)
			commandList.erase(n);
	}
	if (callbackList.size() > 0) {
		lockg(MTXCallback);
		for (auto &i:callbackList)
			if (streq(i.second.extension, ext->serviceInfo.name.getValue()))
				names.push_back(i.first);
		for (string n:names)
			callbackList.erase(n);
	}
	if (eventList.size() > 0) {
		lockg(MTXEventList);
		for (auto &i:eventList)
			if (streq(i.second.extension, ext->serviceInfo.name.getValue()))
				names.push_back(i.first);
		lockgc(lock2, MTXHookEvent);
		for (string n:names) {
			eventList.erase(n);
			hookedEvents.erase(n);
		}
	}
	if (hookList.size() > 0) {
		lockg(MTXHookList);
		for (auto &i:hookList)
			if (streq(i.second.extension, ext->serviceInfo.name.getValue()))
				names.push_back(i.first);
		lockgc(lock2, MTXHookEvent);
		for (string n:names) {
			hookList.erase(n);
			for (auto &j:hookedEvents)
				for (vector<string>::iterator k = j.second.begin(); k < j.second.end(); k++)
					j.second.erase(k);
		}

	}
}


}//_core
}//zeitoon
