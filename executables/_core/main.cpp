//
// Created by ajl on 3/7/16.
//

#include <iostream>
#include <cstdlib>
#include "Router.hpp"
#include "CoreConfig.hpp"

using namespace std;
using namespace zeitoon::_core;

Router *r;


void printListOfServices(Router *r);

void printListOfCommands(Router *r);

void printListOfEvents(Router *r);

void printListOfHooks(Router *r);

ExtensionProfile *inputExt() {
	string inp;
	cout << " EX:";
	cin >> inp;
	try {
		int id = stoi(inp);
		if (id >= 0 && id < r->extManager.size()) {
			if (r->extManager[id]->serviceInfo.serviceType.getValue() == zeitoon::datatypes::EnmServiceType::Core) {
				cerr << "Core is inaccessible\n";
				return NULL;
			}
			return r->extManager[id];
		} else
			cerr << "\n!! Invalid service ID.";
		return NULL;
	} catch (...) {
		ExtensionProfile *ex = r->extManager[inp];
		if (ex == NULL)
			cerr << "\n!! Invalid service Name.";
		return ex;
	}
}

void signalHandler(int s) {
	logger.log("MAIN", "Caught signal " + to_string(s), LogLevel::fatal);
	logger.flush();
	exit(1);
}

int main(int argc, char *argv[]) {
	//handle signals > ctrl+C
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = signalHandler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);
	logger.enableFile("CoreLog.log");
	logger.enableTerminalOut();
	coreConfig.load();
	logger.log("Core", "Zeitoon Server _Core Service", zeitoon::utility::LogLevel::note);
	if (argc >= 2) {
		coreConfig.listenPort = argv[1];
	} else {
		logger.log("Core", "No Parameter provided. Loading configuration",
		           zeitoon::utility::LogLevel::trace);//Iwas here
		if (coreConfig.listenPort.value().size() < 1) {
			logger.log("Core", "No parameter provided. No configuration found, Aborting...",
			           zeitoon::utility::LogLevel::fatal);
			return 0;
		}

	}

	r = new Router(std::stoi(coreConfig.listenPort.getValue()));

	if (argc == 3) {
		if (std::string(argv[2]) == "-save") {
			coreConfig.save();
			logger.log("Core", "Parameters saved",
			           zeitoon::utility::LogLevel::note);
		} else {
			logger.log("Core", "Invalid parameter provided:" + std::string(argv[2]),
			           zeitoon::utility::LogLevel::trace);
		}
	}

	if (!logger.isTerminalLog()) {
		cout << "\nZeitoon Server _Core Service\n";
	}
	cout << "\nValid commands: (installinfo[if]|install[ins]|enable[enb]|disable[dsb]|uninstall[uin]) $service\n";
	//cout << "                hello $service\n";
	cout << "                ls lscmd lsevent lshook\n";
	//cout << "Started Listening on port 5458\n";

	logger.log("Core", "Started Listening on port " + coreConfig.listenPort.getValue(),
	           zeitoon::utility::LogLevel::note);//Iwas here


	string cmd;
	int p1 = -1;
	while (cmd != "q") {
		cout << "\nCMD:\n";
		cin >> cmd;
		try {
			cmd = Strings::toLower(cmd);
			if (streq(cmd, "ping")) {
				r->ping(inputExt());
				//todo:??
			} else if (streq(cmd, "installinfo") || streq(cmd, "if")) {
				ExtensionProfile *se = inputExt();
				if (se != NULL) if (!r->getInstallInfo(se))
					cerr << "\n##InstallInfo '" + se->serviceInfo.name.getValue() + "' failed.";
			} else if (streq(cmd, "upgrade")) {
				ExtensionProfile *se = inputExt();
				if (se != NULL)
					r->upgradeService(se);
			} else if (streq(cmd, "install") || streq(cmd, "ins")) {
				ExtensionProfile *se = inputExt();
				if (se != NULL) if (!r->installService(se))
					cerr << "\n##Install '" + r->extManager[p1]->serviceInfo.name.getValue() + "' failed.";
			} else if (streq(cmd, "enable") || streq(cmd, "enb")) {
				ExtensionProfile *se = inputExt();
				if (se != NULL) if (!r->enableService(se))
					cerr << "\n##enable '" + r->extManager[p1]->serviceInfo.name.getValue() + "' failed.";
			} else if (streq(cmd, "disable") || streq(cmd, "dsb")) {
				ExtensionProfile *se = inputExt();
				if (se != NULL) if (!r->disableService(se))
					cerr << "\n##disable '" + r->extManager[p1]->serviceInfo.name.getValue() + "' failed.";
			} else if (streq(cmd, "uninstall") || streq(cmd, "uni")) {
				ExtensionProfile *se = inputExt();
				if (se != NULL) if (!r->uninstallService(se))
					cerr << "\n##uninstall '" + r->extManager[p1]->serviceInfo.name.getValue() + "' failed.";
			} else if (streq(cmd, "forceuns") || streq(cmd, "fu")) {
				ExtensionProfile *se = inputExt();
				if (se != NULL) if (!r->forceUninstallService(se))
					cerr << "\n##Force uninstall '" + r->extManager[p1]->serviceInfo.name.getValue() + "' failed.";
			}
			else if (cmd == "ls") {
				printListOfServices(r);
			} else if (cmd == "lscmd") {
				printListOfCommands(r);
			} else if (cmd == "lsevent") {
				printListOfEvents(r);
			} else if (cmd == "lshook") {
				printListOfHooks(r);
			} else {
				cerr << "\n>> INVALID COMMAND \n";
			}
		} catch (exceptionEx &ex) {
			cerr << "\nERRR:" << ex.what() << endl;
		}

	}

	return 0;
}

void printListOfServices(Router *r) {
	cout << endl << Strings::padRight("ID", 3) << Strings::padRight("Name", 30) << Strings::padRight("State", 6) <<
	Strings::padRight("NID", 5) << Strings::padRight("InstallID", 12) <<
	Strings::padRight("CEP", 4) << Strings::padRight("req", 4) << Strings::padRight("ST", 4) << endl;

	for (int i = 0; i < r->extManager.size(); i++) {
		cout << Strings::padRight(to_string(i), 3)
		<< Strings::padRight(r->extManager[i]->serviceInfo.name, 30)
		<< Strings::padRight(to_string((int) (r->extManager[i]->state)), 6)
		<< Strings::padRight(to_string(r->extManager[i]->netClientId), 5)
		<< Strings::padRight(r->extManager[i]->installID, 12)
		<< Strings::padRight(to_string((int) r->extManager[i]->CEPermissionsRegistered), 4)
		<< Strings::padRight(to_string(r->extManager[i]->requirementsSatisfied), 4)
		<< Strings::padRight(to_string(r->extManager[i]->serviceInfo.serviceType.getValue()), 4) << endl;
	}
}

void printListOfCommands(Router *r) {
	cout << endl << Strings::padRight("Name", 40) << Strings::padRight("Extension", 20) <<
	Strings::padRight("Input-Datatype", 30) << Strings::padRight("Output-Datatype", 30) << endl;

	for (unordered_map<string, CommandProfile>::iterator i = r->comm.commandList.begin();
	     i != r->comm.commandList.end(); i++) {
		cout << Strings::padRight(i->second.name, 40)
		<< Strings::padRight(i->second.extension, 20)
		<< Strings::padRight(i->second.inputDatatype + "#" + to_string(i->second.inputDatatypeVersion), 30)
		<< Strings::padRight(i->second.inputDatatype + "#" + to_string(i->second.inputDatatypeVersion), 30) << endl;
	}
}

void printListOfEvents(Router *r) {
	cout << endl << Strings::padRight("Name", 40) << Strings::padRight("Extension", 20) <<
	Strings::padRight("Datatype", 30) << endl;

	for (unordered_map<string, EventProfile>::iterator i = r->comm.eventList.begin();
	     i != r->comm.eventList.end(); i++) {
		cout << Strings::padRight(i->second.name, 40)
		<< Strings::padRight(i->second.extension, 20)
		<< Strings::padRight(i->second.datatype + "#" + to_string(i->second.datatypeVersion), 30) << endl;
	}
}

void printListOfHooks(Router *r) {
	cout << endl << Strings::padRight("Name", 40) << Strings::padRight("Extension", 20) <<
	Strings::padRight("UinqID", 30) << Strings::padRight("Datatype", 30) << endl;

	for (unordered_map<string, HookProfile>::iterator i = r->comm.hookList.begin();
	     i != r->comm.hookList.end(); i++) {
		cout << Strings::padRight(i->second.eventName, 40)
		<< Strings::padRight(i->second.extension, 20) << Strings::padRight(i->second.uniqString(), 30)
		<< Strings::padRight(i->second.datatype + "#" + to_string(i->second.datatypeVersion), 30) << endl;
	}
}