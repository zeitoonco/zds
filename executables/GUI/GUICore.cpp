/*
 * GUICore.cpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */
#include<string>
#include<map>
#include<vector>
#include"GUICore.hpp"
#include "GuiCHI.hpp"
#include <executables/UserManagement/UManagementUtility.hpp>

using namespace std;

namespace zeitoon {
namespace GUI {

GUICore::GUICore(int WSListenPort, GuiCHI *ptr) : WS(
		std::bind(&GUICore::WSDataReceived, this, std::placeholders::_1, std::placeholders::_2)),
                                                  guiCHI(ptr) {
	WS.listen(WSListenPort);
}
void GUICore::WSDataReceived(int ID, std::string data) {
	auto d = clients.find(ID);
	if (d == clients.end()) {
		std::cerr << "\nNO CLIENT FOUND\n";
	}
	std::cout << "\n**GUI:WS received.\tID:" << ID << "\tData: " << data << std::endl;
	clientData *cd;
	if (clients.count(ID) == 0) {//new //todo: use a onNewClient event instead!
		cd = new clientData;
		clients[ID] = cd;
		cd->id = ID;
	} else
		cd = clients.at(ID);

	JStruct jdata(data);

	if (jdata["type"] == "call") {
		this->callFromClient(jdata["node"].getValue(),
		                     (jdata.contains("id") ? jdata["id"].getValue() : ""),
		                     ID,
		                     (jdata.contains("data") ? jdata["data"].getValue() : ""),
		                     cd->sessionID);
		if (seq(jdata["node"].getValue(), usermanagement::commandInfo::logout())) {
			cd->sessionID = "";
		}
	} else if (jdata["type"] == "hook")
		this->hookFromClient(jdata["node"].getValue(), ID, cd->sessionID);

	else if (jdata["type"] == "unhook") {
		//Remove hook from ID's Events list
		for (std::vector<std::string>::iterator i = cd->clientHooks.begin(); i != cd->clientHooks.end(); i++) {
			if (*i == jdata["node"].getValue()) {
				cd->clientHooks.erase(i);
				break;
			}
		}
		//Check to see if there are no more events of this kind, if so, send unHook to server,
		int counter = 0;
		for (std::map<int, clientData *>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
			for (std::vector<std::string>::iterator i = iter->second->clientHooks.begin();
			     i != iter->second->clientHooks.end(); i++) {
				if (*i == jdata["node"].getValue()) {
					counter++;
				}//todo:: wat if there is a err while unhooking from _core?
			}
		}
		if (counter == 0)
			guiCHI->sm.communication.removeHook(jdata["node"].getValue());
		return;
	}
}

void GUICore::hookFromClient(std::string EvntName, int clientID, string session) { //incomplete
	clientData *cd = clients.at(clientID);
	auto d = clients.find(clientID);
	if (d == clients.end()) {
		std::cerr << "\nNO CLIENT FOUND\n";
	}

	guiCHI->sm.communication.registerHook(EvntName, session);
	cd->clientHooks.push_back(EvntName);
}

void GUICore::callFromClient(std::string CmdName, std::string cmdID, int clientID, std::string data, string session) {
//todo:incomplete
	if (cmdID == "")
		cmdID = CommunicationUtility::getRandomID();
	clientData *cd = clients.at(clientID);
	guiCHI->sm.communication.runCommand(CmdName, data, cmdID, session);
	cd->clientCmds.push_back(cmdID);
}

void GUICore::callBackReceived(std::string node, std::string cmdID, std::string data) {
	cerr << "\nCB: " << data;
	JStruct Jtemp;
	Jtemp.add("type", "callback");
	Jtemp.add("node", node);
	Jtemp.add("id", cmdID);
	Jtemp.add("data", data);//todo: clear all cb's after connection closed.
	for (std::map<int, clientData *>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
		for (int i = 0; i < iter->second->clientCmds.size(); i++) {
			if (seq(cmdID, iter->second->clientCmds[i])) {
				if (seq(node, usermanagement::commandInfo::login())) {
					JStruct dt(data);
					if (seq(dt["UMLoginResult"].getValue(), "ok"))
						iter->second->sessionID = dt["sessionID"].getValue();
				}
				cerr << "\nCB s. " << cmdID;
				WS.send(WS.ConHdlFinder(iter->first), Jtemp.toString());
				iter->second->clientCmds.erase(iter->second->clientCmds.begin() + i);
				return;
			}
		}
	}
}

void GUICore::eventReceived(std::string name, std::string data) {
	JStruct Jtemp;
	Jtemp.add("type", "event");
	Jtemp.add("node", name);
	Jtemp.add("data", data);
	for (std::map<int, clientData *>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
		for (int i = 0; i < iter->second->clientHooks.size(); i++) {
			if (seq(name, iter->second->clientHooks[i])) {
				cerr << "\nEV s. " << name;
				WS.send(WS.ConHdlFinder(iter->first), Jtemp.toString());
				return;
			}
		}
	}
}

void GUICore::errorReceived(std::string node, std::string cmdID, std::string desc) {
	JStruct Jtemp;
	Jtemp.add("type", "error");
	Jtemp.add("node", node);
	Jtemp.add("id", cmdID);
	Jtemp.add("data", desc);//todo:remove cb after its called. clear all cb's after connection closed.
	for (std::map<int, clientData *>::iterator iter = clients.begin(); iter != clients.end(); iter++) {
		for (int i = 0; i < iter->second->clientCmds.size(); i++) {
			if (seq(cmdID, iter->second->clientCmds[i])) {
				cerr << "\nERR s. " << cmdID;
				WS.send(WS.ConHdlFinder(iter->first), Jtemp.toString());
				iter->second->clientCmds.erase(iter->second->clientCmds.begin() + i);
				return;
			}
		}
	}
}

}
}
