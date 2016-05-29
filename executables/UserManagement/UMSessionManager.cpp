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
	/**Procedure:
	 * 0-if the userID, already has a sessionID. it just returns the existing sessionID
	 * 		and just update the corresponding caches again
	 * 		ELSE:
	 * 1-generates a unique SessionID
	 * 2-insert into sessionList<int sessionID, UMSession(creates an instance of UMSession class)>
	 * 3-iterates through vector of usergroups  in UMSession fills up usergroupCache
	 * 4-iterates through UMSession::permissionCache<int permissionID, int permissionState>
	 * 		and fills up permissionParentCache<int permissionID, int parentID>
	 * 5- returns the generated unique sessionID for Login function of UMcore.
	 */
	int sessionID = 0;
	if (activeSessions.count(userID) == 1) {
		//CHECKS IF USER HAS ALREADY OPENED A SESSION ON PREVIOUS SUCCESSFUL LOGIN.
		sessionID = activeSessions.find(userID)->second;
		/**
		 * THIS WAY: USER CAN LOG FROM MANY DIFFERENT DEVICES SIMULTANEOUSLY WITHOUT ANY LIMITATIONS.
		 * THERE TWO WAY OF HANDLING THE SITUATION.
		 *  1-IF USER ATEMPTS TO LOG FOR MORE THAN ONCE:
		 *  		**REMOVE THE EXISTING SESSIONID( removeSession(int sessionID)
		 *  		**THIS ACTION WILL KICK OUT ANY DEVICE WITH THAT PARRRTICULAR SESSIONID
		 *  2-DESIGN A SYSTEM TO KEEP TRACK OF NUMBER OF USERS PER SESSION AND REMOVE THAT SESSION IF NECESSARY:
		 *    		**DO NOT REMOVE EXISTING SESSIONID ON NEW LOGIN ATTEMPT OF A SAME USER.
		 *    		**ASSIGN THE EXISTING SESSIONID TO NEW LOGIN ATTEMP AND MAYBE MAKE A COUNTER:
		 *    		** FOR EXAMPLE IF NUMBER OF CONCURRENT USERS OF THE SAME SESSION REACHES UP TO A LIMIT
		 *    		** THEN THE SESSION CAN BE DISCARDED AND A NEW SESSIONID WILL BE PROVIDED FOR NEW LOGIN ATTEMPT
		 *
		 */
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

	for (unsigned int i = 0; i < sessionList[sessionID].usergruops.size(); i++) {
		//iterates through vector of usergroups in UMMSEsion fills up usergroupCache
		try {
			updateUsergroupCache(sessionList[sessionID].usergruops[i]);
		} catch (exceptionEx &errorInfo) {
			coreInstance->systemLog.log(getNameAndType(),
			                            "newSession: unable to update usergroupCache for userID: " +
			                            std::to_string(userID) + std::string(errorInfo.what()), LogLevels::warning);
			EXTDBErrorI("newSession: unable to update usergroupCache for userID: " + std::to_string(userID),
			             errorInfo);
		}
		//iterates through vector of usergroups in UMMSEsion fills up usergroupParentCache
		try {
			userGroupParentCacheLoader(sessionList[sessionID].usergruops[i]);
		} catch (exceptionEx &errorInfo) {
			EXTDBErrorI("newSession: unable to update usergroup-Parent-Cache", errorInfo);
		}
	}

	for (std::map<int, int>::iterator it = sessionList[sessionID].permissionsCache.begin();
	     it != sessionList[sessionID].permissionsCache.end(); it++) {
		try {
			permissionParentCacheLoader(it->first);
		} catch (exceptionEx &errorInfo) {
			EXTDBErrorIO("newSession: unable to update permissionParentCache", getNameAndType(), errorInfo);
		}
	}

	return sessionID;
}

void UMSessionManager::removeSession(int sessionID) {
	int a = activeSessions.erase(sessionList.find(sessionID)->second.userID);
	int b = sessionList.erase(sessionID);
	if (a != 1 or b != 1) {            // make a log upon an unsuccessful session removal.
		coreInstance->systemLog.log(getNameAndType(), std::to_string((a == 1) ? b : a), LogLevels::trace);
	}
}

void UMSessionManager::updateUsergroupCache(int groupID) {

	auto res = usergroupCache.insert(
			std::pair<int, usergroupInfo>(groupID, usergroupInfo(groupID, this->coreInstance)));
	if (!res.second) {
		coreInstance->systemLog.log(getNameAndType(),
		                            "Unable to load usergroup cache for " + std::to_string(groupID) +
		                            ". MAP-> INSERT PAIR FAILED", LogLevels::trace);
		EXTDBErrorO("Failed to updateUsergroupCache for " + std::to_string(groupID), getNameAndType());
	}

}

void UMSessionManager::userGroupParentCacheLoader(int groupID) {
	zeitoon::datatypes::DTTableString result("");
	try {
		result = coreInstance->querySync("select parentid from groups where id=" + std::to_string(groupID));
	} catch (exceptionEx &errorInfo) {
		coreInstance->systemLog.log(getNameAndType(),
		                            "Unable to load userGroupParentCache for groupID:" + std::to_string(groupID) +
		                            ". " + errorInfo.what(),
		                            LogLevels::warning);
		EXTDBErrorIO(" Unable to fetch GroupParent from Database. GroupID: " + std::to_string(groupID),
		             getNameAndType(), errorInfo);
	}
	if (result.rowCount() < 1) {
		return;
	}
	int tempParentID = std::stoi(result.fieldValue(0, 0));
	while (tempParentID != -1) {
		userGroupParentCache[groupID] = tempParentID;
		groupID = tempParentID;
		try {
			tempParentID = std::stoi(coreInstance->singleFieldQuerySync("select parentid from groups where id=" +
			                                                            std::to_string(groupID)));
		} catch (exceptionEx &errorInfo) {
			EXTDBErrorIO(" Unable to fetch GroupParent from Database. GroupID: " + std::to_string(groupID),
			             getNameAndType(), errorInfo);
		}

	}
	userGroupParentCache[groupID] = tempParentID;
	/*for (std::map<int, int>::iterator it = userGroupParentCache.begin(); it != userGroupParentCache.end(); it++) {
	}//TODO: why empty loop? by inf*/
}

void UMSessionManager::permissionParentCacheLoader(int permissionID) {
	zeitoon::datatypes::DTTableString result("");
	try {
		result = coreInstance->querySync("select parentid from permission where id=" + std::to_string(permissionID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorIO("Unable to fetch PermissionParent for permissionID: " + std::to_string(permissionID),
		             getNameAndType(), errorInfo);
	}

	int tempParentID = result.fieldIsNull(0, 0) ? -1 : std::stoi(result.fieldValue(0, 0));
	while (tempParentID != -1) {
		permissionParentCache[permissionID] = tempParentID;
		permissionID = tempParentID;
		try {
			result = coreInstance->querySync(
					"select parentid from permission where id=" + std::to_string(permissionID));
		} catch (exceptionEx &errorInfo) {
			EXTDBErrorIO("Unable to fetch PermissionParent for permissionID: " + std::to_string(permissionID),
			             getNameAndType(), errorInfo);
		}
		tempParentID = result.fieldIsNull(0, 0) ? -1 : std::stoi(result.fieldValue(0, 0));
	}
	permissionParentCache[permissionID] = tempParentID;
	/*for (std::map<int, int>::iterator it = permissionParentCache.begin(); it != permissionParentCache.end(); it++) {
	}//todo : what is empty loop for? by inf*/
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
