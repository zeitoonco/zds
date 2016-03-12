/*
 * CommandProfile
 *
 *  Created on: Sep 1, 2015
 *      Author: ajl
 */

#ifndef COMMANDPROFILE_HPP
#define COMMANDPROFILE_HPP

#include <string>

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

	string uniqId() {
		return extension + "#" + identity;
	}

	CallbackProfile(CommandProfile &cmd, string ext, string id) :
			command(cmd), identity(id), extension(ext) {
	}
};


class ExtensionProfile {
public:
	enum class extensionState {
		unknown = 0,    //before hello
		notInstalled,
		installed,
		enabled,
		disabled
	};
	enum class extensioniState {
		unknown = 0,                //before hello, connected
		notConnected,               //disconnected after hello
		requirementNotSatisfied,    //installed/Enabled but requirements not satisfied
		running                     //connected and running normally(might be a reqUnsatisfied Service, but still not installed
	};
	size_t netClientId;
	string name;
	int version;
	extensionState state;
	extensioniState istate;
	string stateDesc;
	//vector<extensionRequirmentProfile> requirments;

	/*void checkRequirments();

	 void callInstall();

	 void callUninstall();

	 void callEnable();

	 void callDisable();

	 void save();

	 void load();*/
};
}
}

#endif /* COMMANDPROFILE_HPP */
