//
// Created by ajl on 2/23/16.
//

#include <iostream>
#include "utility/DTStructs.hpp"

using namespace std;
using namespace zeitoon::datatypes;


int main() {
	cout << "DS TEST\n";
	DSBoolean b;
	b.value = true;

	cout << "DSBoolean test: " << b.toString() << endl;

	DSInstallInfo ii("aService", "A Service!", 22);
	ii.commands.add(new DSInstallInfo::DSCommandDetail("cmd1", "dt1", 1, "dt2", 2), true);

	ii.fromString("{\"name\":\"aService\",\"title\":\"A Service!\",\"version\":44,\"commands\":"
			"[{\"name\":\"cmd1\",\"inputDatatype\":\"akjfhakjfh\",\"inputDatatypeVersion\":1,"
			"\"outputDatatype\":\"dt2\","
			"\"inputDatatypeVersion\":2}],\"events\":[],\"hooks\":[],\"calls\":[],\"requirements\":[],"
			"\"datatypes\":[]}");
	cout << "DSInstallInfo test: " << ii.toString();
	return 0;
}