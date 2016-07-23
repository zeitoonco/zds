/*
 * UManagementUtility.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */

#include <executables/UserManagement/UManagementUtility.hpp>
#include <executables/UserManagement/UMCore.hpp>
#include <utility/exceptions.hpp>

namespace zeitoon {
namespace usermanagement {

UMUserInfo::UMUserInfo() {
	id = 0;
	username = "";
	name = "";
	banReason = "";
	banned = true;
	coreInstance = NULL;
	isOnline = false;
}

UMUserInfo::UMUserInfo(int userID, UMCore* instance) :
		coreInstance(instance) {
	id = userID;
	zeitoon::datatypes::DTTableString result("");
	//PGresult * result = nullptr;
	banned = true;
	isOnline = false;
	try {
		result = coreInstance->querySync("select username, name, banned, banreason from users where id =" + std::to_string(userID));
	} catch (exceptionEx & errorInfo) {
		EXTDBErrorI("Unable to fetch info for UserID: " + std::to_string(userID), errorInfo);
	}
	if (result.rowCount() == 1) { //if tupple ==1 , means that it has found a match for the particular userID
		if (result.fieldValue(0, 2) == "f") { //convert *char from "RESULT" to boolean
			banned = false;
		}
		isOnline = this->coreInstance->isOnline(userID);
		id = userID;
		username = result.fieldValue(0, 0); //PQgetvalue(result, 0, 0);
		name = result.fieldValue(0, 1); //PQgetvalue(result, 0, 1);
		banReason = result.fieldValue(0, 3); //PQgetvalue(result, 0, 3);
	} else if (result.rowCount() == 0) { //if number of retreived tupples is zero, which means that userID has not found any match

	}
}

std::string UMUserInfo::getNameAndType() {
	return "UMUserInfo[" + std::to_string(id) + "]";
}

std::string UMLoginResult::typeString[UMLoginResult::__MAX] = { "ok", "invalidUserPass", "banned","accessDenied", "unknownError" };




} //
} //
