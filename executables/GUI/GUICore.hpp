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

struct clientData {
	int id;
	std::vector<string> clientCmds;
	std::vector<string> clientHooks;
	string sessionID;
};

class GuiCHI;

class GUICore {
	GuiCHI *guiCHI;
	WebSocketPort WS;
	int listenPort;
	std::map<int, clientData *> clients;    //clientID, string event name
public:
	GUICore(int WSListenPort, GuiCHI *PTR);

	~GUICore() {
		WS.joinListenThread();//todo:dc?
	}

	void WSDataReceived(int ID, std::string data);

	void hookFromClient(std::string EvntName, int client, string session);

	void callFromClient(std::string CmdName, std::string cmdID, int clientID, std::string data, string session);

	void callBackReceived(std::string node, std::string id, std::string data);

	void eventReceived(std::string name, std::string data);

	void errorReceived(std::string node, std::string cmdID, std::string desc);

};


}
}
#endif /* GUICORE_HPP_ */
