/*
 * Router.hpp
 *
 *  Created on: Sep 5, 2015
 *      Author: ajl
 */

#ifndef CORE_ROUTER_HPP_
#define CORE_ROUTER_HPP_

#include <string>
#include <mutex>
#include <thread>
#include <unordered_map>
#include "Profiles.hpp"
#include <vector>
#include "utility/utility.hpp"
#include "utility/jsonParser.hpp"
#include "CommunicationManager.hpp"
#include "ExtensionManager.hpp"
#include "network/TCPServer.hpp"
#include "database/sqliteDatabaseHub.h"

namespace zeitoon {
namespace _core {

using namespace zeitoon::utility;

class Router {
private:
	void _onDataReceive(size_t id, std::string data);

	void _onClientDisconnect(size_t id);

	void _onClientConnect(size_t id);

public:
	ExtensionManager extManager;
	CommunicationManager comm;
	TCPServer net;
	utility::sqliteDatabaseHub db;
	ExtensionProfile *ecore;

	Router(int port);

	virtual ~Router();

	void packetReceived(string data, ExtensionProfile *ext, size_t netid);

	void sendPacket(string &data, ExtensionProfile *extension);

	void sendMessage(string extension, string source, string node, string &data, MessageTypes::MessageTypes_ msgT,
	                 string id, string session = "", std::string success = "");

	void callCommandLocal(string node, string &data, string from, string id, string session); //s

	void fireHookLocal(string node, string &data, string from); //s

	void callCallbackLocal(string node, string &data, string from, string id); //s

	void changeDatatypeVersion(string data, string extension, int vfrom, int vto);

	void populateInstallInfo();
	void registerCore();
	bool checkCoreRequirements();
	void registerCEP();
	void registerServiceCEPermissions(ExtensionProfile *ext);
	void enableIfReqSatisfied();
	void disableIfReqNotSatisfied();
	bool enableService(ExtensionProfile *exte);
	bool installService(ExtensionProfile *exte);
	void upgradeService(ExtensionProfile *ext);
	bool getInstallInfo(ExtensionProfile *exte);
	bool forceUninstallService(ExtensionProfile *ext);
	bool uninstallService(ExtensionProfile *exte);
	bool disableService(ExtensionProfile *exte);
	void ping(ExtensionProfile *exte);
	inline string getNameAndType() {
		return "Router";
	}

};

} /* namespace Core */
} /* namespace zeitoon */

#endif /* CORE_ROUTER_HPP_ */
