/*
 * CommandProfile
 *
 *  Created on: Sep 1, 2015
 *      Author: ajl
 */

#ifndef COMMANDPROFILE_HPP
#define COMMANDPROFILE_HPP

#include <string>
#include <utility/DTStructs.hpp>
#include <chrono>
using namespace std;

namespace zeitoon {
namespace _core {

class CommandProfile {
public:
	CommandProfile(string ext, string iname) : extension(ext), name(iname), inputDatatype(""), inputDatatypeVersion(0),
	                                           outputDatatype(""), outputDatatypeVersion(0) { }

	string extension;
	string name;
	string inputDatatype;
	int inputDatatypeVersion;
	string outputDatatype;
	int outputDatatypeVersion;
};

class HookProfile {
public:
	string eventName;
	string extension;
	//string callbackName;
	string datatype;
	int datatypeVersion;

	string uniqString() {
		return eventName + "#" + extension;
	}
};

class EventProfile {
public:
	string extension;
	string name;
	string datatype;
	int datatypeVersion;
};

class CallbackProfile {
public:
	CommandProfile &command;
	string identity;
	string extension;
	std::chrono::system_clock::time_point callStartTime;
	string uniqId() {
		return extension + "#" + identity;
	}

	CallbackProfile(CommandProfile &cmd, string ext, string id) :
			command(cmd), identity(id), extension(ext) {
	}
};


class ExtensionProfile {
public:
	enum class cepState{
		notRegistered = 0, registered, upgrading,
	};
	enum class extensionState {
		unknown = 0, notInstalled, installing, installed, enabling, upgrading, enabled,
	};

	extensionState state;
	bool requirementsSatisfied;
	cepState CEPermissionsRegistered;
	ssize_t netClientId;
	std::chrono::system_clock::time_point pingStart;
	string installID;

	datatypes::DSInstallInfo serviceInfo;
private:
	ExtensionProfile(extensionState sstate, ssize_t netid) :
			serviceInfo("", "", 0, 0, datatypes::EnmServiceType::other){
		netClientId = netid;
		state = sstate;
		requirementsSatisfied = false;
		CEPermissionsRegistered = cepState::notRegistered;
	}

public:
	ExtensionProfile(std::string serviceInfoJson, extensionState sstate, ssize_t netid = -1)
			: ExtensionProfile(sstate, netid) {
		if (serviceInfoJson.size() > 0)
			serviceInfo.fromString(serviceInfoJson);
	}

	ExtensionProfile(std::string name, int version, extensionState sstate, ssize_t netid = -1)
			: ExtensionProfile(sstate, netid) {
		serviceInfo.name = name;
		serviceInfo.serviceVersion = version;
	}

	bool isConnected() { return netClientId != -1; }

	bool isRunning() { return (state == extensionState::enabled) && isConnected() && (requirementsSatisfied == true); }
};
}
}

#endif /* COMMANDPROFILE_HPP */
