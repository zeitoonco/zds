/*
 * UMSessionManager.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */
#include <executables/UserManagement/UMCore.hpp>
#include <executables/UserManagement/UMSessionManager.hpp>
#include <utility/exceptions.hpp>
#include "exceptions.hpp"

using namespace zeitoon::utility;

namespace zeitoon {
namespace usermanagement {


UMSessionManager::UMSessionManager(UMCore *instance) : coreInstance(instance) {
}

UMSessionManager::~UMSessionManager() {

}

int UMSessionManager::newSession(int userID) {

	int sessionID = 0;
	if (activeSessions.count(userID) == 1) {
		//CHECKS IF USER HAS ALREADY OPENED A SESSION ON PREVIOUS SUCCESSFUL LOGIN.
		sessionID = activeSessions.find(userID)->second;

	} else {
		sessionID = uniqueIdGenerator();
		try {
			auto temp = UMSession(userID, sessionID, this->coreInstance);
			sessionList[sessionID] = temp;
		} catch (zeitoon::utility::exceptionEx &err) {
			EXTloginFailI("Unable to create new Session", err);
		}
		activeSessions[userID] = sessionID;
	}


	return sessionID;
}

void UMSessionManager::removeSession(int sessionID) {
	int a = activeSessions.erase(sessionList.find(sessionID)->second.userID);
	int b = sessionList.erase(sessionID);
	if (a != 1 or b != 1) {            // make a log upon an unsuccessful session removal.
//		coreInstance->systemLog.log("SessionRemove Failed" + getNameAndType(), std::to_string((a == 1) ? b : a),
//		                            LogLevels::trace);
	}
}

void UMSessionManager::updateUsergroupCache(int groupID) {//todo:: needs a lock+
	try {
		auto tempUsrgrpINFO = coreInstance->querySync(
				"SELECT parentid FROM groups WHERE id" + std::to_string(groupID));
		if (tempUsrgrpINFO.rowCount() < 1) {
			for (auto iter = sessionList.begin(); iter != sessionList.end(); iter++) {
				for (std::vector<int>::iterator usergroupIter = iter->second.usergroups.begin();
				     usergroupIter != iter->second.usergroups.end(); usergroupIter++) {
					if (*usergroupIter == groupID) {
						iter->second.usergroups.erase(usergroupIter);
					}
				}
			}
		}
	} catch (zeitoon::utility::exceptionEx err) {
		EXTDBError("updateUsergroupCache failed. maybe a wrong groupID");
	}
	auto tempIter = usergroupCache.find(groupID);
	if (tempIter != usergroupCache.end()) {
		tempIter->second = usergroupInfo(groupID, this->coreInstance);
	} else {
		usergroupCache[groupID] = usergroupInfo(groupID, this->coreInstance);
	}

}

void UMSessionManager::userGroupCacheLoader() {//TODO: Should be called on enable
	usergroupCache.clear();
	try {
		auto tempUsrgrpINFO = coreInstance->querySync("SELECT id, parentid FROM groups ORDER BY id");
		auto tempUsrGrpPermissions = coreInstance->querySync(
				"SELECT permissionid, groupid, state FROM grouppermission order by groupid");///inja
		for (int j = 0; j < tempUsrgrpINFO.rowCount(); j++) {
			int GrID = tempUsrgrpINFO.fieldValueInt(j, 0);
			usergroupCache[GrID] = usergroupInfo(GrID, tempUsrgrpINFO.fieldValueInt(j, 1, -1));
		}
		usergroupInfo *userGrPtr = NULL;
		for (int j = 0; j < tempUsrGrpPermissions.rowCount(); j++) {
			int grpId = tempUsrGrpPermissions.fieldValueInt(j, 1);
			if (userGrPtr == NULL || userGrPtr->groupID != grpId)
				userGrPtr = &this->usergroupCache[grpId];
			userGrPtr->permissions[tempUsrGrpPermissions.fieldValueInt(j, 0)] = tempUsrGrpPermissions.fieldValueInt(j,
			                                                                                                        2);
		}
	} catch (zeitoon::utility::exceptionEx err) {
		EXTunknownException(err.what());
	}
}

void UMSessionManager::permissionCacheLoader() {
	try {
		permissionNamesCache.clear();
		permissionParentCache.clear();
		auto tempRes = this->coreInstance->querySync("SELECT id, parentid, name FROM permission");
		size_t Length = tempRes.rowCount();
		for (size_t i = 0; i < Length; i++) {
			auto perm = tempRes.fieldValueInt(i, 0);
			permissionNamesCache[tempRes.fieldValue(i, 2)] = perm;
			permissionParentCache[perm] = tempRes.fieldValueInt(i, 1, -1);
		}
	} catch (zeitoon::utility::exceptionEx err) {
		EXTunknownException(err.what());
	}
}


void UMSessionManager::permissionCacheUpdate(int permissionID) {
	try {
		auto temRes = coreInstance->querySync(
				"SELECT parentid, name FROM permission WHERE id= " + std::to_string(permissionID));
		if (temRes.rowCount() == 1) {
			permissionParentCache[permissionID] = temRes.fieldValueInt(0, 0, -1);
			permissionNamesCache[temRes.fieldValue(0, 1)] = permissionID;
		} else if (temRes.rowCount() != 1) {
			permissionParentCache.erase(permissionID);
			for (std::map<std::string, int>::iterator iter = permissionNamesCache.begin();
			     iter != permissionNamesCache.end(); iter++) {
				if (iter->second == permissionID)
					permissionNamesCache.erase(iter);
			}
			for (std::map<int, UMSession>::iterator iter = sessionList.begin();
			     iter != sessionList.end(); iter++) {
				iter->second.permissionsCache.erase(permissionID);
			}
			for (std::map<int, usergroupInfo>::iterator iter = usergroupCache.begin();
			     iter != usergroupCache.end(); iter++) {
				iter->second.permissions.erase(permissionID);
			}
		} else {
			EXTDBError("PermissionCache update. " + std::to_string(temRes.rowCount()) + " rows returned from DB.");
			/*	*/
		}
	} catch (zeitoon::utility::exceptionEx err) {
		EXTunknownException(err.what());
	}
}

int UMSessionManager::getUserIDBySession(int sessionID) {
	auto temp = this->sessionList.find(sessionID);
	if (temp != this->sessionList.end()) {
		return temp->second.userID;
	}
	return -1;
}


int UMSessionManager::getSessionIDbyUserID(int userID) {
	auto userSession = activeSessions.find(userID);
	if (userSession != activeSessions.end()) {
		return userSession->second;
	}
	return -1;
}

int UMSessionManager::uniqueIdGenerator() {
	return (int) clock();
}

std::string UMSessionManager::getNameAndType() {
	return "[UMSessionManager]";
}

}//usermanagement
}//zeitoon
