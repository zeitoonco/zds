/*
 * UMCore.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */
#include "utility/exceptionex.hpp"
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <UserManagement/UMCore.hpp>
#include "UserManagement/UmCHI.hpp"
#include "UserManagement/DSStructs.hpp"

namespace zeitoon{
namespace usermanagement{


UMCore::UMCore(UmCHI* instancePtr) :
		umCHI(instancePtr), systemLog("UserManagementLogs"), sessionManager(this) {
}
UMCore::~UMCore() {

}

UMLoginResult::UMLoginResultEnum UMCore::login(std::string username, std::string password, int& sessionID,
		std::string& desc) {
	if (userLogInfo.count(username) == 1) {
		auto currentUser = userLogInfo.find(username);
		if (currentUser->second.banned == true) {
			if ((time(NULL) - currentUser->second.Banedtime) > 600) {
				userLogInfo.erase(currentUser);
			} else {
				sessionID = -1;
				desc = "User is temporary banned(too many false login attempts)";
				systemLog.log(getNameAndType(), "Login Failed for User: [" + username + "]. User is temporarily banned. (To many false login attempts) ",
						LogLevels::note);
				return UMLoginResult::invalidUserPass;
			}
		}
	}
	zeitoon::datatypes::DTTableString loginResult("loginResult");
	std::string name = "";
	int userID = 0;
	bool banned = true;
	try {
		if (this->executeSync("select count(*) from users where username = '" + username + "'") != 1) { //now it should save the ip and ban it if needed later on! but how? ip should be passed along
			std::cout << "you are not registered" << endl;
			return UMLoginResult::unknownError;
		}
		loginResult = this->querySync(
				" select id, banned, banreason, name from users where username='" + username + "' and password='" + hashingProccess(password) + "'");
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to fetch Loging info for user:[" + username + "]. " + errorInfo->what(), LogLevels::warning);
		EXTDBErrorIO("Unable to fetch Loging info for user:" + username, getNameAndType(), errorInfo);
	}
	if (loginResult.rowCount() == 1) {
		userID = std::stoi(loginResult.fieldValue(0, 0));
		banned = (loginResult.fieldValue(0, 1) == "t" || loginResult.fieldValue(0, 1) == "1" || loginResult.fieldValue(0, 1) == "y") ? true : false;
		name = loginResult.fieldValue(0, 3);
		if (banned) {			//IF USER IS BANNED
			sessionID = -1;
			desc = loginResult.fieldValue(0, 2);
			return UMLoginResult::banned;
		} else {	//SUCCESSFUL AUTHENTICATION
			desc = "";
			try {
				sessionID = sessionManager.newSession(userID);
				umCHI->sm.communication.runEvent(eventInfo::loggedIn, zeitoon::usermanagement::DSUserInfo(userID, username, name, banned, desc).toString(true));
				//##Event fired
			} catch (exceptionEx *errorInfo) {
				EXTDBErrorIO("Login: unable to register the session for " + username, getNameAndType(), errorInfo);
				return UMLoginResult::unknownError;
			}
			systemLog.log(getNameAndType(), username + " [" + std::to_string(sessionID) + "] " + "Loged in", LogLevels::note);

			return UMLoginResult::ok;
		}
	} else if (loginResult.rowCount() == 0) {	//IF THERE IS NO MATCH FOR USER/PW/NAME
		if (userLogInfo.count(username) == 1) {
			auto currentUser = userLogInfo.find(username);
			currentUser->second.logAttemptTimes.push_back(time(NULL));
			for (unsigned int i = 0; i < currentUser->second.logAttemptTimes.size(); i++) {
				if ((time(NULL) - currentUser->second.logAttemptTimes[i]) > 60) {
					currentUser->second.logAttemptTimes.erase(currentUser->second.logAttemptTimes.begin() + i);
					i--;
				}
			}
			if (currentUser->second.logAttemptTimes.size() > 4) {
				currentUser->second.banned = true;
				std::cout << "\nBanned! Time: " << time(NULL);
				currentUser->second.Banedtime = time(NULL);
				systemLog.log(getNameAndType(),
						"User [" + username + "] is banned now(at [" + std::to_string(currentUser->second.Banedtime)
								+ "]), because of to many wrong login inputs.", LogLevels::note);

			}

		} else {
			userLogInfoStruct currentUserLogStruct;
			currentUserLogStruct.logAttemptTimes.push_back(time(NULL));
			userLogInfo[username] = currentUserLogStruct;
		}
		sessionID = -1;
		desc = "Invalid Username or Password.";
		systemLog.log(getNameAndType(), "Invalid username or password for USER: [" + username + "]. ", LogLevels::note);
		return UMLoginResult::invalidUserPass;

	} else {	//IF ANY THING OTHER THAN PREVIOUSLY DEFINED CONDITIONS HAPPEN.
		sessionID = -1;
		desc = "Unknown Error!";
		systemLog.log(getNameAndType(), "Unknown Error on login attempt for USER: [" + username + "]. ", LogLevels::trace);
		return UMLoginResult::unknownError;
	}

}
void UMCore::logout(int sessionID) {
	std::string username = sessionManager.sessionList.find(sessionID)->second.username;
	sessionManager.removeSession(sessionID);
	int userID = this->sessionManager.sessionList.at(sessionID).userID;
	umCHI->sm.communication.runEvent(eventInfo::loggedOut,
			zeitoon::usermanagement::DSUserInfo(userID, this->sessionManager.sessionList.at(sessionID).username, "", false, "").toString(
			true));
	//##Event Fired
	systemLog.log(getNameAndType(), username + " [" + std::to_string(sessionID) + "] " + "Logged out", LogLevels::note);
}
bool UMCore::checkPermission(int sessionID, int permissionID) {
	switch (checkUserPermissionState(sessionID, permissionID)) {
	case (-1):

		return false;
	case (0):
		switch (checkUserPermissionParentState(sessionID, permissionID)) {
		case (-1):
			return false;
		case (0):
			//check user group
			switch (checkUsergroupPermission(sessionID, permissionID)) {
			case (-1):
				return false;

			case (0):
				//check parent
				switch (checkUsergroupParentPermission(sessionID, permissionID)) {
				case (-1):
					return false;

				case (0):
					return false;
				case (1):
					return true;
				}
				break;
			case (1): //check parrent
				switch (checkUsergroupParentPermission(sessionID, permissionID)) {
				case (-1):
					return false;

				case (0):
					return true;

				case (1):
					return true;
				}
			}
			break;
		case (1):
			return true; ///permission parent state has top priority
		}
		break;
	case (1): //check parent
		switch (checkUserPermissionParentState(sessionID, permissionID)) {
		case (-1):
			return false;
		case (0):
			return true;
		case (1):
			return true;
		default:
			return true;
		}

		break;
	}
	return false;
}

int UMCore::addUser(std::string username, std::string password, std::string name) { //regs a new user in database
///----Reg a new user in Database:

	std::string command = "insert into users (id, username, password, name, banned, banreason, avatar) values"
			"(default, '" + username + "', '" + hashingProccess(password) + "', '" + name + "', default , default, default )";
	try {
		executeSync(command);
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to adduser[ " + username + "  ]. " + errorInfo->what(), LogLevels::warning);
		return -1;	////chi return kone?
	}
	int userID = 0;
///---- Fetch and return Atoi of the generated ID from Database:

	std::string queryForID = " select id from users where username='" + username + "' and password='" + hashingProccess(password) + "' and name='" + name + "'";
	try {
		userID = std::stoi(this->singleFieldQuerySync(queryForID));
	} catch (exceptionEx * errorInfo) {
		systemLog.log(getNameAndType(), "Unable to fetch userID of user[ " + username + " ]. " + errorInfo->what(), LogLevels::trace);
	}
	umCHI->sm.communication.runEvent(eventInfo::userAdded, zeitoon::usermanagement::DSUserInfo(userID, username, name, false, "").toString(true));
	//##Event Fired
	systemLog.log(getNameAndType(), "New user[" + username + "] added", LogLevels::note);
	return userID;
}
void UMCore::removeUser(int userID) {

	logout(sessionManager.getSessionIDbyUserID(userID));
	int executeResult = 0;
	try {
		executeResult = executeSync("delete from users where id = " + std::to_string(userID));

	} catch (exceptionEx *errorInfo) {
		std::cout << "\n" << errorInfo->what() << "\n";
		systemLog.log(getNameAndType(), "Unable to remove userID[" + std::to_string(userID) + "] from database." + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to remove userID[" + std::to_string(userID) + "] from database.", getNameAndType(), errorInfo);
		return;
	}
	if (executeResult == 1) {
		try {		//remove all permissions of user from DB
			executeSync("delete from userpermission where userid=" + std::to_string(userID));
		} catch (exceptionEx *errorInfo) {
			EXTDBErrorI("Unable to remove user's all permissions from DB", errorInfo);
		}

		umCHI->sm.communication.runEvent(eventInfo::userRemoved,
				zeitoon::usermanagement::DSUserInfo(userID, sessionManager.sessionList.at(sessionManager.getSessionIDbyUserID(userID)).username, "", false, "").toString(
				true));
		//##Event Fired
		systemLog.log(getNameAndType(), "User account[" + std::to_string(userID) + "] removed.", LogLevels::note);
	}

}
void UMCore::modifyUser(int userID, std::string username, std::string password, std::string name) {
	int executeResult = 0;
	try {
		executeResult = executeSync(
				"update users set username='" + username + "', password='" + hashingProccess(password) + "', name='" + name + "' where id="
						+ std::to_string(userID));
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to modify user[" + std::to_string(userID) + "] in database." + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to modify user[" + std::to_string(userID) + "] in database.", getNameAndType(), errorInfo);
		return;
	}
	if (executeResult == 1) {
		sessionManager.sessionList.at(userID).username = username;		//update username in umSession-->(info of active users)
		umCHI->sm.communication.runEvent(eventInfo::userModified, zeitoon::usermanagement::DSUserInfo(userID, username, name, false, "").toString(true));
		//##Event Fired
		systemLog.log(getNameAndType(), "User[" + std::to_string(userID) + "] modified.", LogLevels::note);
	}
}
UMUserInfo UMCore::getUserInfo(int ID) {
	return UMUserInfo(ID, this);
}
int UMCore::registerPermission(std::string name, std::string title, std::string desc, int parent) {
	try {
		executeSync(
				"insert into permission( id, parentid, name, title, description) values(default, " + std::to_string(parent) + ", '" + name + "', '" + title
						+ "', '" + desc + "')");
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to register permission[" + name + "(" + title + ")" + desc + "] in database" + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to register permission[" + name + "(" + title + ")" + desc + "] in database", getNameAndType(), errorInfo);
		return -1;
	}

	std::string queryForPermissionID = " select id from permission where name='" + name + "' and title='" + title + "' and description='" + desc
			+ "'and parentid=" + std::to_string(parent);

	int permissionID = 0;
	try {
		permissionID = std::stoi(this->singleFieldQuerySync(queryForPermissionID));
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to fetch ID for permission[" + name + "] from database." + std::string(errorInfo->what()), LogLevels::trace);
		EXTDBErrorIO("Unable to fetch ID for permission[" + name + "] from database.", getNameAndType(), errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::permissionAdded, zeitoon::usermanagement::DSUpdatePermission(permissionID, name, title, desc, parent).toString(true));
	//##Event Fired
	systemLog.log(getNameAndType(), "Permission[" + name + ", ID:" + std::to_string(permissionID) + "] registered.", LogLevels::note);
	return permissionID;

}
void UMCore::updatePermission(int permissionID, std::string name, std::string title, std::string desc, int parentID) {
	int executeResult = 0;
	try {
		executeResult = executeSync(
				"update permission set name='" + name + "', title='" + title + "', description='" + desc + "', parentid=" + std::to_string(parentID)
						+ " where id =" + std::to_string(permissionID));
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to update permission[ID:" + std::to_string(permissionID) + "] in database" + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to update permission[ID:" + std::to_string(permissionID) + "] in database", getNameAndType(), errorInfo);
		return;
	}
	if (executeResult == 1) {
		sessionManager.permissionParentCacheLoader(permissionID);		//reinitialize permissionParentCache with new parent ID for it.
		umCHI->sm.communication.runEvent(eventInfo::permissionModified,
				zeitoon::usermanagement::DSUpdatePermission(permissionID, name, title, desc, parentID).toString(true));
		//##Event Fired
		systemLog.log(getNameAndType(), "Permission[ID:" + std::to_string(permissionID) + " ] updated.", LogLevels::note);
	}
}
void UMCore::removePermission(int permissionID) {
	std::string permIDStr = std::to_string(permissionID);

	try {		//remove permission from users permission table in database
		executeSync("BEGIN;"
				" delete from permission where id=" + permIDStr + ";"
				" delete from permission where id=" + permIDStr + ";"
				" delete from grouppermission where permissionid=" + permIDStr + ";"
				" end;");

	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to remove permission[ID:" + std::to_string(permissionID) + "] from database." + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to remove permission[ID: " + std::to_string(permissionID) + "] from database.", getNameAndType(), errorInfo);
	}
	for (std::map<int, UMSession>::iterator iter = sessionManager.sessionList.begin(); iter != sessionManager.sessionList.end(); iter++) {
		iter->second.permissionsCache.erase(permissionID);
	}
	for (std::map<int, usergroupInfo>::iterator iter = sessionManager.usergroupCache.begin(); iter != sessionManager.usergroupCache.end(); iter++) {
		iter->second.permissions.erase(permissionID);
	}
	sessionManager.permissionParentCache.erase(permissionID);

	umCHI->sm.communication.runEvent(eventInfo::permissionRemoved, zeitoon::usermanagement::DSUpdatePermission(permissionID, "", "", "", -2).toString(true));
	//##Event Fired
	systemLog.log(getNameAndType(), "Permission[ID:" + std::to_string(permissionID) + " ] removed", LogLevels::note);
}

int UMCore::registerUsergroup(std::string title, int parentID, std::string desc) {		//is it user groups or just ||groups||??

	try {
		executeSync("insert into groups( id, title, parentid, description) values(default, '" + title + "', " + std::to_string(parentID) + ", '" + desc + "')");
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to register usergroup[" + title + "(" + desc + ")] in database. " + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to register usergroup[" + title + "] in database.", getNameAndType(), errorInfo);
		return -1;
	}
	std::string queryForGroupID = "select id from groups where title='" + title + "' and parentid=" + std::to_string(parentID) + " and description='" + desc
			+ "'";

	int usergroupID = 0;
	try {
		usergroupID = std::stoi(this->singleFieldQuerySync(queryForGroupID));
	} catch (exceptionEx * errorInfo) {
		systemLog.log(getNameAndType(), "unable to fetch groupID for [" + title + "] from database." + std::string(errorInfo->what()), LogLevels::warning);
		EXTDBErrorIO("unable to fetch groupID for [" + title + "] from database.", getNameAndType(), errorInfo);
	}
	if (usergroupID > 0) {
		umCHI->sm.communication.runEvent(eventInfo::usergroupAdded, zeitoon::usermanagement::DSUpdateUsrGrp(usergroupID, title, parentID, desc).toString(true));
		//##Event Fired
		systemLog.log(getNameAndType(), "Usergroup[ID: " + std::to_string(usergroupID) + "] registered.", LogLevels::note);
	}
	return usergroupID;
}
void UMCore::updateUsergroup(int usergroupID, std::string title, int parentID, std::string desc) {

	try {
		executeSync(
				"update groups set title='" + title + "', description='" + desc + "', parentid=" + std::to_string(parentID) + " where id="
						+ std::to_string(usergroupID));
		sessionManager.userGroupParentCache.at(usergroupID) = parentID;
		umCHI->sm.communication.runEvent(eventInfo::usergroupModified, zeitoon::usermanagement::DSUpdateUsrGrp(usergroupID, title, parentID, desc).toString(true));
		//##Event Fired
		systemLog.log(getNameAndType(), "Usergroup[ID:" + std::to_string(usergroupID) + "] modified.", LogLevels::note);
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to update usergroup[ID: " + std::to_string(usergroupID) + "]." + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to update usergroup[ID: " + std::to_string(usergroupID) + "].", getNameAndType(), errorInfo);
	}

}
void UMCore::removeUsergroup(int usergroupID) {

	std::string GrID = std::to_string(usergroupID);
	try {
		executeSync("BEGIN;"
				"delete from groups where id= " + GrID + "; delete from usergroup where groupid=" + GrID + "; end;");
		sessionManager.usergroupCache.erase(usergroupID);	// remove details from cache
		sessionManager.userGroupParentCache.erase(usergroupID);	//remove usergrp details from parent cchache
		umCHI->sm.communication.runEvent(eventInfo::usergroupRemoved, zeitoon::usermanagement::DSUpdateUsrGrp(usergroupID, "", -2, "").toString(true));
		//##Event Fired
		systemLog.log(getNameAndType(), "Usergroup[ID: " + std::to_string(usergroupID) + "] removed.", LogLevels::note);
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to remove usergroup[" + std::to_string(usergroupID) + "] from database" + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to remove usergroup[" + std::to_string(usergroupID) + "] from database", getNameAndType(), errorInfo);
	}

}
std::vector<zeitoon::datatypes::DTString> UMCore::listUsers() {
	std::vector<zeitoon::datatypes::DTString> allUsers;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = this->querySync("select name from users");
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to fetch all usernames from database" + std::string(errorInfo->what()), LogLevels::warning);
		EXTDBErrorIO("Unable to fetch all usernames from database", getNameAndType(), errorInfo);
		return allUsers;
	}

	for (size_t i = 0; i < result.rowCount(); i++) {
		zeitoon::datatypes::DTString temp("tempUsrList");
		temp.setValue(result.fieldValue(i, 0));
		allUsers.push_back(temp);
	}

	return allUsers;
}
std::vector<zeitoon::datatypes::DTString> UMCore::listUsersByGroup(int groupID) {		//recieves group id and returns all members in that group
	std::vector<zeitoon::datatypes::DTString> allUsersOfSpecificGroup;
	zeitoon::datatypes::DTTableString result("");
	try {

		result = querySync("select userid from usergroup where groupid=" + std::to_string(groupID));
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to fetch  user of group[" + std::to_string(groupID) + "] from database. " + std::string(errorInfo->what()),
				LogLevels::warning);
		EXTDBErrorIO("Unable to fetch  user of group[" + std::to_string(groupID) + "] from database", getNameAndType(), errorInfo);
		return allUsersOfSpecificGroup;
	}

	for (size_t i = 0; i < result.rowCount(); i++) {
		zeitoon::datatypes::DTString temp("tempUsersByGrp");
		temp.setValue(result.fieldValue(i, 0));
		allUsersOfSpecificGroup.push_back(result.fieldValue(i, 0));
	}
	return allUsersOfSpecificGroup;
}
std::vector<zeitoon::datatypes::DTString> UMCore::listPermissions() {
	std::vector<zeitoon::datatypes::DTString> allPermissions;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = querySync("select name from permission");
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to fetch  permission names from database." + std::string(errorInfo->what()), LogLevels::warning);
		EXTDBErrorIO("Unable to fetch  permission names from database", getNameAndType(), errorInfo);
		return allPermissions;
	}
	for (size_t i = 0; i < result.rowCount(); i++) {
		zeitoon::datatypes::DTString temp("tempPermList");
		temp.setValue(result.fieldValue(i, 0));
		allPermissions.push_back(temp);
	}

	return allPermissions;
}
std::vector<zeitoon::datatypes::DTString> UMCore::listUsergroups() {
	std::vector<zeitoon::datatypes::DTString> allGroups;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = querySync("select title from groups");
	} catch (exceptionEx *errorInfo) {
		systemLog.log(getNameAndType(), "Unable to fetch all usergroup titles from database" + std::string(errorInfo->what()), LogLevels::warning);
		EXTDBErrorIO("Unable to fetch all usergroup titles from database", getNameAndType(), errorInfo);
		return allGroups;
	}
	for (size_t i = 0; i < result.rowCount(); i++) {
		zeitoon::datatypes::DTString temp("tempUsrGrpList");
		temp.setValue(result.fieldValue(i, 0));
		allGroups.push_back(temp);
	}

	return allGroups;
}

//-----------------------------Communicatrion functions
zeitoon::datatypes::DTTableString UMCore::querySync(std::string sql) {
	return this->umCHI->sm.database.querySync(sql);
}

int UMCore::executeSync(std::string cmd) {
	return this->umCHI->sm.database.executeSync(cmd);
}
std::string UMCore::singleFieldQuerySync(std::string query) {
	return this->umCHI->sm.database.singleFieldQuerySync(query);
}

//----------------------------------------------------

std::string UMCore::hashingProccess(std::string STRING) {		//pw+(random salt saved to DB) + a digit, would be a better way to go
	CryptoPP::SHA256 sha1;
	std::string hashedVal = "";
	std::string salt = "L+8D fk.3#|";
	STRING += salt;
	CryptoPP::StringSource(STRING, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hashedVal))));
	return hashedVal;
}
int UMCore::checkUserPermissionState(int sessionID, int permissionID) {
//if permission exist. check the state
	const std::map<int, int> &permissioncache = sessionManager.sessionList.at(sessionID).permissionsCache;
	if (permissioncache.find(permissionID) == permissioncache.end()) {
		return 0;
	}
	return permissioncache.find(permissionID)->second;

}
int UMCore::checkUserPermissionParentState(int sessionID, int permissionID) {
	const std::map<int, int> &tempParentCache = sessionManager.permissionParentCache;
	const std::map<int, int> &tempPermissionCache = sessionManager.sessionList.at(sessionID).permissionsCache;
	int tempParentID = 0;

	if (tempParentCache.count(permissionID) != 1) {
		std::cout << "No parent found on cache for " << permissionID << "!\nfetching information from database....\n";
		try {
			tempParentID = getPermissionParent(permissionID);
		} catch (exceptionEx &errorInfo) {
			std::cout << "Cought the Error\n" << errorInfo.what();
			return 202;
		}
		sessionManager.permissionParentCache[permissionID] = tempParentID;
		std::cout << "\nParent of " << permissionID << " is: " << tempParentID << "\n";

	} else {
		tempParentID = tempParentCache.find(permissionID)->second;	///->second;
	}
	int tempState = 0;
	while (tempParentID != -1) {
		std::map<int, int>::const_iterator currentPermission = tempPermissionCache.find(tempParentID);
		if (tempPermissionCache.count(tempParentID) != 1) {
			//if ? tempstate not found on the map,check again for the parent of parent and Continue from start of the loop;
			std::cout << "\nNO STATE FOUND FOR PERMISSION " << tempParentID << "\n";
			tempParentID = tempParentCache.find(tempParentID)->second;	// getPermissionParent(tempParentID);//tempParentCache.find(tempParentID)->second;
			continue;
		}
		switch (currentPermission->second) {
		case (-1):
			std::cout << "\nPERMISSION STATE OF " << currentPermission->first << " IS -1\n";
			return -1;
		case (0):
			std::cout << "\nPERMISSION STATE OF " << currentPermission->first << " IS  0\n";
			break;
		case (1):
			std::cout << "\nPERMISSION STATE OF " << currentPermission->first << " IS  1\n";
			tempState = 1;
			break;
		default:
			break;
		}
		tempParentID = tempParentCache.find(tempParentID)->second;
	}
	return tempState;
}
int UMCore::checkUsergroupPermission(int sessionID, int permissionID) {
	int tempState = 0;
	const std::vector<int> & USERGROUPS = sessionManager.sessionList.at(sessionID).usergruops;
	const std::map<int, usergroupInfo> &USERGROUPCACHE = sessionManager.usergroupCache;
	const std::map<int, int> &PERMISSIONPARENT = sessionManager.permissionParentCache;
	std::cout << "\nNumber of Usergroups:    " << USERGROUPS.size() << "\n";
	for (unsigned int i = 0; i < USERGROUPS.size(); i++) {
		int tempPermissionParent = permissionID;
		int currentusergroup = USERGROUPS[i];
		bool breakHandler = false; //breakHandler is incharge of breaking out of the while-loop when needed
		std::cout << "\nChecking permissions of group no: " << currentusergroup << " \n";
		if (USERGROUPCACHE.count(currentusergroup) != 1) {
			//if usergroup not found!
			//exeption needed!!each usergroup should be registered in userinfoCache!
			std::cout << "\no info on usergroupcache for group: " << USERGROUPS[i];
			continue;	//there sshould be another try to populate usergroupinfo and user it , if not then Continue!!!
		} else {		//if usergroup found!
			//	sessionManager.usergroupCache.find(23)->second
			//while permission parent != -1 { if case(0): or case(1): tempPermissionParent = permissionParent.Find(permissionID)->second
			//continue;
			std::cout << "\nBOOOOOLean" << USERGROUPCACHE.at(currentusergroup).permissions.find(tempPermissionParent)->second << "\n";

			while (tempPermissionParent != -1) {
				if (breakHandler) {
					break;
				}			//breakHandler is incharge of breaking out of the while-loop when needed
				std::cout << "\nChecking for permission " << tempPermissionParent << "\n";
				if (USERGROUPCACHE.at(currentusergroup).permissions.count(tempPermissionParent) != 1) {
					if (PERMISSIONPARENT.count(tempPermissionParent) != 1) {
						breakHandler = true;
						continue;
					}
					tempPermissionParent = PERMISSIONPARENT.find(tempPermissionParent)->second;
					continue;
				}
				switch (USERGROUPCACHE.at(currentusergroup).permissions.find(tempPermissionParent)->second) {
				case (-1):
					std::cout << "\nPermission state of permission " << tempPermissionParent << " of group " << currentusergroup << " is: -1\n";
					return -1;
				case (0):
					std::cout << "\nPermission state of permission " << tempPermissionParent << " of group " << currentusergroup << " is: 0\n";
					if (PERMISSIONPARENT.count(tempPermissionParent) != 1) {
						breakHandler = true;
						break;
					}
					tempPermissionParent = PERMISSIONPARENT.find(tempPermissionParent)->second;
					continue;
					//check parent
				case (1):
					std::cout << "\nPermission state of permission " << tempPermissionParent << " of group " << currentusergroup << " is: 1\n";
					tempState = 1;
					if (PERMISSIONPARENT.count(tempPermissionParent) != 1) {
						breakHandler = true;
						break;
					}
					tempPermissionParent = PERMISSIONPARENT.find(tempPermissionParent)->second;
					continue;
					//check parent

				default:
					if (PERMISSIONPARENT.count(tempPermissionParent) != 1) {
						breakHandler = true;
						break;
					}
					tempPermissionParent = PERMISSIONPARENT.find(tempPermissionParent)->second;
					continue;
				}
			}
		}
	}

	return tempState;
}			////PERMISSION HA BAAYAAD DAR KHODESHOON CHECK BESHAN!!

int UMCore::checkUsergroupParentPermission(int sessionID, int permissionID) {

	const std::vector<int> &tempUserGroups = sessionManager.sessionList.at(sessionID).usergruops;
	const std::map<int, int> &tempUsergroupParentCache = sessionManager.userGroupParentCache;
	const std::map<int, usergroupInfo> &tempUserGroupCache = sessionManager.usergroupCache;
	const std::map<int, int> &permissionParentCache = sessionManager.permissionParentCache;
	int tempState = 0;
	int tempParentID = 0;
	for (unsigned int i = 0; i < tempUserGroups.size(); i++) {			//std::vector<int> usergroups

		if (tempUsergroupParentCache.count(tempUserGroups[i]) != 1) {
			tempParentID = getUsergroupParent(tempUserGroups[i]);
			sessionManager.userGroupParentCache[tempUserGroups[i]] = tempParentID;
		} else {
			tempParentID = tempUsergroupParentCache.find(tempUserGroups[i])->second;
		}

		while (tempParentID != -1) {
			int tempPermissionParent = permissionID;
			while (tempPermissionParent != -1) {
				if (tempUserGroupCache.count(tempParentID) != 1) {
					sessionManager.usergroupCache[tempParentID] = usergroupInfo(tempParentID, this);
				}

				if (tempUserGroupCache.at(tempParentID).permissions.count(tempPermissionParent) != 1) {
					tempPermissionParent = permissionParentCache.find(tempPermissionParent)->second;
					continue;
				}
				switch (tempUserGroupCache.at(tempParentID).permissions.find(tempPermissionParent)->second) {
				case (-1):
					return -1;
				case (0):
					tempPermissionParent = permissionParentCache.find(tempPermissionParent)->second;
					continue;
				case (1):
					tempState = 1;
					tempPermissionParent = permissionParentCache.find(tempPermissionParent)->second;
					continue;
				}

			}
			tempParentID = tempUsergroupParentCache.find(tempParentID)->second;

		}
	}

	return tempState;

}
int UMCore::getPermissionParent(int permissionID) {
//PGresult * result = nullptr;
//zeitoon::datatypes::DTTableString result("");
	try {
		std::cout << "\nTrying to get parent of permission\n";
		//result = querySync("select parentid from permission where id=" + std::to_string(permissionID));
		return std::stoi(querySync("select parentid from permission where id=" + std::to_string(permissionID)).fieldValue(0, 0));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorO("unable to get permissionParent from database" + std::string(errorInfo.what()),
				"PermissionID: " + std::to_string(permissionID) + "@usermanagement::umcore::getpermissionParent]");
	}
//	return std::atoi(PQgetvalue(result, 0, 0));

}
int UMCore::getUsergroupParent(int parentID) {			/////////////////////////COMMENTED OUT THE OLD IMPLEMENTATION
//PGresult * result = nullptr;
	try {

		return std::stoi(querySync("select parentid from groups where id=" + std::to_string(parentID)).fieldValue(0, 0));
		//result = query("select parentid from groups where id=" + std::to_string(parentID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorO("unable to fetch groupParentID for Database " + std::string(errorInfo.what()),
				"[UsergroupID: " + std::to_string(parentID) + "@usermanagement::umcore::getusergroupParent]");
	}

//return atoi(PQgetvalue(result, 0, 0));

}
std::string UMCore::getNameAndType() {
	return "[UMCore]";
}
void UMCore::addUserUsergroup(int userID, int groupID) {
	try {
		executeSync("insert into usergroup values("+std::to_string(userID)+", "+std::to_string(groupID)+")");
	} catch (exceptionEx *errorInfo) {
		EXTDBErrorI("Unable to addUserUsergroup", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersUsergroupAdded, zeitoon::usermanagement::DSUserUsergroup(userID, groupID).toString(true));
}

void UMCore::removeUserUsergroup(int userID, int groupID) {
	try{
		executeSync("delete from usergroup where userid="+std::to_string(userID)+" and groupid="+std::to_string(groupID));
	}catch(exceptionEx *errorInfo){
		EXTDBErrorI("Unable to remove from usergroup table", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersUsergroupRemoved, zeitoon::usermanagement::DSUserUsergroup(userID, groupID).toString(true));
}

void UMCore::addUserPermission(int userID, int permissionID, int state) {
	try{
		executeSync("insert into userpermission values("+ std::to_string(userID) +", "+std::to_string(permissionID) +", "+ std::to_string(state));
	}catch(exceptionEx *errorInfo){
		EXTDBErrorI("Unable to add the permission for the user", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersPermissionAdded,zeitoon::usermanagement::DSUserPermission(userID, permissionID, state).toString(true));
}

void UMCore::removeUserPermission(int userID, int permissionID, int state) {
	try{
		executeSync("delete from userpermission where userid="+ std::to_string(userID) +" and permissionid="+ std::to_string(permissionID));
	}catch(exceptionEx * errorInfo){
		EXTDBErrorI("Unable to remove user's permission",errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersPermissionRemoved,zeitoon::usermanagement::DSUserPermission(userID, permissionID, state).toString(true));
}

}//usermanagement
}//zeitoon
