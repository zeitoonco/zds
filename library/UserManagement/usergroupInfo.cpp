/*
 * usergroupInfo.cpp

 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */
#include <UserManagement/usergroupInfo.hpp>
#include <UserManagement/UMCore.hpp>
#include "utility/exceptionex.hpp"

using namespace zeitoon::utility;

namespace zeitoon {
namespace usermanagement {

usergroupInfo::usergroupInfo() {
	groupID = 0;
	coreInstance = NULL;
}

usergroupInfo::usergroupInfo(int groupIDn, UMCore* instance) :
		coreInstance(instance) {
	/**Procedure:
	 * 1- receive groupID;
	 * 2- make a query to fetch all corresponding permissions of a particular group and its permissions state;
	 * 3- iterates through *result and populate map<int permissionID, int permissionState>Permission
	 */
	groupID = groupIDn;
	zeitoon::datatypes::DTTableString result("");
	//PGresult * result = nullptr;
	try {
		result = coreInstance->querySync("select permissionid, state from grouppermission where groupid=" + std::to_string(groupIDn));
	} catch (exceptionEx * errorInfo) {
		coreInstance->systemLog.log(getNameAndType(), "Unable to fetch usergroupInfo from database. " + std::string(errorInfo->what()), LogLevels::warning);
		EXTDBErrorIO("Unable to fetch usergroupInfo from database " + std::to_string(groupID), getNameAndType(), errorInfo);
	}

	for (int i = 0; i < result.rowCount(); i++) {
		permissions[std::stoi(result.fieldValue(i, 0))] = std::stoi(result.fieldValue(i, 1)); //atoi(PQgetvalue(result, i, 1));
	}
}
std::string usergroupInfo::getNameAndType() {
	return "usergroupInfo[" + std::to_string(groupID) + "]";

}

} //usermanagement
} //zeitoon

