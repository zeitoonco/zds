/*
 * DSStructs.hpp
 *
 *  Created on: Feb 23, 2016
 *      Author: inf
 */

#ifndef USERMANAGEMENT_DTSTRUCTS_HPP_
#define USERMANAGEMENT_DTSTRUCTS_HPP_

#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"
#include "datatypes/DTTableString.hpp"
#include <vector>
#include <string>

namespace zeitoon {
namespace usermanagement {

using namespace zeitoon::datatypes;

class DSLoginInfo: public DTStruct {
public:
	DTString username = { "username" };
	DTString password = { "password" };
	static std::string getStructName() {
		return "DSLoginInfo";
	}
	static int getStructVersion() {
		return 1;
	}
	DSLoginInfo() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSLoginInfo(std::string data) :
			DSLoginInfo() {
		this->fromString(data);
	}
	DSLoginInfo(std::string iusername, std::string ipassword) :
			DSLoginInfo() {
		username = iusername;
		password = ipassword;
		this->list.push_back(&username);
		this->list.push_back(&password);
	}
};
class DSLoginResult: public DTStruct {
public:
	DTString UMLoginResult = { "UMLoginResult" };
	DTString description = { "description" };
	DTInteger<int> sessionID = { "sessionID" };
	static std::string getStructName() {
		return "DSLoginResult";
	}
	static int getStructVersion() {
		return 1;
	}
	DSLoginResult() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSLoginResult(std::string data) :
			DSLoginResult() {
		this->fromString(data);
	}
	DSLoginResult(std::string iUMLoginResult, std::string idescription, int isessionID) :
			DSLoginResult() {
		UMLoginResult = iUMLoginResult;
		description = idescription;
		sessionID = isessionID;
		this->list.push_back(&UMLoginResult);
		this->list.push_back(&description);
		this->list.push_back(&sessionID);
	}
};

class DSChkPermission: public DTStruct {
public:
	DTInteger<int> sessionID = { "sessionID" };
	DTInteger<int> permissionID = { "permissionID" };
	static std::string getStructName() {
		return "DSChkPermission";
	}
	static int getStructVersion() {
		return 1;
	}
	DSChkPermission() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSChkPermission(std::string data) :
			DSChkPermission() {
		this->fromString(data);
	}
	DSChkPermission(int isessionID, int ipermissionID) :
			DSChkPermission() {
		permissionID = ipermissionID;
		sessionID = isessionID;
		this->list.push_back(&permissionID);
		this->list.push_back(&sessionID);
	}
};

class DSAddUser: public DTStruct {
public:
	DTString username = { "username" };
	DTString password = { "password" };
	DTString name = { "name" };
	static std::string getStructName() {
		return "DSAddUser";
	}
	static int getStructVersion() {
		return 1;
	}
	DSAddUser() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSAddUser(std::string data) :
			DSAddUser() {
		this->fromString(data);
	}
	DSAddUser(std::string iusername, std::string ipassword, std::string iname) :
			DSAddUser() {
		username = iusername;
		password = ipassword;
		name = iname;
		this->list.push_back(&username);
		this->list.push_back(&password);
		this->list.push_back(&name);
	}
};

class DSModifyUser: public DTStruct {
public:
	DTInteger<int> userID = { "userID" };
	DTString username = { "username" };
	DTString password = { "password" };
	DTString name = { "name" };
	static std::string getStructName() {
		return "DSModifyUser";
	}
	static int getStructVersion() {
		return 1;
	}
	DSModifyUser() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSModifyUser(std::string data) :
			DSModifyUser() {
		this->fromString(data);
	}
	DSModifyUser(int iuserID, std::string iusername, std::string ipassword, std::string iname) :
			DSModifyUser() {
		userID = iuserID;
		username = iusername;
		password = ipassword;
		name = iname;
		this->list.push_back(&username);
		this->list.push_back(&password);
		this->list.push_back(&name);
	}
};

class DSUserInfo: public DTStruct {
public:
	DTInteger<int> userID = { "userID" };
	DTString username = { "username" };
	DTString name = { "name" };
	DTBoolean banned = { "banned" };
	DTString banReason = { "bsnReason" };
	static std::string getStructName() {
		return "DSUserInfo";
	}
	static int getStructVersion() {
		return 1;
	}
	DSUserInfo() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSUserInfo(std::string data) :
			DSUserInfo() {
		this->fromString(data);
	}
	DSUserInfo(int iuserID, std::string iusername, std::string iname, bool ibanned, std::string ibanReason) :
			DSUserInfo() {
		userID = iuserID;
		username = iusername;
		banReason = ibanReason;
		banned = ibanned;
		name = iname;
		list.push_back(&userID);
		list.push_back(&username);
		list.push_back(&banReason);
		list.push_back(&banned);
		list.push_back(&name);
	}
};

class DSRegPermission: public DTStruct {
public:
	DTString name = { "name" };
	DTString title = { "title" };
	DTString description = { "description" };
	DTInteger<int> parentID = { "parentID" };
	static std::string getStructName() {
		return "DSRegPermission";
	}
	static int getStructVersion() {
		return 1;
	}
	DSRegPermission() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSRegPermission(std::string data) :
			DSRegPermission() {
		this->fromString(data);
	}
	DSRegPermission(std::string iname, std::string ititle, std::string idescription, int iparentID) :
			DSRegPermission() {
		name = iname;
		title = ititle;
		description = idescription;
		parentID = iparentID;
		list.push_back(&name);
		list.push_back(&title);
		list.push_back(&description);
		list.push_back(&parentID);
	}
};

class DSUpdatePermission: public DTStruct {
public:
	DTInteger<int> permissiosnID = { "permissionID" };
	DTString name = { "name" };
	DTString title = { "title" };
	DTString description = { "description" };
	DTInteger<int> parentID = { "parentID" };
	static std::string getStructName() {
		return "DSUpdatePermission";
	}
	static int getStructVersion() {
		return 1;
	}
	DSUpdatePermission() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSUpdatePermission(std::string data) :
			DSUpdatePermission() {
		this->fromString(data);
	}
	DSUpdatePermission(int ipermissionID, std::string iname, std::string ititle, std::string idescription, int iparentID) :
			DSUpdatePermission() {
		permissiosnID = ipermissionID;
		name = iname;
		title = ititle;
		description = idescription;
		list.push_back(&permissiosnID);
		list.push_back(&name);
		list.push_back(&title);
		list.push_back(&description);
		list.push_back(&parentID);
	}
};

class DSRegUsrGrp: public DTStruct {
public:
	DTInteger<int> parentID = { "parentID" };
	DTString title = { "title" };
	DTString description = { "description" };
	static std::string getStructName() {
		return "DSRegUsrGrp";
	}
	static int getStructVersion() {
		return 1;
	}
	DSRegUsrGrp() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSRegUsrGrp(std::string JSONdata) :
			DSRegUsrGrp() {
		this->fromString(JSONdata);
	}
	DSRegUsrGrp(std::string ititle, int iparentID, std::string idescription) :
			DSRegUsrGrp() {
		title = ititle;
		parentID = iparentID;
		description = idescription;
		list.push_back(&title);
		list.push_back(&parentID);
		list.push_back(&description);
	}
};

class DSUpdateUsrGrp: public DTStruct {
public:
	DTInteger<int> usergroupID = { "usergroupID" };
	DTString title = { "title" };
	DTInteger<int> parentID = { "parentID" };
	DTString description = { "description" };
	static std::string getStructName() {
		return "DSUpdateUsrGrp";
	}
	static int getStructVersion() {
		return 1;
	}
	DSUpdateUsrGrp() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSUpdateUsrGrp(std::string JSONdata) :
			DSUpdateUsrGrp() {
		this->fromString(JSONdata);
	}
	DSUpdateUsrGrp(int iusergroupID, std::string ititle, int iparentID, std::string idescription) :
			DSUpdateUsrGrp() {
		usergroupID = iusergroupID;
		title = ititle;
		parentID = iparentID;
		description = idescription;
		list.push_back(&usergroupID);
		list.push_back(&title);
		list.push_back(&parentID);
		list.push_back(&description);
	}

};

class DSUserList: public DTStruct {
public:
	DTSet<DTString> usersList = { "DSUserList" };
	static std::string getStructName() {
		return "DSUserList";
	}
	static int getStructVersion() {
		return 1;
	}
	DSUserList() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {

	}
	DSUserList(std::string data) :
			DSUserList() {
		this->fromString(data);
	}
	DSUserList(std::vector<DTString> iusersList) :
			DSUserList() {
		usersList.addRange(iusersList);
	}
};

class DSPermissionsList: public DTStruct {
public:
	DTSet<DTString> permissionsList = { "listPermissions" };
	static std::string getStructName() {
		return "DSlistPermissions";
	}
	static int getStructVersion() {
		return 1;
	}
	DSPermissionsList() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSPermissionsList(std::string data) :
			DSPermissionsList() {
		this->fromString(data);
	}
	DSPermissionsList(std::vector<DTString> iusersList) :
			DSPermissionsList() {
		permissionsList.addRange(iusersList);
	}
};

class DSUserGroupsList: public DTStruct {
public:
	DTSet<DTString> userGrpsList = { "listUsergroups" };
	static std::string getStructName() {
		return "DSlistUsergroups";
	}
	static int getStructVersion() {
		return 1;
	}
	DSUserGroupsList() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {

	}
	DSUserGroupsList(std::string data) :
			DSUserGroupsList() {
		this->fromString(data);
	}
	DSUserGroupsList(std::vector<DTString> iusersList) :
			DSUserGroupsList() {
		userGrpsList.addRange(iusersList);
	}
};

class DSUserUsergroup: public DTStruct {
public:
	DTInteger<int> userID = { "userID" };
	DTInteger<int> groupID = { "groupID" };
	static std::string getStructName() {
		return "DSUserUsergroup";
	}
	static int getStructVersion() {
		return 1;
	}
	DSUserUsergroup() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSUserUsergroup(std::string data) :
			DSUserUsergroup() {
		this->fromString(data);
	}
	DSUserUsergroup(int iuserID, int igroupID) :
			DSUserUsergroup() {
		userID = iuserID;
		groupID = igroupID;
		list.push_back(&userID);
		list.push_back(&groupID);
	}
};

class DSUserPermission: public DTStruct {
public:
	DTInteger<int> userID = { "userID" };
	DTInteger<int> permissionID = { "permissionID" };
	DTInteger<int> permissionState = { "permissionState" };
	static std::string getStructName() {
		return "DSUserPermission";
	}
	static int getStructVersion() {
		return 1;
	}
	DSUserPermission() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}
	DSUserPermission(std::string data) :
			DSUserPermission() {
		this->fromString(data);
	}
	DSUserPermission(int iuserID, int ipermissionID, int ipermissionState) :
			DSUserPermission() {
		permissionID = ipermissionID;
		userID = iuserID;
		permissionState = ipermissionState;
		list.push_back(&userID);
		list.push_back(&permissionID);
		list.push_back(&permissionState);
	}
};
} //namespace: zeitoon
} //namespace: datatypes

#endif /* USERMANAGEMENT_DSSTRUCTS_HPP_ */
