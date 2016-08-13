/*
 * UManagementUtility.hpp
 *
 *  Created on: Feb 21, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_UMANAGEMENTUTILITY_HPP_
#define USERMANAGEMENT_UMANAGEMENTUTILITY_HPP_

#include <string>

namespace zeitoon {
namespace usermanagement {

class UMLoginResult {
public:
	enum UMLoginResultEnum {
		ok, invalidUserPass, banned, accessDenied, unknownError, __MAX
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
	/**user's availability state*/
	bool isOnline;

	/**Default Constructor*/
	UMUserInfo();

	/**Overloaded Constructor
	 * @param userID int*/
	UMUserInfo(int userID, UMCore *instance);
	/**
	 * getNameAndType baraye moshakhas kardan Owner e exception ast va kaarbord e ann faghat baraye exceptionEx hast
	 */
private:
	UMCore *coreInstance;

	std::string getNameAndType();
};

class commandInfo {
public:

	//----------------------------set Command names:
	static std::string login() { return "userman.login"; }

	static std::string logout() { return "userman.logout"; }

	static std::string checkpermission() { return "userman.checkPermission"; }

	static std::string checkpermissionByName() { return "userman.checkPermissionByName"; }

	static std::string addUser() { return "userman.addUser"; }

	static std::string modifyUser() { return "userman.modifyUser"; }

	static std::string removeUser() { return "userman.removeUser"; }

	static std::string getUserInfo() { return "userman.getUserInfo"; }

	static std::string registerPermission() { return "userman.registerPermission"; }

	static std::string updatePermission() { return "userman.updatePermission"; }

	static std::string removePermission() { return "userman.removePermission"; }

	static std::string registerUsergroup() { return "userman.registerUsergroup"; }

	static std::string updateUsergroup() { return "userman.updateUsergroup"; }

	static std::string removeUsergroup() { return "userman.removeUsergroup"; }

	static std::string listUsers() { return "userman.listUsers"; }

	static std::string listUsersByGroup() { return "userman.listUsersByGroup"; }

	static std::string listPermissions() { return "userman.listPermissions"; }

	static std::string listUsergroups() { return "userman.listUsergroups"; }

	static std::string addUserUsergroup() { return "userman.addUserUsergroup"; }

	static std::string removeUserUsergroup() { return "userman.removeUserUsergroup"; }

	static std::string addUserPermission() { return "userman.addUserPermission"; }

	static std::string removeUserPermission() { return "userman.removeUserPermission"; }

	static std::string listUserPermissions() { return "userman.listUserPermissions"; }

	static std::string addUsergroupPermission() { return "userman.addUsergroupPermission"; }

	static std::string removeUsergroupPermission() { return "userman.removeUsergroupPermission"; }

	static std::string listUsergroupPermissions() { return "userman.listUsergroupPermissions"; }

	static std::string addContact() { return "userman.addContact"; }

	static std::string removeContact() { return "userman.removeContact"; }

	static std::string modifyContact() { return "userman.modifyContact"; }

	static std::string listContacts() { return "userman.listContacts"; }

};

class eventInfo {
public:

	//--------------------------set Event names:
	static std::string usersUsergroupAdded() { return "userman.usersUsergroupAdded"; }

	static std::string usersUsergroupRemoved() { return "userman.usersUsergroupRemoved"; }

	static std::string usersPermissionAdded() { return "userman.usersPermissionAdded"; }

	static std::string usersPermissionRemoved() { return "userman.usersPermissionRemoved"; }

	static std::string loggedIn() { return "userman.loggedIn"; }

	static std::string loggedOut() { return "userman.loggedOut"; }

	static std::string userAdded() { return "userman.userAdded"; }

	static std::string userRemoved() { return "userman.userRemoved"; }

	static std::string userModified() { return "userman.userModified"; }

	static std::string permissionAdded() { return "userman.permissionAdded"; }

	static std::string permissionModified() { return "userman.permissionModified"; }

	static std::string permissionRemoved() { return "userman.permissionRemoved"; }

	static std::string usergroupAdded() { return "userman.usergroupAdded"; }

	static std::string usergroupModified() { return "userman.usergroupModified"; }

	static std::string usergroupRemoved() { return "userman.usergroupRemoved"; }

	static std::string usergroupPermissionAdded() { return "userman.usergroupPermissionAdded"; }

	static std::string usergroupPermissionRemoved() { return "userman.usergroupPermissionRemoved"; }
};

}//usermanagenment
}//zeitoon
#endif /* USERMANAGEMENT_UMANAGEMENTUTILITY_HPP_ */
