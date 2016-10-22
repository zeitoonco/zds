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
this->clearCaches();
}

int UMSessionManager::newSession(int userID, std::string uname) {

	int sessionID = 0;
	if (activeSessions.count(userID) == 1) {
		//CHECKS IF USER HAS ALREADY OPENED A SESSION ON PREVIOUS SUCCESSFUL LOGIN.
		sessionID = activeSessions.find(userID)->second;

	} else {
		sessionID = utility::CommunicationUtility::getRandomIDInt();
		try {
			auto temp = UMSession(userID, sessionID, uname, this->coreInstance);
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
				"SELECT parentid FROM groups WHERE id=" + std::to_string(groupID));
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
			userGrPtr->permissions[tempUsrGrpPermissions.fieldValueInt(j,
			                                                           0)] = tempUsrGrpPermissions.fieldValueInt(
					j,
					2);
		}
	} catch (zeitoon::utility::exceptionEx err) {
		EXTunknownException(err.what());
	}
}

void UMSessionManager::clearCaches() {
	for (auto iter:permissionCache)
		delete iter.second;
	permissionCache.empty();
	permissionNamesCache.empty();
	usergroupCache.empty();
}


void UMSessionManager::permissionCacheLoader() {
	try {//int ipermissionID, std::string iname, std::string ititle, std::string idescription, int iparentID
		permissionNamesCache.clear();
		permissionCache.clear();
		auto tempRes = this->coreInstance->querySync(
				"SELECT id, name, title, description, parentid FROM permission");
		size_t Length = tempRes.rowCount();
		for (size_t i = 0; i < Length; i++) {
			DSUpdatePermission *temp = new DSUpdatePermission(tempRes.fieldValueInt(i, 0),
			                                                  tempRes.fieldValue(i, 1),
			                                                  tempRes.fieldValue(i, 2),
			                                                  tempRes.fieldValue(i, 3),
			                                                  tempRes.fieldValueInt(i, 4, -1));
			this->permissionNamesCache[tempRes.fieldValue(i, 1)] = temp;/*new DSUpdatePermission(tempRes.fieldValueInt(i, 0),
			                                                                              tempRes.fieldValue(i, 1),
			                                                                              tempRes.fieldValue(i, 2),
			                                                                              tempRes.fieldValue(i, 3),
			                                                                              tempRes.fieldValueInt(i, 4,
			                                                                                                    -1));*/
			this->permissionCache[temp->permissiosnID.getValue()] = temp;
		}


	} catch (zeitoon::utility::exceptionEx err) {
		EXTunknownException(err.what());
	}
}


void UMSessionManager::permissionCacheUpdate(int permissionID) {
//todo by inf: could get needed info from the caller function
	size_t orgCacheSize = this->permissionNamesCache.size();
	try {
		auto temRes = coreInstance->querySync(
				"SELECT name, title, description, parentid FROM permission WHERE id= " + std::to_string(permissionID));
		if (temRes.rowCount() == 1) {
			std::string _name = temRes.fieldValue(0, 0);
			std::string _title = temRes.fieldValue(0, 1);
			std::string _description = temRes.fieldValue(0, 2);
			int _parentid = temRes.fieldValueInt(0, 3, -1);
			if (permissionCache.count(permissionID) > 0) {
				auto PID = permissionCache.at(permissionID);

				PID->parentID = _parentid;
				if (PID->name != _name)
					permissionNamesCache.erase(PID->name);
				PID->name = _name;
				PID->title = _title;
				PID->description = _description;
				permissionNamesCache[_name] = PID;

			} else {
				auto temp = new DSUpdatePermission(permissionID, _name, _title, _description, _parentid);
				permissionCache[permissionID] = temp;
				permissionNamesCache[_name] = temp;
			}
		} else if (temRes.rowCount() != 1) {
			auto temp = permissionCache.find(permissionID);
			if(temp != permissionCache.end()){
				permissionNamesCache.erase(temp->second->name);
				delete temp->second;
				permissionCache.erase(permissionID);
			}
			permissionCache.erase(permissionID);

			for (std::map<int, UMSession>::iterator iter = sessionList.begin();
			     iter != sessionList.end(); iter++) {
				iter->second.permissionsCache.erase(permissionID);
			}
			for (std::map<int, usergroupInfo>::iterator iter = usergroupCache.begin();
			     iter != usergroupCache.end(); iter++) {
				iter->second.permissions.erase(permissionID);
			}
		} else {
			EXTDBError(
					"PermissionCache update. " + std::to_string(temRes.rowCount()) + " rows returned from DB.");
			/*	*/
		}
	} catch (zeitoon::utility::exceptionEx err) {
		EXTunknownException(err.what());
	}auto temps = (int)this->permissionNamesCache.size()- (int)orgCacheSize;
	lDebug("Permission cache updated. size: " + std::to_string(this->permissionNamesCache.size())
	       +"    "+std::to_string(std::abs(temps))+ " item " + (temps > 0 ? "added": "removed"));
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


std::string UMSessionManager::getNameAndType() {
	return "[UMSessionManager]";
}

}//usermanagement
}//zeitoon
