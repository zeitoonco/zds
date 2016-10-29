/*
 * UMCore.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */
#include <utility/exceptions.hpp>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include <executables/UserManagement/UMCore.hpp>
#include "executables/UserManagement/UmCHI.hpp"
#include "executables/UserManagement/DTStructs.hpp"
#include "exceptions.hpp"

namespace zeitoon {
namespace usermanagement {


UMCore::UMCore(UmCHI *instancePtr) :
		umCHI(instancePtr), /**systemLog("UserManagementLogs"),*/ sessionManager(this) {
}

UMCore::~UMCore() {

}

DSLoginResult UMCore::login(std::string username, std::string password) {
	DSLoginResult tempResult;
	auto currentUser = userLogInfo.find(username);
	if (currentUser != userLogInfo.end()) {    //If user found on userLoginfo list.
		if (currentUser->second.banned) {
			if ((time(NULL) - currentUser->second.Banedtime) > 600) {
				userLogInfo.erase(currentUser);
			} else {
				tempResult.sessionID = -1;
				//sessionID = -1;
				//uID = -1;
				tempResult.userInfo.userID = -1;
				tempResult.loginResult = UMLoginResult::typeString[UMLoginResult::banned];
				//desc = "User is temporary banned(too many false login attempts)";
				tempResult.description = "User is temporary banned(too many false login attempts)";
				lTrace("Login Failed for User: [" + username + "]." + tempResult.description.getValue());
				return tempResult;
			}
		}
	}
	zeitoon::datatypes::DTTableString loginResult("loginResult");
	try {//todo: look for a better solution for case insensitive comparison.
		loginResult = this->querySync(
				"select id, banned, banreason, name from users where lower(username) = lower('" +
				username + "') and password='" + hashingProccess(password) + "'");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch Loging info for user:" + username,
		            errorInfo);
	}
	if (loginResult.rowCount() == 1) {
		tempResult.userInfo.userID = std::stoi(loginResult.fieldValue(0, 0));
		tempResult.userInfo.banned = (loginResult.fieldValue(0, 1) == "t" ||
		                              loginResult.fieldValue(0, 1) == "1" ||
		                              loginResult.fieldValue(0, 1) == "y") ? true : false;
		tempResult.userInfo.banReason = loginResult.fieldValue(0, 2, "None");
		tempResult.userInfo.name = loginResult.fieldValue(0, 3);
		tempResult.userInfo.username = username;
		if (tempResult.userInfo.banned) {            //IF USER IS BANNED
			tempResult.sessionID = -1;
			tempResult.userInfo.userID = -1;
			tempResult.description = loginResult.fieldValue(0, 2);
			tempResult.loginResult = UMLoginResult::typeString[UMLoginResult::banned];
			return tempResult;
		} else {    //SUCCESSFUL AUTHENTICATION
			try {
				tempResult.loginResult = UMLoginResult::typeString[UMLoginResult::ok];
				/*auto dds= this->sessionManager.newSession(tempResult.userInfo.userID.getValue(),
														  tempResult.userInfo.username.getValue());*/
				tempResult.sessionID = this->sessionManager.newSession(tempResult.userInfo.userID.getValue(),
				                                                       tempResult.userInfo.username.getValue());
				if (!this->checkPermissionByName(tempResult.sessionID, "userman.login")) {
					sessionManager.removeSession(tempResult.sessionID);
					tempResult.sessionID = -1;
					tempResult.description = "Access Denied";
					tempResult.loginResult = UMLoginResult::typeString[UMLoginResult::accessDenied];
					return tempResult;
				}
				tempResult.userInfo.isOnline = true;
				umCHI->sm.communication.runEvent(eventInfo::loggedIn(),
				                                 zeitoon::usermanagement::DSUserInfo(
						                                 tempResult.userInfo.userID,
						                                 username,
						                                 tempResult.userInfo.name,
						                                 tempResult.userInfo.banned,
						                                 "",
						                                 this->isOnline(tempResult.userInfo.userID)).toString(true));
			} catch (exceptionEx &errorInfo) {
				EXTDBErrorI("Login: unable to register the session for " + username, errorInfo);
			}
			lNote(username + " [" + std::to_string(tempResult.sessionID.getValue()) + "] " + "Loged in");
			tempResult.permissions = this->userPermissionDetailedList(tempResult.userInfo.userID.getValue());
			return tempResult;
		}
	} else if (loginResult.rowCount() == 0) {    //IF THERE IS NO MATCH FOR USER/PW/NAME
		if (currentUser != userLogInfo.end()) {
			//	auto currentUser = userLogInfo.find(username);
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
				lWarnig("User [" + username + "] is banned now(at [" +
				        std::to_string(currentUser->second.Banedtime)
				        + "]), because of to many wrong login inputs.");
			}

		} else {
			userLogInfoStruct currentUserLogStruct;
			currentUserLogStruct.logAttemptTimes.push_back(time(NULL));
			userLogInfo[username] = currentUserLogStruct;
		}
		tempResult.sessionID = -1;
		tempResult.userInfo.userID = -1;
		tempResult.description = "Invalid Username or Password.";
		tempResult.loginResult = UMLoginResult::typeString[UMLoginResult::invalidUserPass];
		lNote("Invalid username or password for USER: [" + username + "]. ");
		return tempResult;
	} else {    //IF ANY THING OTHER THAN PREVIOUSLY DEFINED CONDITIONS HAPPEN.
		tempResult.sessionID = -1;
		tempResult.userInfo.userID = -1;
		tempResult.description = "Unknown Error!";
		tempResult.loginResult = UMLoginResult::typeString[UMLoginResult::unknownError];
		lDebug("Unknown Error on login attempt for USER: [" + username + "]. ");
		return tempResult;
	}

}


void UMCore::logout(int sessionID) {
	if (sessionID == -1 or sessionManager.getUserIDBySession(sessionID) == -1)
		return;
	//EXTloginFail("LOGOUT FAILED");
	auto iter = sessionManager.sessionList.find(sessionID);
	std::string username = iter->second.username;//sessionManager.sessionList.find(sessionID)->second.username;
	int userID = iter->second.userID;
	sessionManager.removeSession(sessionID);
	//this->sessionManager.sessionList.at(sessionID).userID;//:: initialize upon removal of session? by inf
	umCHI->sm.communication.runEvent(
			eventInfo::loggedOut(), zeitoon::usermanagement::DSUserInfo(userID, username, "", false, "",
			                                                            this->isOnline(userID)).toString(true));
	//##Event Fired
	lNote(username + " [" + std::to_string(sessionID) + "] " + "Logged out");
}

bool UMCore::checkPermission(int sessionID, int permissionID) {
	try {
		switch (this->checkUserPermissionState(sessionID, permissionID)) {
			case (-1):
				return false;
			case (0):
				return this->checkUsergroupPermission(sessionID, permissionID) == 1;
			case (1):
				return true;
			default:
				EXTunknownException("checkPermission Failed. permission state");
		}
	} catch (zeitoon::utility::exceptionEx &err) {
		lFatal("UM CK PERM: " + std::string(err.what()));
		EXTcheckPermissionFailI("Check Permission Failed", err);
	} catch (exception &err) {
		EXTcheckPermissionFail(string("Unknown Error  ") + err.what());
	}
}

bool UMCore::checkPermissionByName(int sessionID, string permissionName) {
	auto permissionID = this->sessionManager.permissionNamesCache.find(permissionName);
	if (permissionID != this->sessionManager.permissionNamesCache.end()) {
		try {
			return this->checkPermission(sessionID, permissionID->second->permissiosnID);
		} catch (zeitoon::utility::exceptionEx err) {
			EXTcheckPermissionFailI("checkPermissionByName FAILED", err);
		}
	}
	EXTcheckPermissionFail("checkPermissionByName FAILED. No PermissionID found for given PermissionName");
}

void UMCore::loadCaches() {
	try {
		this->sessionManager.permissionCacheLoader();
		this->sessionManager.userGroupCacheLoader();
		lNote("Permission cache loaded. size: " +
		      std::to_string(this->sessionManager.permissionNamesCache.size()));
		lNote("Usergroup cache loaded. size: " + std::to_string(this->sessionManager.usergroupCache.size()));
	} catch (zeitoon::utility::exceptionEx err) {
		EXTunknownException("Unable to load cache" + std::string(err.what()));
	}
}

int UMCore::getUserIDUsingSessionID(int sessionID) {
	return sessionManager.getUserIDBySession(sessionID);
}


int UMCore::addUser(std::string username, std::string password,
                    std::string name) { //regs a new user in database
///----Reg a new user in Database:
	if (username.size() < 3 || password.size() == 0 || name.size() == 0)
		EXTinvalidParameter("Invalid parameters provided.");
	if (username.find(" ") != username.npos)//todo @ ajl, shall we have any constraint on userName or password?
		EXTinvalidParameter("UserName cannot contain space character");
	std::string command = "INSERT INTO users (id, username, password, name, banned, banreason, avatar) VALUES"
			                      "(default, '" + username + "', '" + hashingProccess(password) + "', '" +
	                      name +
	                      "', default , default, default ) RETURNING id";
	int userID = 0;
	try {//todo:Error if user exist??!
		userID = std::stoi(this->singleFieldQuerySync(command));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to add user [" + username + "] to database", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::userAdded(),
	                                 zeitoon::usermanagement::DSUserInfo(userID, username, name, false, "",
	                                                                     this->isOnline(userID)).toString(
			                                 true));
	//##Event Fired
	lNote("User added. [user: " + username + " id:" + std::to_string(userID) + "]");
	return userID;
}

void UMCore::removeUser(int userID) {
	std::string userName = "";
	auto isessionID = sessionManager.getSessionIDbyUserID(userID);
	//if (isessionID != -1)
	logout(isessionID);

	try {//remove user from users in database
		userName = this->singleFieldQuerySync(
				"DELETE FROM users WHERE id = " + std::to_string(userID) + " RETURNING username");

	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("removeUser failed for userID[" + std::to_string(userID), errorInfo);
	}
	/*if (userName.size() < 1) {
		*//**this condition verfies size of username
         * since the username with lest than 3 characters is prohibited in add user function
         * returning any username.size() less than 3 is invalid thus should be considered as an invalid userID.//by inf
         **//*
        EXTDBError("USER REMOVE FAILED. User" + std::to_string(userID) + "was not found in database");
    }*/
	try {        //remove all permissions of user from DB
		executeSync("DELETE FROM userpermission WHERE userid=" + std::to_string(userID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI(
				"Unable to remove permissions for [user: " + userName + " id:" + std::to_string(userID) +
				"]from DB",
				errorInfo);
	}

	umCHI->sm.communication.runEvent
			(eventInfo::userRemoved(), zeitoon::usermanagement::DSUserInfo
					(userID, userName, "", false, "",
					 this->isOnline(userID)).toString(true));//##Event Fired

	lNote("User removed. [ID: " + std::to_string(userID) + " Name: " + userName + "]");
}


void UMCore::modifyUser(int userID, std::string username, std::string password, std::string name) {
	if (username.size() < 3 || password.size() == 0 || name.size() == 0)
		EXTinvalidParameter("Invalid parameters provided.");
	try {
		//todo: compare username in database and function parameter after update to check
		if (this->executeSync(
				"update users set username='" + username + "', password='" + hashingProccess(password) +
				"', name='" +
				name + "' where id="
				+ std::to_string(userID)) < 1) {
			EXTDBError("No Error but No update. Maybe a Wrong userID?");
		}
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to modify user[" + std::to_string(userID) + "] in database.", errorInfo);
	}
	try {
		auto tempSession = sessionManager.sessionList.find(sessionManager.getSessionIDbyUserID(userID));
		if (tempSession != sessionManager.sessionList.end())
			tempSession->second.username = username;//updates session's username if user has an active session going on
		lNote("User " + std::to_string(userID) + " modified. Username: " + username);
	} catch (...) {
		lNote("Session update failed. User " + username + " is offline.");
	}

	umCHI->sm.communication.runEvent(eventInfo::userModified(), zeitoon::usermanagement::DSUserInfo(
			userID, username, name, false, "", this->isOnline(userID)).toString(true));

	//##Event Fired


}

UMUserInfo UMCore::getUserInfo(int ID) {
	try {
		return UMUserInfo(ID, this);
	} catch (exceptionEx err) {
		EXTinvalidNameI("User's info not found on DB", err);
	}
}

int UMCore::registerPermission(std::string name, std::string title, std::string desc, int parent,
                               const std::string &from) {
	std::string permissionID = "";
	try {
		permissionID = singleFieldQuerySync(
				"insert into permission( id, parentid, name, title, description) values(default, " +
				(parent == -1 ? string("NULL") : std::to_string(parent)) + ", '" + name + "', '" + title
				+ "', '" + desc + "')  RETURNING id");
		if (permissionID.size() > 0)
			executeSync("INSERT INTO servicepermission (service, permissionid) VALUES ('" + from + "'," + permissionID +
			            ")");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to register permission[" + name + "(" + title + ")" + desc + "] in database",
		            errorInfo);
	}
	umCHI->sm.communication.runEvent
			(eventInfo::permissionAdded(), zeitoon::usermanagement::DSUpdatePermission(
					std::stoi(permissionID), name, title, desc, parent).toString(true));
	//##Event Fired
	lNote("Permission[" + name + ", ID:" + permissionID + "] registered.");
	this->sessionManager.permissionCacheUpdate(std::stoi(permissionID));//add to permissionCache
	return std::stoi(permissionID);
}

void UMCore::updatePermission(int permissionID, std::string name, std::string title, std::string desc,
                              int parentID) {
	try {
		int executeResult = executeSync(
				"update permission set name='" + name + "', title='" + title + "', description='" + desc +
				"', parentid=" + (parentID == -1 ? string("NULL") : std::to_string(parentID))
				+ " where id =" + std::to_string(permissionID));
		if (executeResult < 1)
			EXTDBError("No update happend, No Error Occured");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to update permission[ID:" + std::to_string(permissionID) + "] in database",
		            errorInfo);
	}//todo:first remove permission info of the cache---??
	umCHI->sm.communication.runEvent
			(eventInfo::permissionModified(), zeitoon::usermanagement::DSUpdatePermission(
					permissionID, name, title, desc, parentID).toString(true));
	//##Event Fired
	lNote("Permission[ID:" + std::to_string(permissionID) + " ] updated.");
	this->sessionManager.permissionCacheUpdate(permissionID);//update permissionCache
}

void UMCore::removePermission(int permissionID) {
	std::string permIDStr = std::to_string(permissionID);
	try {        //remove permission from users permission table in database
		executeSync("BEGIN;"
				            " delete from userpermission where permissionid=" + permIDStr + ";"
				            " delete from grouppermission where permissionid=" + permIDStr + ";"
				            " delete from permission where id=" + permIDStr + ";"
				            " END;");
		//todo@ ajl:  how to check if succeeded?
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove permission[ID: " + std::to_string(permissionID) + "] from database.",
		            errorInfo);
	}
	sessionManager.permissionCacheUpdate(permissionID);
	umCHI->sm.communication.runEvent(eventInfo::permissionRemoved(),
	                                 zeitoon::usermanagement::DSUpdatePermission(permissionID, "", "", "",
	                                                                             -2).toString(
			                                 true));
	//##Event Fired
	lNote("Permission[ID:" + std::to_string(permissionID) + " ] removed");
	sessionManager.permissionCacheUpdate(permissionID);//update to remove from cache
}

int UMCore::registerUsergroup(std::string title, int parentID, std::string desc) {
	int usergroupID;
	try {
		usergroupID = std::stoi(singleFieldQuerySync(
				"insert into groups( id, title, parentid, description) values(default, '" + title + "', " +
				(parentID == -1 ? string("NULL") : std::to_string(parentID)) + ", '" + desc +
				"') RETURNING id"));

		if (usergroupID < 1)
			EXTDBError("Insert failed. no error from database");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to register usergroup[" + title + "] in database.", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usergroupAdded(),
	                                 zeitoon::usermanagement::DSUpdateUsrGrp(usergroupID, title, parentID,
	                                                                         desc).toString(true));
	//##Event Fired
	lNote("Usergroup[ID: " + std::to_string(usergroupID) + "] registered.");
	sessionManager.updateUsergroupCache(usergroupID);//updateUserGroupCache
	return usergroupID;
}

void UMCore::updateUsergroup(int usergroupID, std::string title, int parentID, std::string desc) {
	int a;
	try {
		a = executeSync(
				"update groups set title='" + title + "', description='" + desc + "', parentid=" +
				(parentID == -1 ? string("NULL") : std::to_string(parentID)) + " where id="
				+ std::to_string(usergroupID));
		if (a < 1)
			EXTDBError("DB ERROR, no update but no error");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to update usergroup[ID: " + std::to_string(usergroupID) + "].",
		            errorInfo);
	}
	umCHI->sm.communication.runEvent(
			eventInfo::usergroupModified(), zeitoon::usermanagement::DSUpdateUsrGrp(
					usergroupID, title, parentID, desc).toString(true));
	//##Event Fired
	lNote("Usergroup[ID:" + std::to_string(usergroupID) + "] modified.");
	try {
		sessionManager.updateUsergroupCache(usergroupID);
	} catch (zeitoon::utility::exceptionEx err) {
		EXTDBErrorI("Unable to update usergroup cache after group updated in DB", err);
	}


}

void UMCore::removeUsergroup(int usergroupID) {

	std::string GrID = std::to_string(usergroupID);
	try {
		/*executeSync("BEGIN;"
							"delete from groups where id= " + GrID + "; delete from usergroup where groupid=" + GrID +
					"; end;");*/
		if (not executeSync("delete from groups where id= " + GrID))
			EXTDBError("REMOVE Group Failed" + GrID + " no error from database.");

	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove usergroup[" + std::to_string(usergroupID) + "] from database", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usergroupRemoved(),
	                                 zeitoon::usermanagement::DSUpdateUsrGrp(usergroupID, "", -2, "").toString(
			                                 true));
	//##Event Fired
	lNote("Usergroup[ID: " + std::to_string(usergroupID) + "] removed.");

	sessionManager.updateUsergroupCache(usergroupID);//todo:check if this works properly
}

DSUserList UMCore::listUsers(bool listAllUsers, DSUserIDs IDs) {
	DSUserList allUsers;
	std::string strPatch = "";
	if (not listAllUsers) {
		strPatch = "WHERE id IN (";
		for (int i = 0; i < IDs.idlist.length(); i++)
			strPatch += std::to_string(IDs.idlist[i]->getValue()) + (i == (IDs.idlist.length() - 1) ? ")" : ",");
	}
	zeitoon::datatypes::DTTableString result("");
	try {
		result = this->querySync("select id,username,name,banned,banreason from users" + strPatch + " order by id");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch all usernames from database", errorInfo);
	}

	for (size_t i = 0; i < result.rowCount(); i++) {
		DTBoolean b("");
		b.fromString(result.fieldValue(i, 3));
		int userID = stoi(result.fieldValue(i, 0));
		allUsers.usersList.add(
				new DSUserInfo(userID, result.fieldValue(i, 1), result.fieldValue(i, 2),
				               b.getValue(), result.fieldValue(i, 4), this->isOnline(userID)), true);
	}

	return allUsers;
}

DSUserList UMCore::listUsersByGroup(
		int groupID) {        //recieves group id and returns all members in that group
	DSUserList allUsersOfSpecificGroup;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = querySync(
				"select id,username,name,banned,banreason from users inner join usergroup on usergroup.userid=users.id where groupid=" +
				std::to_string(groupID) + " order by users.id");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch  users of group[" + std::to_string(groupID) + "] from database",
		            errorInfo);
	}

	for (size_t i = 0; i < result.rowCount(); i++) {
		DTBoolean b("");
		b.fromString(result.fieldValue(i, 3));
		allUsersOfSpecificGroup.usersList.add(
				new DSUserInfo(stoi(result.fieldValue(i, 0)), result.fieldValue(i, 1), result.fieldValue(i, 2),
				               b.getValue(), result.fieldValue(i, 4),
				               this->isOnline(stoi(result.fieldValue(i, 0)))), true);
	}
	return allUsersOfSpecificGroup;
}

DSPermissionsList UMCore::listPermissions(std::string serviceName) {
	/*Receive service name as a string only from core
	 * node listPermission still has no data*/
	DSPermissionsList allPermissions;
	zeitoon::datatypes::DTTableString result("");
	std::string addQuery = "";
	if (serviceName.size() > 0) {
		addQuery = "WHERE id IN (SELECT permissionid FROM servicepermission WHERE service= '" + serviceName + "')";
	}
	try {
		result = querySync("select id,parentid,name,title,description from permission " + addQuery + " order by id");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch  permission names from database", errorInfo);

	}
	for (size_t i = 0; i < result.rowCount(); i++) {
		allPermissions.permissionsList.add(new DSUpdatePermission(stoi(result.fieldValue(i, 0)),
		                                                          result.fieldValue(i, 2),
		                                                          result.fieldValue(i, 3),
		                                                          result.fieldValue(i, 4),
		                                                          result.fieldIsNull(i, 1) ? -1 : stoi(
				                                                          result.fieldValue(i, 1))), true);
	}
	return allPermissions;
}

DSUserGroupsList UMCore::listGroups(int userID) {
	DSUserGroupsList allGroups;
	zeitoon::datatypes::DTTableString result("");
	/*todo @ Ajl: CANNOT make the list using the existing cache.
	 *  Class usergroupInfo does not have "title" and "desc" field
	 **/

	try {
		result = querySync(
				"select id,title,parentid,description from groups where id=(select groupid from usergroup where userid=" +
				std::to_string(userID) + ")");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch usergroups data from database", errorInfo);
	}
	for (size_t i = 0; i < result.rowCount(); i++) {
		allGroups.userGrpsList.add(new DSUpdateUsrGrp(stoi(result.fieldValue(i, 0)),
		                                              result.fieldValue(i, 1),
		                                              result.fieldIsNull(i, 2) ? -1 : stoi(
				                                              result.fieldValue(i, 2)),
		                                              result.fieldValue(i, 3)), true);
	}

	return allGroups;
}

DSUserGroupsList UMCore::listUsergroups() {
	DSUserGroupsList allGroups;
	zeitoon::datatypes::DTTableString result("");
	/*todo @ Ajl: CANNOT make the list using the existing cache.
	 *  Class usergroupInfo does not have "title" and "desc" field
	 **/

	try {
		result = querySync("select id,title,parentid,description from groups order by id");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch usergroups data from database", errorInfo);
	}
	for (size_t i = 0; i < result.rowCount(); i++) {
		allGroups.userGrpsList.add(new DSUpdateUsrGrp(stoi(result.fieldValue(i, 0)),
		                                              result.fieldValue(i, 1),
		                                              result.fieldIsNull(i, 2) ? -1 : stoi(
				                                              result.fieldValue(i, 2)),
		                                              result.fieldValue(i, 3)), true);
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

void UMCore::addContact(int userID, int contactID, std::string note) {
	int temp = 0;
	try {
		temp = executeSync("INSERT INTO contacts (userid, contactid, note) VALUES(" + std::to_string(userID) + "," +
		                   std::to_string(contactID) + ",'" + note + "')");
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTaddContactFailed("Error: " + std::string(err.what()));
	}
	if (!temp)
		EXTaddContactFailed("No error but no affected rows in database..needs debug!");
}


void UMCore::modifyContact(int userID, int contactID, std::string note) {
	int temp = 0;
	try {
		temp = executeSync(
				"UPDATE contacts SET note='" + note + "' Where userid=" +
				std::to_string(userID) + " AND contactid=" + std::to_string(contactID));
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTmodifyContactFailed("Error: " + std::string(err.what()));
	}

	if (!temp)
		EXTmodifyContactFailed("No error but no affected rows in database..needs debug!");
}


void UMCore::removeContact(int userID, int contactID) {
	int temp = 0;
	try {
		temp = executeSync("DELETE FROM contacts WHERE userid=" + std::to_string(userID) + " and contactid=" +
		                   std::to_string(contactID));
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTremoveContactFailed("Error: " + std::string(err.what()));
	}
	if (!temp)
		EXTremoveContactFailed("No error but no affected rows in database..needs debug!");
}

DSUserContactList UMCore::listContacts(int userID) {
	try {
		auto tempResult = querySync("SELECT contactid, note FROM contacts WHERE userid=" + std::to_string(userID));
		DTSet<DSUserContactInfo> tempList = {"contactList"};
		DSUserContactList retList;
		auto length = tempResult.rowCount();
		for (size_t iter = 0; iter < length; iter++) {
			retList.contactList.add(
					new DSUserContactInfo(tempResult.fieldValueInt(iter, 0, -1), tempResult.fieldValue(iter, 1, "")),
					true);
		}
		retList.userID = userID;
		return retList;
	} catch (exceptionEx &err) {
		EXTlistContactsFailed(std::string(err.what()));
	}
}


//----------------------------------------------------

std::string UMCore::hashingProccess(
		std::string STRING) {        //pw+(random salt saved to DB)
	CryptoPP::SHA256 sha1;
	std::string hashedVal = "";
	std::string salt = "L+8D fk.3#|";//todo review salt
	STRING += salt;
	CryptoPP::StringSource(STRING, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(
			new CryptoPP::StringSink(hashedVal))));
	return hashedVal;
}

int UMCore::checkUserPermissionState(int &sessionID, int &permissionID) {
	auto tempSession = this->sessionManager.sessionList.find(sessionID);//Find UMsession
	if (tempSession == this->sessionManager.sessionList.end())//exception if its not a valid sessionID
		EXTinvalidName("checkUserPermissionState FAILED. Invalid sessionID.");
	auto tempPermission = tempSession->second.permissionsCache.find(
			permissionID);//Find sessions permission state
	if (tempPermission == tempSession->second.permissionsCache.end())
		return this->checkUserPermissionParentState(sessionID, permissionID);
	if (tempPermission != tempSession->second.permissionsCache.end()) {//if permission is found
		if (tempPermission->second == -1)
			return -1;
		if (tempPermission->second == 0)
			return checkUserPermissionParentState(sessionID, permissionID);
		if (tempPermission->second == 1)
			return (this->checkUserPermissionParentState(sessionID, permissionID) == -1 ? -1 : 1);
	}
	EXTunknownException("checkUserPermissionState Failed. unkown condition. needs debug");


}

int UMCore::checkUserPermissionParentState(int &sessionID, int &permissionID) {//todo: to be tested
	int tempParID = this->getPermissionParent(permissionID);
	if (tempParID == -1) {//if no parent for given permission
		return 0;
	}
	return this->checkUserPermissionState(sessionID, tempParID);
}


int UMCore::checkUsergroupPermission(int &sessionID, int &permissionID) {
	auto tempSession = this->sessionManager.sessionList.find(sessionID);
	if (tempSession == this->sessionManager.sessionList.end())
		EXTinvalidName("checkUsergroupPermission FAILED. Invalid sessionID.");
	for (std::vector<int>::iterator iter = tempSession->second.usergroups.begin();
	     iter != tempSession->second.usergroups.end(); iter++) {
		auto tempGroupInfo = this->sessionManager.usergroupCache.find(*iter);
		if (tempGroupInfo == this->sessionManager.usergroupCache.end())
			EXTunknownException("UserGroup" + std::to_string(*iter) + " Not found on usergroupCache");
		auto tempGroupPermission = tempGroupInfo->second.permissions.find(permissionID);
		if (tempGroupPermission == tempGroupInfo->second.permissions.end())
			continue;
		if (tempGroupPermission != tempGroupInfo->second.permissions.end()) {
			if (tempGroupPermission->second == -1)
				return -1;
			if (tempGroupPermission->second == 0)
				return checkUsergroupParentPermission(*iter, sessionID, permissionID);
			if (tempGroupPermission->second == 1) {
				int tempGrpParentState = checkUsergroupParentPermission(*iter, sessionID, permissionID);
				return (tempGrpParentState == -1 ? -1 : 1);
			}
		}
	}
	//EXTunknownException("checkUserPermissionState Failed. unkown condition. needs debug");//todo:@navidi review
	return 0;
}

int UMCore::checkUsergroupParentPermission(int &userGroupID, int &sessionID, int &permissionID) {
	int tempParID = this->getUsergroupParent(userGroupID);
	if (tempParID == -1)
		return 0;
	return this->checkUsergroupPermission(sessionID, permissionID);
}

int UMCore::getPermissionParent(int &permissionID) {
	auto temp = this->sessionManager.permissionCache.find(permissionID);
	if (temp != this->sessionManager.permissionCache.end()) {
		return temp->second->parentID;
	} else {/*///todo: make sure cache is update or make a query
		try {
			int res = this->querySync(
					"SELECT parentid  FROM permission WHERE id = " + std::to_string(permissionID)).fieldValueInt(0, 0,
			                                                                                                     -1);
			return res;
		} catch (zeitoon::utility::exceptionEx err) {
			EXTDBErrorI("Unanble to fetch permission parent from database", err);
		}*/
		return -1;
	}
}

int UMCore::getUsergroupParent(int groupID) {
	auto temp = this->sessionManager.usergroupCache.find(groupID);
	if (temp != this->sessionManager.usergroupCache.end()) {
		return temp->second.parentID;
	} else {
		/*try {
			int res = this->querySync("select parentid from groups where id=" + std::to_string(groupID)).fieldValueInt(
					0, 0, -1);
			return res;
		} catch (zeitoon::utility::exceptionEx err) {
			EXTDBErrorI("Unanble to fetch usergroup parent from database", err);
		}*////todo: make sure cache is update or make a query//cache is updated after each update in db
		return -1;
	}


}

void UMCore::addUserUsergroup(int userID, int groupID) {
	try {
		int a = executeSync(
				"insert into usergroup values(" + std::to_string(userID) + ", " + std::to_string(groupID) +
				")");
		if (a != 1)
			EXTDBError("INSER FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to addUserUsergroup", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersUsergroupAdded(),
	                                 zeitoon::usermanagement::DSUserUsergroup(userID, groupID).toString(true));
	lNote(eventInfo::usersUsergroupAdded() + "User: " + std::to_string(userID) + "Group: " +
	      std::to_string(groupID));
	auto temp = sessionManager.sessionList.find(this->sessionManager.getSessionIDbyUserID(userID));
	if (temp != sessionManager.sessionList.end()) {
		temp->second.usergroups.push_back(groupID);
	}
}

void UMCore::removeUserUsergroup(int userID, int groupID) {
	try {
		int a = executeSync("delete from usergroup where userid=" + std::to_string(userID) + " and groupid=" +
		                    std::to_string(groupID));
		if (a != 1)
			EXTDBError("DELETE FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove from usergroup table", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersUsergroupRemoved(),
	                                 zeitoon::usermanagement::DSUserUsergroup(userID, groupID).toString(true));
	lNote(eventInfo::usersUsergroupRemoved() + "User: " + std::to_string(userID) + "Group: " +
	      std::to_string(groupID));
	auto temp = sessionManager.sessionList.find(this->sessionManager.getSessionIDbyUserID(userID));
	if (temp != sessionManager.sessionList.end()) {
		for (auto i = temp->second.usergroups.begin(); i != temp->second.usergroups.end(); i++) {
			if (*i == groupID) {
				temp->second.usergroups.erase(i);
			}
		}
	}
}

void UMCore::addUserPermission(int userID, int permissionID, int state) {
	int a = 0;
	try {
		/*"INSERT INTO userpermission (userid, permissionid, state) SELECT "++", WHERE NOT EXISTS( SELECT * FROM userpermission WHERE state = "+ std::to_string(state)+")"*/

		a = executeSync(
				"INSERT INTO userpermission(userid, permissionid, state) VALUES(" + std::to_string(userID) + ", " +
				std::to_string(permissionID) + ", " + std::to_string(state) +
				") ON CONFLICT(userid,permissionid) DO UPDATE SET state=" + std::to_string(state));
		if (a < 1) {
			EXTDBError("No permission added for user:" + std::to_string(userID));
		}
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to add the permission for the user", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersPermissionAdded(),
	                                 zeitoon::usermanagement::DSUserPermission(userID, permissionID,
	                                                                           state).toString(
			                                 true));
	lNote(eventInfo::usersPermissionAdded() + "User: " + std::to_string(userID) + "PermissionID: " +
	      std::to_string(permissionID) + "State: " +
	      std::to_string(state));//todo @ ajl to chek if this log is ok
	auto iter = sessionManager.sessionList.find(this->sessionManager.getSessionIDbyUserID(userID));
	if (iter != sessionManager.sessionList.end())
		iter->second.permissionsCache[permissionID] = state;
}

void UMCore::removeUserPermission(int userID, int permissionID) {
	try {
		int a = executeSync(
				"delete from userpermission where userid=" + std::to_string(userID) + " and permissionid=" +
				std::to_string(permissionID));
		if (a != 1)
			EXTDBError("DELETE FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove user's permission", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersPermissionRemoved(),
	                                 zeitoon::usermanagement::DSUserPermission(userID, permissionID,
	                                                                           0).toString(
			                                 true));
	lNote(eventInfo::usersPermissionRemoved() + "User: " + std::to_string(userID) + "PermissionID: " +
	      std::to_string(permissionID));
	auto iter = sessionManager.sessionList.find(this->sessionManager.getSessionIDbyUserID(userID));
	if (iter != sessionManager.sessionList.end())
		iter->second.permissionsCache.erase(permissionID);
}

DSUserPermissionList UMCore::listUserPermissions(int userID) {
	DSUserPermissionList list;
	zeitoon::datatypes::DTTableString result("");
	if (this->sessionManager.getSessionIDbyUserID(userID) > -1) {
		//Todo @ ajl : check -> now tries to fetch the list from user's active session if available
		auto aa = this->sessionManager.sessionList.at(
				this->sessionManager.getSessionIDbyUserID(userID)).permissionsCache;
		for (std::map<int, int>::iterator iter = aa.begin(); iter != aa.end(); iter++) {
			list.permissionsList.add(new DSPermissionState(iter->first, iter->second), true);
		}
	} else {
		try {
			result = querySync(
					"select permissionid,state from userpermission where userid=" + to_string(userID));
		} catch (exceptionEx &errorInfo) {
			EXTDBErrorI("Unable to fetch  permission names from database", errorInfo);
		}
		try {
			for (size_t i = 0; i < result.rowCount(); i++) {
				list.permissionsList.add(new DSPermissionState(stoi(result.fieldValue(i, 0)),
				                                               stoi(result.fieldValue(i, 1))), true);
			}
		} catch (zeitoon::utility::exceptionEx &err) {
			EXTDBErrorI("Unable to make a list of permissions", err);
		}
	}
	return list;
}

void UMCore::addUsergroupPermission(int usergroupID, int permissionID, int state) {
	try {
		int a = executeSync(
				"insert into grouppermission(permissionid,groupid,state) values(" +
				std::to_string(permissionID) + ", " + std::to_string(usergroupID) +
				", " + std::to_string(state) + ") ON CONFLICT(permissionid, groupid) DO UPDATE SET state=" +
				std::to_string(state));
		if (a != 1)
			EXTDBError("INSERT FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to add the permission for the user", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usergroupPermissionAdded(),
	                                 zeitoon::usermanagement::DSUserPermission(usergroupID, permissionID,
	                                                                           state).toString(
			                                 true));
	lNote(eventInfo::usergroupPermissionAdded() + "UsergroupID: " + std::to_string(usergroupID) +
	      "PermissionID: " +
	      std::to_string(permissionID) + " State: " + std::to_string(state));
	sessionManager.updateUsergroupCache(usergroupID);//see if its modified in the cache
}

void UMCore::removeUsergroupPermission(int usergroupID, int permissionID, int state) {
	try {
		int a = executeSync(
				"delete from grouppermission where groupid=" + std::to_string(usergroupID) +
				" and permissionid=" +
				std::to_string(permissionID));
		if (a != 1)
			EXTDBError("DELETE FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove user's permission", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usergroupPermissionRemoved(),
	                                 zeitoon::usermanagement::DSUserPermission(usergroupID, permissionID,
	                                                                           state).toString(
			                                 true));
	lNote(eventInfo::usergroupPermissionRemoved() + "UsergroupID: " + std::to_string(usergroupID) +
	      "PermissionID: " +
	      std::to_string(permissionID));
	sessionManager.updateUsergroupCache(usergroupID);//see if it removes from cache

	//todo:In DSUserPermission, first field is called UserID- ask if the actual field name is important
}

DSUsergroupPermissionList UMCore::listUsergroupPermissions(int usergroupID) {
	DSUsergroupPermissionList list;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = querySync(
				"select permissionid, state, parentid, name, description from grouppermission "
						"LEFT JOIN permission ON grouppermission.permissionid = permission.id where grouppermission.groupid=" + to_string(usergroupID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch  permission names from database", errorInfo);
	}
	try {

		for (size_t i = 0; i < result.rowCount(); i++) {
			list.permissionsList.add(
					new DSPermissionInfo(stoi(result.fieldValue(i, 0)), stoi(result.fieldValue(i, 1)),
					                      stoi(result.fieldValue(i, 2)), result.fieldValue(i, 3),result.fieldValue(i, 4)),
					true);
		}
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTDBErrorI("Unable to make a list of userGroupPermissions", err);

	}
	return list;
}


void UMCore::removeServicePermissions(std::string serviceName) {
	std::string quer =
			"DELETE FROM permission WHERE id IN (SELECT permissionid FROM servicepermission WHERE service= '" +
			serviceName + "') RETURNING id";
	auto res = querySync(quer);
	//todo: incomplete & trial
	for (size_t iter = 0; iter < res.rowCount(); iter++) {
		sessionManager.permissionCacheUpdate(res.fieldValueInt(iter, 0));
	}
}


std::string UMCore::getNameAndType() {
	return "UMCore";
}

DTSet<DSBrfPermission> UMCore::userPermissionDetailedList(int userID) {
	DTSet<DSBrfPermission> temp = {"permissions"};


	auto sID = this->sessionManager.getSessionIDbyUserID(userID);
	if (sID < 0)
		EXTunknownException("LIST USER PERMISSIONS FAILED. User is offline");
	std::map<int, DSUpdatePermission *>::const_iterator perCache;
	for (perCache = this->sessionManager.permissionCache.begin();
	     perCache != this->sessionManager.permissionCache.end(); perCache++) {
		// if (this->checkPermission(sID, perCache->first)) {
		temp.add(new DSBrfPermission(perCache->second->permissiosnID.getValue(),
		                             perCache->second->name.getValue(),
		                             perCache->second->parentID.getValue(),
		                             this->checkPermission(sID, perCache->second->permissiosnID.getValue())),
		         true);
	}

	return temp;
}


bool UMCore::isOnline(int userID) {//if sessionID = -1 ==> user is offline

	try {
		return (this->sessionManager.getSessionIDbyUserID(userID) != -1);
	} catch (zeitoon::utility::exceptionEx err) {
	}
	return false;
}

DSUserAvatar UMCore::getUserAvatar(int userID) {

	try {
		std::string tempR = this->singleFieldQuerySync("SELECT avatar FROM users WHERE id=" + std::to_string(userID));
		/*string encoded;
		auto strSink = new CryptoPP::StringSink(encoded);
		auto Base64Enc = new CryptoPP::Base64Encoder(strSink, false);
		CryptoPP::StringSource ss(tempR, true, Base64Enc);*/
		//	delete strSink, Base64Enc; //TODO IMPORTANT, deletation causes seg fault! to be revised
		DSUserAvatar tempDSUA(tempR, userID);
		std::cerr << "IMG: \n" << tempDSUA.toString() << "\n";
		return tempDSUA;

	} catch (...) {
		lError("AVATAR GET ERROR, DEBUG NEEDED!");
	}
}

void UMCore::setUserAvatar(std::string img, int userID) {
	/*string decoded;
	auto strSink = new CryptoPP::StringSink(decoded);
	auto Base64Enc = new CryptoPP::Base64Decoder(strSink);// Base64Decoder
	CryptoPP::StringSource ss(img, true, Base64Enc); // StringSource*/

	try {
		int a = executeSync("UPDATE users SET avatar='" + img + "' WHERE id=" + std::to_string(userID));
		if (a < 1) {
			EXTDBError("setUserAvatar Failed" + std::to_string(userID));
		}
	} catch (...) {
		std::cerr << "AVATAR SET ERR, DBG NEEDED!\n";
	}

}


}//usermanagement
}