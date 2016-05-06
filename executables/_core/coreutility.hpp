/*
 * pgutility.hpp
 *
 *  Created on: Mar 1, 2016
 *      Author: inf
 */

#ifndef CORE_COREUTILITY_HPP_
#define CORE_COREUTILITY_HPP_

#include <string>

namespace zeitoon {
namespace _core {

class eventInfo {
public:
	static std::string onSettingUpdate() { return "_core.onSettingUpdate"; }

	static std::string onServiceConnect() { return "_core.onServiceConnect"; }

	static std::string onServiceDisconnect() { return "_core.onServiceDisconnect"; }

	static std::string onServiceEnable() { return "_core.onServiceEnable"; }

	static std::string onServiceDisable() { return "_core.onServiceDisable"; }

	static std::string onServiceInstall() { return "_core.onServiceInstall"; }

	static std::string onServiceUninstall() { return "_core.onServiceUninstall"; }

};

class commandInfo {//todo:use DTStruct or DTEnum instead?
public:

	static std::string registerCommand() { return "_core.registerCommand"; }

	static std::string removeCommand() { return "_core.removeCommand"; }

	static std::string registerEvent() { return "_core.registerEvent"; }

	static std::string removeEvent() { return "_core.removeEvent"; }

	static std::string registerHook() { return "_core.registerHook"; }

	static std::string removeHook() { return "_core.removeHook"; }

	static std::string error() { return "error"; }

	static std::string warning() { return "warning"; }

	static std::string registerSetting() { return "_core.registerSetting"; }

	static std::string removeSetting() { return "_core.removeSetting"; }

	static std::string setSetting() { return "_core.setSetting"; }

	static std::string getSetting() { return "_core.getSetting"; }

	static std::string updateSetting() { return "_core.updateSetting"; }

	static std::string resetSetting() { return "_core.resetSetting"; }

	static std::string getListOfServices() { return "_core.getListOfServices"; }

	static std::string getServiceInfo() { return "_core.getServiceInfo"; }

	static std::string installService() { return "_core.installService"; }

	static std::string uninstallService() { return "_core.uninstallService"; }

	static std::string enableService() { return "_core.enableService"; }

	static std::string disableService() { return "_core.disableService"; }

	static std::string kickService() { return "_core.kickService"; }

	static std::string pingService() { return "_core.pingService"; }

};

}
}

#endif /* CORE_COREUTILITY_HPP_ */
