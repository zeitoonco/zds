/*
 * UManagementUtility.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */

#include <UserManagement/UManagementUtility.hpp>
#include <UserManagement/UMCore.hpp>
#include "utility/exceptionex.hpp"

namespace zeitoon {
namespace usermanagement {

UMUserInfo::UMUserInfo() {
	id = 0;
	username = "";
	name = "";
	banReason = "";
	banned = true;
	coreInstance = NULL;
}

UMUserInfo::UMUserInfo(int userID, UMCore* instance) :
		coreInstance(instance) {
	id = userID;
	zeitoon::datatypes::DTTableString result("");
	//PGresult * result = nullptr;
	banned = true;

	try {
		result = coreInstance->querySync("select username, name, banned, banreason from users where id =" + std::to_string(userID));
	} catch (exceptionEx * errorInfo) {
		coreInstance->systemLog.log(getNameAndType(), "Unable to fetch info on userID: " + std::to_string(userID) + errorInfo->what(), LogLevels::warning);
		EXTDBErrorIO("Unable to fetch infor for UserID: " + std::to_string(userID), getNameAndType(), errorInfo);
	}
	if (result.rowCount() == 1) { //if tupple ==1 , means that it has found a match for the particular userID
		if (result.fieldValue(0, 2) == "f") {
			//REimplemented above --if (*(PQgetvalue(result, 0, 2)) == 'f') { //convert *char from "RESULT" to boolean
			banned = false;
		}
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

std::string UMLoginResult::typeString[UMLoginResult::__MAX] = { "ok", "invalidUserPass", "banned", "unknownError" };

//----------------------------set Command names:

std::string commandInfo::login = "userman.login";
std::string commandInfo::logout = "userman.logout";
std::string commandInfo::checkpermission = "userman.checkpermission";
std::string commandInfo::addUser = "userman.addUser";
std::string commandInfo::modifyUser = "userman.modifyUser";
std::string commandInfo::removeUser = "userman.removeUser";
std::string commandInfo::getUserInfo = "userman.getUserInfo";
std::string commandInfo::registerPermission = "userman.registerPermission";
std::string commandInfo::updatePermission = "userman.updatePermission";
std::string commandInfo::removePermission = "userman.removePermission";
std::string commandInfo::registerUsergroup = "userman.registerUsergroup";
std::string commandInfo::updateUsergroup = "userman.updateUsergroup";
std::string commandInfo::removeUsergroup = "userman.removeUsergroup";
std::string commandInfo::listUsers = "userman.listUsers";
std::string commandInfo::listUsersByGroup = "userman.listUsersByGroup";
std::string commandInfo::listPermissions = "userman.listPermissions";
std::string commandInfo::listUsergroups = "userman.listUsergroups";
std::string commandInfo::addUserUsergroup = "userman.addUserUsergroup";
std::string commandInfo::removeUserUsergroup = "userman.removeUserUsergroup";
std::string commandInfo::addUserPermission = "userman.addUserPermission";
std::string commandInfo::removeUserPermission = "userman.removeUserPermission";

//--------------------------set Event names:

std::string eventInfo::usersUsergroupAdded ="userman.usersUsergroupAdded";
std::string eventInfo::usersUsergroupRemoved ="userman.usersUsergroupRemoved";
std::string eventInfo::usersPermissionAdded ="userman.usersPermissionAdded";
std::string eventInfo::usersPermissionRemoved ="userman.usersPermissionRemoved";
std::string eventInfo::loggedIn ="userman.loggedIn";
std::string eventInfo::loggedOut ="userman.loggedOut";
std::string eventInfo::userAdded ="userman.userAdded";
std::string eventInfo::userRemoved ="userman.userRemoved";
std::string eventInfo::userModified ="userman.userModified";
std::string eventInfo::permissionAdded ="userman.permissionAdded";
std::string eventInfo::permissionModified ="userman.permissionModified";
std::string eventInfo::permissionRemoved ="userman.permissionRemoved";
std::string eventInfo::usergroupAdded ="userman.usergroupAdded";
std::string eventInfo::usergroupModified ="userman.usergroupModified";
std::string eventInfo::usergroupRemoved ="userman.usergroupRemoved";
} //
} //
