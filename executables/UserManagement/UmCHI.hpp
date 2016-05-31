/*
 * UmCHI.hpp
 *
 *  Created on: Feb 18, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_UMCHI_HPP_
#define USERMANAGEMENT_UMCHI_HPP_

#include "utility/DTStructs.hpp"
#include <mediator/CommunicationHandlerInterface.hpp>
#include "executables/UserManagement/UMCore.hpp"
#include "UMConfig.hpp"
#include <string>

namespace zeitoon {
namespace usermanagement {

class UmCHI: public CommunicationHandlerInterface {

	zeitoon::datatypes::DSInstallInfo insInfo;
	void setInstallInfo();
	void checkDBTables();

	zeitoon::usermanagement::UMCore userMngrInterface;
public:

	UmCHI();
	void onCommand(string node, string data, string id, string from);
	void onCallback(string node, string data, string id, string from);
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
	string changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion, int &newVersion);
	void onError(string node, string id, string description);
	void onWarning(string level, string node, string id, string description);
	void pong(string id, int miliseconds);
};
}
}

#endif /* USERMANAGEMENT_UMCHI_HPP_ */
