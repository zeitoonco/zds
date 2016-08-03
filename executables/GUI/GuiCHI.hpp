//
// Created by inf on 3/7/16.
//

#ifndef ZDS_GUICHI_HPP
#define ZDS_GUICHI_HPP

#include <mediator/CommunicationHandlerInterface.hpp>
#include "GUICore.hpp"
#include "utility/DTStructs.hpp"
#include "GUIConfig.hpp"

namespace zeitoon {
namespace GUI {

class GuiCHI : public CommunicationHandlerInterface {
	GUICore guiCore;
	zeitoon::datatypes::DSInstallInfo insInfo;
public:
	GuiCHI(int WSListenPort)
			: CommunicationHandlerInterface(this), guiCore(WSListenPort, this),
			  insInfo("GUI", "GUI", 1, 1, datatypes::EnmServiceType::GUI) {
		setInstallInfo();
	}

	bool onCommand(string node, string data, string id, string from, std::string &resultStr);

	void onCallback(string node, string data, string id, string from, std::string success);

	void onEvent(string node, string data, string from);

	void onInstall(string id);

	void onEnable();

	void onDisable();

	void onUninstall();

	void onConnect();

	void onDisconnect();

	string getInstallInfo();

	string getInstallID();

	string getServiceName();

	size_t getServiceVersion();

	string changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion,
	                             int &newVersion);

	void onError(string node, string id, string description);

	void onWarning(string level, string node, string id, string description);

	void pong(string id, int miliseconds);

	std::string serviceID = "";

	void setInstallInfo();

	std::string getNameAndType() {
		return "GUICHI";
	}
};


}
}
#endif //ZDS_GUICHI_HPP
