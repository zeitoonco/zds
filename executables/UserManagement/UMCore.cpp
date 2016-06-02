/*
 * UMCore.cpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */
#include <utility/exceptions.hpp>
#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <executables/UserManagement/UMCore.hpp>
#include "executables/UserManagement/UmCHI.hpp"
#include "executables/UserManagement/DTStructs.hpp"
#include "exceptions.hpp"

namespace zeitoon {
namespace usermanagement {


UMCore::UMCore(UmCHI *instancePtr) :
		umCHI(instancePtr), systemLog("UserManagementLogs"), sessionManager(this) {
}

UMCore::~UMCore() {

}

UMLoginResult::UMLoginResultEnum UMCore::login(std::string username, std::string password, int &sessionID, int &uID,
                                               std::string &desc) {
	auto currentUser = userLogInfo.find(username);
	if (currentUser != userLogInfo.end()) {    //If user found on userLoginfo list.
		if (currentUser->second.banned) {
			if ((time(NULL) - currentUser->second.Banedtime) > 600) {
				userLogInfo.erase(currentUser);
			} else {
				sessionID = -1;
				uID = -1;
				desc = "User is temporary banned(too many false login attempts)";
				systemLog.log(getNameAndType(), "Login Failed for User: [" + username +
				                                "]. User is temporarily banned. (To many false login attempts) ",
				              LogLevels::note);
				return UMLoginResult::banned;
			}
		}
	}

	zeitoon::datatypes::DTTableString loginResult("loginResult");
	std::string name = "";
	int userID = 0;
	bool banned = true;
	try {//fixme:use getfield instead of execute!! && why UM breaks after last query? why 2 query?
		//if (this->executeSync("select count(*) from users where username = '" + username + "'") != 1) { //now it should save the ip and ban it if needed later on! but how? ip should be passed along
		loginResult = this->querySync("select id, banned, banreason, name from users where username='" +
		                              username + "' and password='" + hashingProccess(password) + "'");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch Loging info for user:" + username,
		            errorInfo);
	}
	if (loginResult.rowCount() == 1) {
		userID = std::stoi(loginResult.fieldValue(0, 0));
		banned = (loginResult.fieldValue(0, 1) == "t" || loginResult.fieldValue(0, 1) == "1" ||
		          loginResult.fieldValue(0, 1) == "y") ? true : false;
		name = loginResult.fieldValue(0, 3);
		if (banned) {            //IF USER IS BANNED
			sessionID = -1;
			uID = -1;
			desc = loginResult.fieldValue(0, 2);
			return UMLoginResult::banned;
		} else {    //SUCCESSFUL AUTHENTICATION
			desc = "";
			try {
				sessionID = sessionManager.newSession(userID);
				uID = userID;
				umCHI->sm.communication.runEvent(eventInfo::loggedIn(),
				                                 zeitoon::usermanagement::DSUserInfo(userID, username, name, banned,
				                                                                     desc,
				                                                                     this->isOnline(userID)).toString(
						                                 true));                //##Event fired
			} catch (exceptionEx &errorInfo) {
				EXTDBErrorI("Login: unable to register the session for " + username, errorInfo);
			}
			systemLog.log(getNameAndType(), username + " [" + std::to_string(sessionID) + "] " + "Loged in",
			              LogLevels::note);

			return UMLoginResult::ok;
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
				systemLog.log(getNameAndType(),
				              "User [" + username + "] is banned now(at [" +
				              std::to_string(currentUser->second.Banedtime)
				              + "]), because of to many wrong login inputs.", LogLevels::note);

			}

		} else {
			userLogInfoStruct currentUserLogStruct;
			currentUserLogStruct.logAttemptTimes.push_back(time(NULL));
			userLogInfo[username] = currentUserLogStruct;
		}
		sessionID = -1;
		uID = -1;
		desc = "Invalid Username or Password.";
		systemLog.log(getNameAndType(), "Invalid username or password for USER: [" + username + "]. ", LogLevels::note);
		return UMLoginResult::invalidUserPass;

	} else {    //IF ANY THING OTHER THAN PREVIOUSLY DEFINED CONDITIONS HAPPEN.
		sessionID = -1;
		uID = -1;
		desc = "Unknown Error!";
		systemLog.log(getNameAndType(), "Unknown Error on login attempt for USER: [" + username + "]. ",
		              LogLevels::trace);
		return UMLoginResult::unknownError;
	}

}

void UMCore::logout(int sessionID) {
	if (sessionID == -1)
		return;
	auto iter = sessionManager.sessionList.find(sessionID);
	std::string username = iter->second.username;//sessionManager.sessionList.find(sessionID)->second.username;
	int userID = iter->second.userID;
	sessionManager.removeSession(sessionID);
	//this->sessionManager.sessionList.at(sessionID).userID;//:: initialize upon removal of session? by inf
	umCHI->sm.communication.runEvent(eventInfo::loggedOut(),
	                                 zeitoon::usermanagement::DSUserInfo(userID, username, "", false, "",
	                                                                     this->isOnline(userID)).toString(
			                                 true));
	//##Event Fired
	systemLog.log(getNameAndType(), username + " [" + std::to_string(sessionID) + "] " + "Logged out", LogLevels::note);
}

bool UMCore::checkPermission(int sessionID,
                             int permissionID) {//todo:breaks on invalid ids!//todo: place try catch..by inf
	try {


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
		}
		return false;
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTcheckPermissionFailI("Check Permission Failed", err);
	}
}

bool UMCore::checkPermissionByName(int sessionID, string permissionName) {
	int pid;
	try {
		pid = std::stoi(this->singleFieldQuerySync("select id from permission where name='" + permissionName + "'"));
	} catch (exceptionEx &errorInfo) {
		EXTcheckPermissionFailI("checkPermissionByName Failed. Unable to get ID of permission: " + permissionName,
		                        errorInfo);
	}
	try {
		return this->checkPermission(sessionID, pid);
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTcheckPermissionFailI("checkPermissionByName Failed. Unable to checkPermission: " + permissionName, err);
	}
}

int UMCore::addUser(std::string username, std::string password, std::string name) { //regs a new user in database
///----Reg a new user in Database:

	std::string command = "INSERT INTO users (id, username, password, name, banned, banreason, avatar) VALUES"
			                      "(default, '" + username + "', '" + hashingProccess(password) + "', '" + name +
	                      "', default , default, default ) RETURNING id";
	int userID = 0;
	std::cerr << command << endl;
	try {//todo:Error if user exist??!
		userID = std::stoi(this->singleFieldQuerySync(command));
	} catch (exceptionEx &errorInfo) {
		std::cerr << "ADD-USER ERROR: " << errorInfo.what() << std::endl;
		EXTDBErrorI("Unable to add user [" + username + "] to database", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::userAdded(),
	                                 zeitoon::usermanagement::DSUserInfo(userID, username, name, false, "",
	                                                                     this->isOnline(userID)).toString(
			                                 true));
	//##Event Fired
	systemLog.log(getNameAndType(), "User added. [user: " + username + " id:" + std::to_string(userID) + "]",
	              LogLevels::note);
	return userID;
}

void UMCore::removeUser(int userID) {
	std::string userName = "";
	logout(sessionManager.getSessionIDbyUserID(userID));
	try {//remove user from users in database
		userName = this->singleFieldQuerySync(
				"DELETE FROM users WHERE id = " + std::to_string(userID) + " RETURNING username");

	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove userID[" + std::to_string(userID) + "] from database.", errorInfo);
	}
	try {        //remove all permissions of user from DB
		executeSync("DELETE FROM userpermission WHERE userid=" + std::to_string(userID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI(
				"Unable to remove permissions for [user: " + userName + " id:" + std::to_string(userID) + "]from DB",
				errorInfo);
	}

	umCHI->sm.communication.runEvent(eventInfo::userRemoved(),
	                                 zeitoon::usermanagement::DSUserInfo(userID, userName, "", false,
	                                                                     "", this->isOnline(userID)).toString(
			                                 true));//##Event Fired

	systemLog.log(getNameAndType(), "User removed. [ID: " + std::to_string(userID) + " Name: " + userName + "]",
	              LogLevels::note);
}


void UMCore::modifyUser(int userID, std::string username, std::string password, std::string name) {
	int executeResult = 0;
	try {
		executeResult = executeSync(
				"update users set username='" + username + "', password='" + hashingProccess(password) + "', name='" +
				name + "' where id="
				+ std::to_string(userID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to modify user[" + std::to_string(userID) + "] in database.", errorInfo);
	}
	if (executeResult == 1) {
		sessionManager.sessionList.at(
				userID).username = username;        //update username in umSession-->(info of active users)
		umCHI->sm.communication.runEvent(eventInfo::userModified(),
		                                 zeitoon::usermanagement::DSUserInfo(userID, username, name, false,
		                                                                     "", this->isOnline(userID)).toString(
				                                 true));
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
				"insert into permission( id, parentid, name, title, description) values(default, " +
				(parent == -1 ? string("NULL") : std::to_string(parent)) + ", '" + name + "', '" + title
				+ "', '" + desc + "')");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to register permission[" + name + "(" + title + ")" + desc + "] in database", errorInfo);
	}

	std::string queryForPermissionID =
			" select id from permission where name='" + name + "' and title='" + title + "' and description='" + desc
			+ "'and parentid=" + (parent == -1 ? string("NULL") : std::to_string(parent));

	int permissionID = 0;
	try {
		permissionID = std::stoi(this->singleFieldQuerySync(queryForPermissionID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch ID for permission[" + name + "] from database.", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::permissionAdded(),
	                                 zeitoon::usermanagement::DSUpdatePermission(permissionID, name, title, desc,
	                                                                             parent).toString(true));
	//##Event Fired
	systemLog.log(getNameAndType(), "Permission[" + name + ", ID:" + std::to_string(permissionID) + "] registered.",
	              LogLevels::note);
	return permissionID;

}

void UMCore::updatePermission(int permissionID, std::string name, std::string title, std::string desc, int parentID) {
	int executeResult = 0;
	try {
		executeResult = executeSync(
				"update permission set name='" + name + "', title='" + title + "', description='" + desc +
				"', parentid=" + (parentID == -1 ? string("NULL") : std::to_string(parentID))
				+ " where id =" + std::to_string(permissionID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to update permission[ID:" + std::to_string(permissionID) + "] in database", errorInfo);
	}
	if (executeResult == 1) {
		sessionManager.permissionParentCacheLoader(
				permissionID);        //reinitialize permissionParentCache with new parent ID for it.
		umCHI->sm.communication.runEvent(eventInfo::permissionModified(),
		                                 zeitoon::usermanagement::DSUpdatePermission(permissionID, name, title, desc,
		                                                                             parentID).toString(true));
		//##Event Fired
		systemLog.log(getNameAndType(), "Permission[ID:" + std::to_string(permissionID) + " ] updated.",
		              LogLevels::note);
	}
}

void UMCore::removePermission(int permissionID) {
	std::string permIDStr = std::to_string(permissionID);

	try {        //remove permission from users permission table in database
		executeSync("BEGIN;"
				            " delete from permission where id=" + permIDStr + ";"
				            " delete from permission where id=" + permIDStr + ";"
				            " delete from grouppermission where permissionid=" + permIDStr + ";"
				            " end;");

	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove permission[ID: " + std::to_string(permissionID) + "] from database.", errorInfo);
	}
	for (std::map<int, UMSession>::iterator iter = sessionManager.sessionList.begin();
	     iter != sessionManager.sessionList.end(); iter++) {
		iter->second.permissionsCache.erase(permissionID);
	}
	for (std::map<int, usergroupInfo>::iterator iter = sessionManager.usergroupCache.begin();
	     iter != sessionManager.usergroupCache.end(); iter++) {
		iter->second.permissions.erase(permissionID);
	}
	sessionManager.permissionParentCache.erase(permissionID);

	umCHI->sm.communication.runEvent(eventInfo::permissionRemoved(),
	                                 zeitoon::usermanagement::DSUpdatePermission(permissionID, "", "", "", -2).toString(
			                                 true));
	//##Event Fired
	systemLog.log(getNameAndType(), "Permission[ID:" + std::to_string(permissionID) + " ] removed", LogLevels::note);
}

int UMCore::registerUsergroup(std::string title, int parentID,
                              std::string desc) {        //is it user groups or just ||groups||??

	try {
		executeSync("insert into groups( id, title, parentid, description) values(default, '" + title + "', " +
		            (parentID == -1 ? string("NULL") : std::to_string(parentID)) + ", '" + desc + "')");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to register usergroup[" + title + "] in database.", errorInfo);
	}
	std::string queryForGroupID =
			"select id from groups where title='" + title + "' and parentid=" +
			(parentID == -1 ? string("NULL") : std::to_string(parentID)) +
			" and description='" + desc
			+ "'";

	int usergroupID = 0;
	try {
		usergroupID = std::stoi(this->singleFieldQuerySync(queryForGroupID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("unable to fetch groupID for [" + title + "] from database.", errorInfo);
	}
	if (usergroupID > 0) {
		umCHI->sm.communication.runEvent(eventInfo::usergroupAdded(),
		                                 zeitoon::usermanagement::DSUpdateUsrGrp(usergroupID, title, parentID,
		                                                                         desc).toString(true));
		//##Event Fired
		systemLog.log(getNameAndType(), "Usergroup[ID: " + std::to_string(usergroupID) + "] registered.",
		              LogLevels::note);
	}
	return usergroupID;
}

void UMCore::updateUsergroup(int usergroupID, std::string title, int parentID, std::string desc) {

	try {
		executeSync(
				"update groups set title='" + title + "', description='" + desc + "', parentid=" +
				(parentID == -1 ? string("NULL") : std::to_string(parentID)) + " where id="
				+ std::to_string(usergroupID));
		sessionManager.userGroupParentCache.at(usergroupID) = parentID;
		umCHI->sm.communication.runEvent(eventInfo::usergroupModified(),
		                                 zeitoon::usermanagement::DSUpdateUsrGrp(usergroupID, title, parentID,
		                                                                         desc).toString(true));
		//##Event Fired
		systemLog.log(getNameAndType(), "Usergroup[ID:" + std::to_string(usergroupID) + "] modified.", LogLevels::note);
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to update usergroup[ID: " + std::to_string(usergroupID) + "].",
		            errorInfo);
	}

}

void UMCore::removeUsergroup(int usergroupID) {

	std::string GrID = std::to_string(usergroupID);
	try {
		executeSync("BEGIN;"
				            "delete from groups where id= " + GrID + "; delete from usergroup where groupid=" + GrID +
		            "; end;");
		sessionManager.usergroupCache.erase(usergroupID);    // remove details from cache
		sessionManager.userGroupParentCache.erase(usergroupID);    //remove usergrp details from parent cchache
		umCHI->sm.communication.runEvent(eventInfo::usergroupRemoved(),
		                                 zeitoon::usermanagement::DSUpdateUsrGrp(usergroupID, "", -2, "").toString(
				                                 true));
		//##Event Fired
		systemLog.log(getNameAndType(), "Usergroup[ID: " + std::to_string(usergroupID) + "] removed.", LogLevels::note);
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove usergroup[" + std::to_string(usergroupID) + "] from database", errorInfo);
	}

}

DSUserList UMCore::listUsers() {
	DSUserList allUsers;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = this->querySync("select id,username,name,banned,banreason from users order by id");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch all usernames from database", errorInfo);
	}

	for (size_t i = 0; i < result.rowCount(); i++) {
		DTBoolean b("");
		b.fromString(result.fieldValue(i, 3));
		int userID = stoi(result.fieldValue(i, 0));
		allUsers.usersList.add(new DSUserInfo(userID,
		                                      result.fieldValue(i, 1),
		                                      result.fieldValue(i, 2),
		                                      b.getValue(),
		                                      result.fieldValue(i, 4), this->isOnline(userID)), true);
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
		EXTDBErrorI("Unable to fetch  users of group[" + std::to_string(groupID) + "] from database", errorInfo);
	}

	for (size_t i = 0; i < result.rowCount(); i++) {
		DTBoolean b("");
		b.fromString(result.fieldValue(i, 3));
		allUsersOfSpecificGroup.usersList.add(
				new DSUserInfo(stoi(result.fieldValue(i, 0)), result.fieldValue(i, 1), result.fieldValue(i, 2),
				               b.getValue(), result.fieldValue(i, 4), this->isOnline(stoi(result.fieldValue(i, 0)))),
				true);
	}
	return allUsersOfSpecificGroup;
}

DSPermissionsList UMCore::listPermissions() {
	DSPermissionsList allPermissions;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = querySync("select id,parentid,name,title,description from permission order by id");
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

DSUserGroupsList UMCore::listUsergroups() {
	DSUserGroupsList allGroups;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = querySync("select id,title,parentid,description from groups order by id");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch usergroups data from database", errorInfo);
	}
	for (size_t i = 0; i < result.rowCount(); i++) {
		allGroups.userGrpsList.add(new DSUpdateUsrGrp(stoi(result.fieldValue(i, 0)),
		                                              result.fieldValue(i, 1),
		                                              result.fieldIsNull(i, 2) ? -1 : stoi(result.fieldValue(i, 2)),
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

//----------------------------------------------------

std::string UMCore::hashingProccess(
		std::string STRING) {        //pw+(random salt saved to DB)
	CryptoPP::SHA256 sha1;
	std::string hashedVal = "";
	std::string salt = "L+8D fk.3#|";
	STRING += salt;
	CryptoPP::StringSource(STRING, true, new CryptoPP::HashFilter(sha1, new CryptoPP::HexEncoder(
			new CryptoPP::StringSink(hashedVal))));
	return hashedVal;
}

int UMCore::checkUserPermissionState(int sessionID, int permissionID) {
//if permission exist. check the state
	const std::map<int, int> &permissioncache = sessionManager.sessionList.at(sessionID).permissionsCache;
	auto returnVal = permissioncache.find(permissionID);
	if (returnVal != permissioncache.end())
		return returnVal->second;
	return 0;

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
		tempParentID = tempParentCache.find(permissionID)->second;    ///->second;
	}
	int tempState = 0;
	while (tempParentID != -1) {
		/*std::map<int, int>::const_iterator currentPermission = tempPermissionCache.find(tempParentID);
		if (tempPermissionCache.count(tempParentID) != 1) {

			//if ? tempstate not found on the map,check again for the parent of parent and Continue from start of the loop;
			std::cout << "\nNO STATE FOUND FOR PERMISSION " << tempParentID << "\n";
			tempParentID = tempParentCache.find(tempParentID)->second;	//fixme:invalid
			//todo:@navidi: r u sur eits update? no need for db query?
			// getPermissionParent(tempParentID);//tempParentCache.find(tempParentID)->second;
			continue;
		}*/
		int state = 0;

		DTTableString dt = querySync(
				"select state from userpermission where permissionid=" + std::to_string(tempParentID) + " and userid=" +
				to_string(sessionManager.sessionList.at(sessionID).userID));

		if (dt.rowCount() > 0)
			state = stoi(dt.fieldValue(0, 0));// ajl: a quick fix for bugged commented codes
		switch (state) {
			case (-1):
				std::cout << "\nPERMISSION STATE OF " << state << " IS -1\n";
				return -1;
			case (0):
				std::cout << "\nPERMISSION STATE OF " << state << " IS  0\n";
				break;
			case (1):
				std::cout << "\nPERMISSION STATE OF " << state << " IS  1\n";
				tempState = 1;
				break;
			default:
				break;
		}
		tempParentID = getPermissionParent(tempParentID);
		//tempParentID = tempParentCache.find(tempParentID)->second;
	}
	return tempState;
}

int UMCore::checkUsergroupPermission(int sessionID, int permissionID) {
	int tempState = 0;
	const std::vector<int> &USERGROUPS = sessionManager.sessionList.at(sessionID).usergruops;
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
			continue;    //there sshould be another try to populate usergroupinfo and user it , if not then Continue!!!
		} else {        //if usergroup found!
			//	sessionManager.usergroupCache.find(23)->second
			//while permission parent != -1 { if case(0): or case(1): tempPermissionParent = permissionParent.Find(permissionID)->second
			//continue;
			std::cout << "\nBOOOOOLean" <<
			USERGROUPCACHE.at(currentusergroup).permissions.find(tempPermissionParent)->second << "\n";

			while (tempPermissionParent != -1) {
				if (breakHandler) {
					break;
				}            //breakHandler is incharge of breaking out of the while-loop when needed
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
						std::cout << "\nPermission state of permission " << tempPermissionParent << " of group " <<
						currentusergroup << " is: -1\n";
						return -1;
					case (0):
						std::cout << "\nPermission state of permission " << tempPermissionParent << " of group " <<
						currentusergroup << " is: 0\n";
						if (PERMISSIONPARENT.count(tempPermissionParent) != 1) {
							breakHandler = true;
							break;
						}
						tempPermissionParent = PERMISSIONPARENT.find(tempPermissionParent)->second;
						continue;
						//check parent
					case (1):
						std::cout << "\nPermission state of permission " << tempPermissionParent << " of group " <<
						currentusergroup << " is: 1\n";
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
}            ////PERMISSION HA BAAYAAD DAR KHODESHOON CHECK BESHAN!!

int UMCore::checkUsergroupParentPermission(int sessionID, int permissionID) {

	const std::vector<int> &tempUserGroups = sessionManager.sessionList.at(sessionID).usergruops;
	const std::map<int, int> &tempUsergroupParentCache = sessionManager.userGroupParentCache;
	const std::map<int, usergroupInfo> &tempUserGroupCache = sessionManager.usergroupCache;
	const std::map<int, int> &permissionParentCache = sessionManager.permissionParentCache;
	int tempState = 0;
	int tempParentID = 0;
	for (unsigned int i = 0; i < tempUserGroups.size(); i++) {            //std::vector<int> usergroups

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
		auto r = querySync("select parentid from permission where id=" + std::to_string(permissionID));
		if (r.rowCount() < 1)
			return -1;
		return r.fieldIsNull(0, 0) ? -1 : std::stoi(
				r.fieldValue(0, 0));//fixme:invalid id breaks here// return -1 is aaded as a temporaty workaround by inf
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("unable to get permissionParent from database for PermissionID: " + std::to_string(permissionID),
		            errorInfo);
	}
}

int UMCore::getUsergroupParent(int grouptID) {
	///it first looks up the cache
	auto tempRes = this->sessionManager.userGroupParentCache.find(grouptID);
	if (tempRes != this->sessionManager.userGroupParentCache.end())
		return tempRes->second;
	try {
		//if not found on the cache
		return std::stoi(
				querySync("select parentid from groups where id=" + std::to_string(grouptID)).fieldValue(0, 0));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("unable to fetch groupParentID of " + std::to_string(grouptID) + " form Database", errorInfo);
	}

}

void UMCore::addUserUsergroup(int userID, int groupID) {
	try {
		int a = executeSync(
				"insert into usergroup values(" + std::to_string(userID) + ", " + std::to_string(groupID) + ")");
		if (a != 1)
			EXTDBError("INSER FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to addUserUsergroup", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersUsergroupAdded(),
	                                 zeitoon::usermanagement::DSUserUsergroup(userID, groupID).toString(true));
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
}

void UMCore::addUserPermission(int userID, int permissionID, int state) {
	try {
		int a = executeSync(
				"insert into userpermission values(" + std::to_string(userID) + ", " + std::to_string(permissionID) +
				", " + std::to_string(state) + ")");
		if (a != 1)
			EXTDBError("INSERT FAILED");

	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to add the permission for the user", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersPermissionAdded(),
	                                 zeitoon::usermanagement::DSUserPermission(userID, permissionID, state).toString(
			                                 true));
}

void UMCore::removeUserPermission(int userID, int permissionID) {
	try {
		int a = executeSync("delete from userpermission where userid=" + std::to_string(userID) + " and permissionid=" +
		                    std::to_string(permissionID));
		if (a != 1)
			EXTDBError("DELETE FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove user's permission", errorInfo);
	}
	umCHI->sm.communication.runEvent(eventInfo::usersPermissionRemoved(),
	                                 zeitoon::usermanagement::DSUserPermission(userID, permissionID, 0).toString(
			                                 true));
}

DSUserPermissionList UMCore::listUserPermissions(int userID) {
	DSUserPermissionList list;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = querySync("select permissionid,state from userpermission where userid=" + to_string(userID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch  permission names from database", errorInfo);
	}
	try {
		for (size_t i = 0; i < result.rowCount(); i++) {
			list.permissionsList.add(new DSPermissionState(stoi(result.fieldValue(i, 0)),
			                                               stoi(result.fieldValue(i, 1))), true);
		}
		return list;
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTDBErrorI("Unable to make a list of permissions", err);
	}
}

void UMCore::addUsergroupPermission(int usergroupID, int permissionID, int state) {
	try {
		int a = executeSync(
				"insert into grouppermission(permissionid,groupid,state) values(" +
				std::to_string(permissionID) + ", " + std::to_string(usergroupID) +
				", " + std::to_string(state) + ")");
		if (a != 1)
			EXTDBError("INSERT FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to add the permission for the user", errorInfo);
	}
	/*umCHI->sm.communication.runEvent(eventInfo::usersPermissionAdded(),//todo: no node has been defined for addUsergroupPermission
	                                 zeitoon::usermanagement::DSUserPermission(userID, permissionID, state).toString(
			                                 true));*/
}

void UMCore::removeUsergroupPermission(int usergroupID, int permissionID, int state) {
	try {
		int a = executeSync(
				"delete from grouppermission where groupid=" + std::to_string(usergroupID) + " and permissionid=" +
				std::to_string(permissionID));
		if (a != 1)
			EXTDBError("DELETE FAILED");
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to remove user's permission", errorInfo);
	}
	/*umCHI->sm.communication.runEvent(eventInfo::usersPermissionRemoved(),//todo: no node has been defined for removeUsergroupPermission
	                                 zeitoon::usermanagement::DSUserPermission(userID, permissionID, state).toString(
			                                 true));*/
}

DSUsergroupPermissionList UMCore::listUsergroupPermissions(int usergroupID) {
	DSUsergroupPermissionList list;
	zeitoon::datatypes::DTTableString result("");
	try {
		result = querySync("select permissionid,state from grouppermission where groupid=" + to_string(usergroupID));
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to fetch  permission names from database", errorInfo);
	}
	try {
		for (size_t i = 0; i < result.rowCount(); i++) {
			list.permissionsList.add(
					new DSPermissionState(stoi(result.fieldValue(i, 0)), stoi(result.fieldValue(i, 1))),
					true);
		}
	} catch (zeitoon::utility::exceptionEx &err) {
		EXTDBErrorI("Unable to make a list of userGroupPermissions", err);

	}
	return list;
}

std::string UMCore::getNameAndType() {
	return "UMCore";
}

bool UMCore::isOnline(int userID) {//if sessionID = -1 ==> user is offline
	return (this->sessionManager.getSessionIDbyUserID(userID) != -1);

}


}//usermanagement
}