//
// Created by ajl on 3/7/16.
//

#include <iostream>
#include <cstdlib>
#include "Router.hpp"

using namespace std;
using namespace zeitoon::_core;

void printListOfServices(Router &r);

void printListOfCommands(Router &r);

void printListOfEvents(Router &r);

void printListOfHooks(Router &r);


int main() {
	Router r(5458);

	cout << "Zeitoon Server _Core Service\n";
	cout << "Valid commands: (installinfo|install|enable|disable|uninstall) $service\n";
	//cout << "                hello $service\n";
	cout << "                ls lscmd lsevent lshook\n";
	cout << "Started. Listening on port 5458\n";

	string cmd, p1, p2, p3 = "";
	while (cmd != "q") {
		cout << "\nCMD:";
		cin >> cmd;
		try {
			cmd = Strings::toLower(cmd);
			if (cmd == "ping") {
				cin >> p1;
				//todo:??
			} else if (cmd == "installinfo") {
				cin >> p1;
				r.getInstallInfo(p1);
			} else if (cmd == "install") {
				cin >> p1;
				r.installService(p1);
			} else if (cmd == "enable") {
				cin >> p1;
				r.enableService(p1);
			} else if (cmd == "disable") {
				cin >> p1;
				r.disableService(p1);
			} else if (cmd == "uninstall") {
				cin >> p1;
				r.uninstallService(p1);
			} else if (cmd == "ls") {
				printListOfServices(r);
			} else if (cmd == "lscmd") {
				printListOfCommands(r);
			} else if (cmd == "lsevent") {
				printListOfEvents(r);
			} else if (cmd == "lshook") {
				printListOfHooks(r);
			}
		} catch (exceptionEx *ex) {
			cerr << "\nERRR:" << ex->what() << endl;
		}

	}

	return 0;
}

void printListOfServices(Router &r) {
	cout << endl << Strings::padRight("Name", 30) << Strings::padRight("State", 6) <<
	Strings::padRight("NID", 5) << Strings::padRight("InstallID", 12) <<
	Strings::padRight("CEP", 4) << Strings::padRight("req", 4) << endl;

	for (int i = 0; i < r.extManager.size(); i++) {
		cout << Strings::padRight(r.extManager[i]->serviceInfo.name, 30)
		<< Strings::padRight(to_string((int) (r.extManager[i]->state)), 6)
		<< Strings::padRight(to_string(r.extManager[i]->netClientId), 5)
		<< Strings::padRight(r.extManager[i]->installID, 12)
		<< Strings::padRight(to_string(r.extManager[i]->CEPermissionsRegistered), 4)
		<< Strings::padRight(to_string(r.extManager[i]->requirementsSatisfied), 4) << endl;
	}
}

void printListOfCommands(Router &r) {
	cout << endl << Strings::padRight("Name", 40) << Strings::padRight("Extension", 20) <<
	Strings::padRight("Input-Datatype", 30) << Strings::padRight("Output-Datatype", 30) << endl;

	for (unordered_map<string, CommandProfile>::iterator i = r.comm.commandList.begin();
	     i != r.comm.commandList.end(); i++) {
		cout << Strings::padRight(i->second.name, 40)
		<< Strings::padRight(i->second.extension, 20)
		<< Strings::padRight(i->second.inputDatatype + "#" + to_string(i->second.inputDatatypeVersion), 30)
		<< Strings::padRight(i->second.inputDatatype + "#" + to_string(i->second.inputDatatypeVersion), 30) << endl;
	}
}

void printListOfEvents(Router &r) {
	cout << endl << Strings::padRight("Name", 40) << Strings::padRight("Extension", 20) <<
	Strings::padRight("Datatype", 30) << endl;

	for (unordered_map<string, EventProfile>::iterator i = r.comm.eventList.begin();
	     i != r.comm.eventList.end(); i++) {
		cout << Strings::padRight(i->second.name, 40)
		<< Strings::padRight(i->second.extension, 20)
		<< Strings::padRight(i->second.datatype + "#" + to_string(i->second.datatypeVersion), 30) << endl;
	}
}

void printListOfHooks(Router &r) {
	cout << endl << Strings::padRight("Name", 40) << Strings::padRight("Extension", 20) <<
	Strings::padRight("UinqID", 30) << Strings::padRight("Datatype", 30) << endl;

	for (unordered_map<string, HookProfile>::iterator i = r.comm.hookList.begin();
	     i != r.comm.hookList.end(); i++) {
		cout << Strings::padRight(i->second.eventName, 40)
		<< Strings::padRight(i->second.extension, 20) << Strings::padRight(i->second.uniqString(), 30)
		<< Strings::padRight(i->second.datatype + "#" + to_string(i->second.datatypeVersion), 30) << endl;
	}
}