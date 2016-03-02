/*
 * UMSession.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */
#include <UserManagement/UMSession.hpp>
#include "utility/exceptionex.hpp"
#include <UserManagement/UMCore.hpp>

using namespace zeitoon::utility;

namespace zeitoon {
namespace usermanagement {

UMSession::UMSession() {	//we should have a umSession start time record to arrange timeouts!!
	username = "";
	userID = -1;
	sessionID = -1;
	coreInstance = NULL;
}
UMSession::UMSession(int userIDIN, int sessionIDIN, UMCore* instance) :
		coreInstance(instance) {
	/** Proccedure:
	 *  1-Constructor-->>initializes member variables and loads corresponding permission and usergroup caches.
	 *  ** Makes a query to fetch  the username from database and initialize "string username" with it.
	 */try {
		username = coreInstance->singleFieldQuerySync("select username from users where id=" + std::to_string(userIDIN));
		//ReImplemented above! username = PQgetvalue(queryGlobal("select username from users where id=" + std::to_string(userIDIN)), 0, 0);
	} catch (exceptionEx *errorInfo) {
		coreInstance->systemLog.log(getNameAndType(), "Failed to load username. " + std::string(errorInfo->what()));
		EXTDBErrorIO("Unable to fetch username from Database", getNameAndType(), errorInfo);
	}
	userID = userIDIN;
	sessionID = sessionIDIN;
	try {
		updatePermissionsCache();
		updateUsergroups();
	} catch (exceptionEx *errorInfo) {
		EXTDBErrorIO("Cache Update Failed", this->getNameAndType(), errorInfo);
		coreInstance->systemLog.log(getNameAndType(), errorInfo->what(), LogLevels::warning);
	}
}
void UMSession::updatePermissionsCache() {
	zeitoon::datatypes::DTTableString result("");
	try {
		result = coreInstance->querySync("select permissionid, state from userpermission where userid=" + std::to_string(userID));
	} catch (exceptionEx *errorInfo) {
		coreInstance->systemLog.log(getNameAndType(), "Failed to updatePermissionCache() " + std::string(errorInfo->what()), LogLevels::warning);
		EXTDBErrorIO("Failed to updatePermissionCache", getNameAndType(), errorInfo);
	}
	if (result.rowCount() > 1) {
		for (size_t i = 0; i < result.rowCount(); i++) {
			permissionsCache[std::stoi(result.fieldValue(i, 0))] = std::stoi(result.fieldValue(i, 1));
		}
	}
	}

void UMSession::updatePermissionsCache(int permissionID) {
	zeitoon::datatypes::DTTableString result("");
	try {
		result = coreInstance->querySync(
				"select  state from userpermission where permissionid=" + std::to_string(permissionID) + " and userid=" + std::to_string(userID));
	} catch (exceptionEx *errorInfo) {
		coreInstance->systemLog.log(getNameAndType(),
				"Failed to updatePermissionCache(" + std::to_string(permissionID) + "). " + std::string(errorInfo->what()));
		EXTDBErrorIO("Failed to updatePermissionCache(int PermissionID)", getNameAndType(), errorInfo);
	}

	if (result.rowCount() == 1) {
		permissionsCache[permissionID] = std::stoi(result.fieldValue(0, 0));
	}
}

void UMSession::clearPermissionsCache() {
	permissionsCache.clear();
}

void UMSession::clearUsergroups() {
	usergruops.clear();
}
void UMSession::updateUsergroups() {
	/**Procedure:
	 * 1- makes a query to fetch all corresponding group-ids of current user from DB
	 * 2- loads the prefetched group-ids into a vector called usergroups
	 */
	zeitoon::datatypes::DTTableString result("");
	try {
		result = coreInstance->querySync("select groupid from usergroup where userid=" + std::to_string(userID));
	} catch (exceptionEx *errorInfo) {
		coreInstance->systemLog.log(getNameAndType(), "Failed to update usergroup IDs ", LogLevels::warning);
		EXTDBErrorIO("Failed to update usergroup IDs", getNameAndType(), errorInfo);
	}
	if (result.rowCount() > 0) {
		for (size_t i = 0; i < result.rowCount(); i++) {
			usergruops.push_back(std::stoi(result.fieldValue(i, 0)));
		}
	}

}
std::string UMSession::getNameAndType() {
	return "UMSession[ " + std::to_string(this->sessionID) + " : " + this->username + "(" + std::to_string(this->userID) + ") ]";

}

}		//usermanagement
}		//zeitoon

