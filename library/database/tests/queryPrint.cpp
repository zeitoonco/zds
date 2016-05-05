#include <iostream>
#include "database/sqliteDatabaseHub.h"

using namespace zeitoon::datatypes;
using namespace zeitoon::utility;

int main() {
	sqliteDatabaseHub db("/home/ajl/workspaceZeitoon/zds/executables/_core/data.db3");

	DTTableString t= db.query("select * from services where name=\"test\"");

	cout<<t.toString()<<endl<<t.fieldValue(0,1);

	return 0;
}

