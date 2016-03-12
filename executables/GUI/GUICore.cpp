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

using namespace std;

namespace zeitoon {
namespace GUI {

GUICore::GUICore(std::string iBaseDir, int WSListenPort, GuiCHI *ptr) : WS(
		std::bind(&GUICore::WSDataReceived, this, std::placeholders::_1, std::placeholders::_2)), FileSys(iBaseDir),
                                                                        guiCHI(ptr) {
	WS.listen(WSListenPort);
}

void GUICore::WSDataReceived(int ID, std::string data) {
	std::cout << "\n**GUI:WS received.\tID:" << ID << "\tData: " << data << std::endl;

	JStruct jdata(data);

	if (jdata["type"] == "call")
		this->callFromClient(jdata["node"].getValue(), jdata["id"].getValue(), ID, jdata["data"].getValue());

	else if (jdata["type"] == "hook")
		this->hookFromClient(jdata["node"].getValue(), ID);

	else if (jdata["type"] == "unhook") {
		//Remove hook from ID's Events list
		for (std::map<int, std::vector<std::string>>::iterator iter = clientHooks.begin();
			//replace with finde---above
			 iter != clientHooks.end(); iter++) {
			if (iter->first == ID) {
				for (std::vector<std::string>::iterator i = iter->second.begin(); i != iter->second.end(); i++) {
					if (*i == jdata["node"].getValue()) {
						iter->second.erase(i);
						break;
					}
				}
			}
		}
		//Check to see if there are no more events of this kind, if so, send unHook to server,
		int counter = 0;
		for (std::map<int, std::vector<std::string>>::iterator iter = clientHooks.begin();
		     iter != clientHooks.end(); iter++) {
			for (std::vector<std::string>::iterator i = iter->second.begin(); i != iter->second.end(); i++) {
				if (*i == jdata["node"].getValue()) {
					counter++;
				}
			}
		}
		if (counter == 0)
			guiCHI->sm.communication.removeHook(jdata["node"].getValue());
		return;
	}

	else if (jdata["type"] == "getList") {
		{

			JStruct msgBack;
			msgBack.add("type", "list");
			msgBack.add("node", jdata["node"].getValue());
			msgBack.add("data", new JVariable(this->getList(jdata["node"].getValue())));
			WS.send(WS.ConHdlFinder(ID), msgBack.toString());

			return;
		}
	}

	else if (jdata["type"] == "getTemplate") {
		JStruct msgBack;
		msgBack.add("type", "template");
		msgBack.add("node", jdata["node"].getValue());
		msgBack.add("data", new JVariable(this->getTemplate(jdata["node"].getValue(), jdata["data"].getValue())));
		WS.send(WS.ConHdlFinder(ID), msgBack.toString());

		return;
	}
	else if (jdata["type"] == "getTemplateList") {
		JStruct msgBack;
		msgBack.add("type", "templateList");
		msgBack.add("data", new JArray(this->getTemplatesList()));
		WS.send(WS.ConHdlFinder(ID), msgBack.toString());
	}
}

stringList GUICore::getListOfLists() {
	return FileSys.listFiles(listsDirectory, false);
}

std::string GUICore::getList(std::string name) {
	return FileSys.fileRead(listsDirectory + name + listExtension);
}


void GUICore::registerList(std::string name, std::string content) {
	name = listsDirectory + name + listExtension;
	if (FileSys.fileExist(name)) {
		FileSys.fileUpdate(name, content, true);
	} else {
		FileSys.fileCreate(name, content);
	}
}

void GUICore::updateList(std::string name, std::string content) {
	name = listsDirectory + name + listExtension;
	FileSys.fileUpdate(name, content, true);
}

void GUICore::removeList(std::string name) {
	name = listsDirectory + name + listExtension;
	FileSys.fileRemove(name);
}

std::string GUICore::getTemplate(std::string templateName, std::string fileName) {
	return FileSys.fileRead(templatesDirectory + templateName + FileSystem::pathSeprator + fileName);
}

void GUICore::registerTemplate(std::string templateName, std::string fileName, std::string content) {
	if (FileSys.directoryExist(templatesDirectory + templateName)) {

	} else {
		FileSys.directoryCreate(templatesDirectory + templateName);
		FileSys.fileCreate(templatesDirectory + templateName + FileSystem::pathSeprator + fileName, content);
	}
}

void GUICore::updateTemplate(std::string templateName, std::string fileName, std::string content) {
	FileSys.fileUpdate(templatesDirectory + templateName + FileSystem::pathSeprator + fileName,
	                   content); ///to be tested!
}

void GUICore::removeTemplate(std::string templateName, std::string fileName) {
	FileSys.fileRemove(templatesDirectory + templateName + FileSystem::pathSeprator + fileName);
}

stringList GUICore::getTemplatesList() {
	return FileSys.listDirectory(templatesDirectory);
}

stringList GUICore::getTemplateFilesList(std::string templateName) {
	return FileSys.listFiles(templatesDirectory + templateName, true);
}

void GUICore::hookFromClient(std::string EvntName, int clientID) { //incomplete
	guiCHI->sm.communication.registerHook(EvntName);
	clientHooks[clientID].push_back(EvntName);///TODO:Should be testet---[]operatorr may overwrite the key

}

void GUICore::callFromClient(std::string CmdName, std::string cmdID, int clientID, std::string data) { //incomplete
	guiCHI->sm.communication.runCommand(CmdName, data, cmdID);
	clientCmds[clientID].push_back(cmdID);//TODO:Should be testet---[]operatorr may overwrite the key
}

void GUICore::callBackReceived(std::string cmdID, std::string data) {
	JStruct Jtemp;
	Jtemp.add("type", "callback");
	Jtemp.add("node", cmdID);
	Jtemp.add("data", data);
	for (std::map<int, std::vector<string>>::iterator iter = clientCmds.begin(); iter != clientCmds.end(); iter++) {
		for (std::vector<std::string>::iterator i = iter->second.begin(); i != iter->second.end(); i++) {
			if (!Strings::compare(*i, cmdID, false)) {
				WS.send(WS.ConHdlFinder(iter->first), Jtemp.toString());
				break;//TODO: usingg break would be more efficient if there is no need to iterate anymore.ASK ALI
			}


		}
	}

}

void GUICore::eventReceived(std::string name, std::string data) {
	JStruct Jtemp;
	Jtemp.add("type", "event");
	Jtemp.add("node", name);
	Jtemp.add("data", data);
	for (std::map<int, std::vector<string>>::iterator iter = clientHooks.begin(); iter != clientHooks.end(); iter++) {
		for (std::vector<std::string>::iterator i = iter->second.begin(); i != iter->second.end(); i++) {
			if (!Strings::compare(*i, name, false)) {
				WS.send(WS.ConHdlFinder(iter->first), Jtemp.toString());
				break;//TODO: usingg break would be more efficient if there is no need to iterate anymore.ASK ALI
			}
		}
	}
}

}


}
