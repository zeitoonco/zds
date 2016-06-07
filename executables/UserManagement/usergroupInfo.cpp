/*
 * usergroupInfo.cpp

 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */
#include <executables/UserManagement/usergroupInfo.hpp>
#include <executables/UserManagement/UMCore.hpp>
#include <utility/exceptions.hpp>

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
	try {///todo:: select all from grouppermission, then for , tehn populate each group
		result = coreInstance->querySync("select permissionid, state from grouppermission where groupid=" + std::to_string(groupIDn));
	} catch (exceptionEx & errorInfo) {
		EXTDBErrorI("Unable to fetch usergroupInfo from database " + std::to_string(groupID), errorInfo);
	}

	for (size_t i = 0; i < result.rowCount(); i++) {
		permissions[std::stoi(result.fieldValue(i, 0))] = std::stoi(result.fieldValue(i, 1)); //atoi(PQgetvalue(result, i, 1));
	}
	try {
		std::string temp = coreInstance->singleFieldQuerySync(
				"SELECT parentid  FROM groups where id=" + std::to_string(groupIDn));

		this->parentID = std::stoi(temp);
	} catch (zeitoon::utility::exceptionEx err) {
		EXTDBErrorI("Unable to fetch usergroupParentID for group: " + std::to_string(groupIDn), err);
	} catch (exception) {
		this->parentID = -1;
	}
}

usergroupInfo::usergroupInfo(int groupID, int iparentID) {
	this->parentID = iparentID;
	this->groupID = groupID;

}


std::string usergroupInfo::getNameAndType() {
	return "usergroupInfo[" + std::to_string(groupID) + "]";

}

} //usermanagement
} //zeitoon

