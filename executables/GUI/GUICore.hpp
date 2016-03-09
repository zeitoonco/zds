/*
 * GUICore.hpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */

#ifndef GUICORE_HPP_
#define GUICORE_HPP_


#include"WebSocketPort.hpp"
#include"FileSystem.hpp"
#include "utility/jsonParser.hpp"


using std::string;
using namespace zeitoon::utility;


namespace zeitoon {
namespace GUI {

class GuiCHI;

class GUICore {
	GuiCHI *guiCHI;
	const string listExtension = ".jsn";
	const string listsDirectory = "lists" + string(&FileSystem::pathSeprator, 1);
	const string templatesDirectory = "templates" + string(&FileSystem::pathSeprator, 1);
	WebSocketPort WS;
	FileSystem FileSys;
	int listenPort = 0;
	std::map<int, std::vector<string>> clientHooks;
	//clientID, string event name
	std::map<int, std::vector<string>> clientCmds;//int clientID, string cmdID
public:
	GUICore(std::string iBaseDir, int WSListenPort, GuiCHI *PTR);


	~GUICore() {
		WS.joinListenThread();
	}

	void WSDataReceived(int ID, std::string data);

	std::string getList(std::string name);

	//returns whole file as a string
	stringList getListOfLists();

	void registerList(std::string name, std::string content);

	//rewrites if the file already exists!
	void updateList(std::string name, std::string content);

	void removeList(std::string name);

	std::string getTemplate(std::string templateName, std::string fileName);

	void registerTemplate(std::string templateName, std::string fileName, std::string content);

	void updateTemplate(std::string templateName, std::string fileName, std::string content);

	void removeTemplate(std::string templateName, std::string fileName);

	stringList getTemplateFilesList(std::string templateName);

	stringList getTemplatesList();

	void hookFromClient(std::string EvntName, int client);

	void callFromClient(std::string CmdName, std::string cmdID, int clientID, std::string data);

	void callBackReceived(std::string id, std::string data);

	void eventReceived(std::string name, std::string data);


};


}
}
#endif /* GUICORE_HPP_ */
