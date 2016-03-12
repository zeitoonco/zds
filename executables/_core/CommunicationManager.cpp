/*
 * CommunicationManager.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: ajl
 */

#include "Profiles.hpp"
#include "CommunicationManager.hpp"
#include "utility/exceptionex.hpp"
#include "utility/utility.hpp"
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
				sendFunc(hook.extension, from, hook.eventName, data, MessageTypes::MTFire, "");
			} catch (...) {
				//todo: log fire fail
				cerr << "LOG:Firing hook failed. " << eventName << ">" << hookId;
			}
		}
	} else
		EXTnamesMismatch("No event named '" + eventName + "' is registered");
}

void CommunicationManager::callCommand(string cmdName, string &data, string from, string id) {
	lockg(MTXCommandList);
	auto cmd = commandList.find(cmdName);
	if (cmd != commandList.end()) {
		CallbackProfile cbp(cmd->second, from, id);
		if (id.length() > 0) {
			lockg(MTXCallback);
			callbackList.emplace(cbp.uniqId(), cbp);
		}
		lock.unlock();
		sendFunc(cmd->second.extension, from, cmd->second.name, data, MessageTypes::MTCall, cbp.uniqId());
	} else
		EXTnamesMismatch("No command named '" + cmdName + "' is registered");
}

void CommunicationManager::callCallback(string clbName, string &data, string from, string id) {
	lockg(MTXCallback);
	auto clb = callbackList.find(id);
	if (clb != callbackList.end()) {
		sendFunc(clb->second.extension, from, clb->second.command.name, data, MessageTypes::MTCallback,
		         clb->second.identity);
		commandList.erase(id);
	} else
		EXTnamesMismatch("No callbackList with id '" + id + "' is registered");
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
		EXTnamesMismatch("No callbackList with id '" + name + "' is registered");
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
			if (seq(i.second.extension, ext->name))
				names.push_back(i.first);
		for (string n:names)
			commandList.erase(n);
	}
	if (callbackList.size() > 0) {
		lockg(MTXCallback);
		for (auto &i:callbackList)
			if (seq(i.second.extension, ext->name))
				names.push_back(i.first);
		for (string n:names)
			callbackList.erase(n);
	}
	if (eventList.size() > 0) {
		lockg(MTXEventList);
		for (auto &i:eventList)
			if (seq(i.second.extension, ext->name))
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
			if (seq(i.second.extension, ext->name))
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
