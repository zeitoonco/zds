//
// Created by ajl on 3/7/16.
//

#include <iostream>
#include <cstdlib>
#include "Router.hpp"

using namespace std;
using namespace zeitoon::_core;

int main() {
	Router r(5458);

	cout << "Zeitoon Server _Core Service\n";
	cout << "Valid commands: ping $service\n";
	cout << "                hello $service\n";
	cout << "Started. Listening on port 5458\n";

	string cmd, p1, p2, p3 = "";
	while (cmd != "q") {
		cout << "\nCMD:";
		cin >> cmd;
		if (cmd == "ping") {
			cin >> p1;

		} else if (cmd == "install") {
			cin >> p1;
			r.sendMessage(p1, "_core", "getinstallInfo", p3, MessageTypes::MTCall, "INSTALLID");
		} else if (cmd == "enable") {
			cin >> p1;
			r.sendMessage(p1, "_core", "onenable", p3, MessageTypes::MTCall, "");
		} else if (cmd == "disable") {
			cin >> p1;
			r.sendMessage(p1, "_core", "ondisable", p3, MessageTypes::MTCall, "");
		} else if (cmd == "uninstall") {
			cin >> p1;
			r.sendMessage(p1, "_core", "onuninstall", p3, MessageTypes::MTCall, "");
			r.db.execute("delete from services where name=\"" + p1 + "\" COLLATE NOCASE");
		}
	}

	return 0;
}