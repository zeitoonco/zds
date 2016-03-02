/*
 * UManagementUtility.hpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_UMANAGEMENTUTILITY_HPP_
#define USERMANAGEMENT_UMANAGEMENTUTILITY_HPP_
#include <string>

namespace zeitoon{
namespace usermanagement{

class UMLoginResult {
public:
	enum UMLoginResultEnum {
		ok, invalidUserPass, banned, unknownError,__MAX
	};

	static std::string typeString[UMLoginResult::__MAX];

};

class UMCore;
class UMUserInfo {
public:
	/**Moteghayere haviye id e user*/
	int id;
	/**Moteghayere haviye username*/
	std::string username;
	/**Moteghayere haaviye name*/
	std::string name;
	/**Moteghayere Boolean ban*/
	bool banned;
	/**Moteghayere haviye dalil e ban shodan*/
	std::string banReason;
	/**Default Constructor*/
	UMUserInfo();
	/**Overloaded Constructor
	 * @param userID int*/
	UMUserInfo(int userID, UMCore* instance);
	/**
	 * getNameAndType baraye moshakhas kardan Owner e exception ast va kaarbord e ann faghat baraye exceptionEx hast
	 */
private:
	UMCore* coreInstance;
	std::string getNameAndType();
};

class commandInfo{
public:
	commandInfo(){
	}
static std::string login;
static std::string logout;
static std::string checkpermission;
static std::string addUser;
static std::string modifyUser;
static std::string removeUser;
static std::string getUserInfo;
static std::string registerPermission;
static std::string updatePermission;
static std::string removePermission;
static std::string registerUsergroup;
static std::string updateUsergroup;
static std::string removeUsergroup;
static std::string listUsers;
static std::string listUsersByGroup;
static std::string listPermissions;
static std::string listUsergroups;
static std::string addUserUsergroup;
static std::string removeUserUsergroup;
static std::string addUserPermission;
static std::string removeUserPermission;

};
class eventInfo{
public:
	eventInfo(){
	}
	static std::string usersUsergroupAdded;
	static std::string usersUsergroupRemoved;
	static std::string usersPermissionAdded;
	static std::string usersPermissionRemoved;
	static std::string loggedIn;
	static std::string loggedOut;
	static std::string userAdded;
	static std::string userRemoved;
	static std::string userModified;
	static std::string permissionAdded;
	static std::string permissionModified;
	static std::string permissionRemoved;
	static std::string usergroupAdded;
	static std::string usergroupModified;
	static std::string usergroupRemoved;
};

}//usermanagenment
}//zeitoon
#endif /* USERMANAGEMENT_UMANAGEMENTUTILITY_HPP_ */
