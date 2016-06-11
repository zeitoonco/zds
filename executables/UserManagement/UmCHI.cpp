/*
 * UmCHI.cpp
 *
 *  Created on: Feb 18, 2016
 *      Author: inf
 */
#include <executables/UserManagement/UmCHI.hpp>
#include <fstream>
#include <unistd.h>
#include "executables/PGdatabase/DTStructs.hpp"
#include "executables/PGdatabase/pgutility.hpp"
#include "executables/_core/coreutility.hpp"
#include <deque>
#include "datatypes/dtsingletypes.hpp"
#include "UManagementUtility.hpp"
#include "DTStructs.hpp"

namespace zeitoon {
namespace usermanagement {

UmCHI::UmCHI() :
		CommunicationHandlerInterface(this), userMngrInterface(this),
		insInfo("UserManagement", "User Management", 1, 1, EnmServiceType::UserManager) {
	setInstallInfo();
}

void UmCHI::onCommand(string node, string data, string id, string from) {
	try {
		if (!Strings::compare(node, commandInfo::login(), false)) {
			DSLoginInfo logInfo(data);
			int sessionID, userID;
			std::string description;
			std::string UMlogResString =
					UMLoginResult::typeString[userMngrInterface.login(
							logInfo.username.getValue(), logInfo.password.getValue(), sessionID, userID, description)];
			DSLoginResult logResult(UMlogResString, description, sessionID, userID);
			sm.communication.runCallback(node, logResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::logout(), false)) {
			DSInteger sessionID;
			sessionID.fromString(data);
			userMngrInterface.logout(sessionID.value.getValue());
		} else if (!Strings::compare(node, commandInfo::checkpermission(), false)) {
			DSChkPermission checkInfo(data);
			DSBoolean checkResult;
			checkResult.value = userMngrInterface.checkPermission(checkInfo.sessionID.getValue(),
			                                                      checkInfo.permissionID.getValue());
			sm.communication.runCallback(node, checkResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::checkpermissionByName(), false)) {
			DSChkPermissionByName checkInfo(data);
			DSBoolean checkResult;
			checkResult.value = userMngrInterface.checkPermissionByName(checkInfo.sessionID.getValue(),
			                                                            checkInfo.pname.getValue());
			sm.communication.runCallback(node, checkResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::addUser(), false)) {
			DSAddUser adUsrInfo(data);
			DSInteger addResult;
			addResult.value = userMngrInterface.addUser(adUsrInfo.username.getValue(), adUsrInfo.password.getValue(),
			                                            adUsrInfo.name.getValue());
			sm.communication.runCallback(node, addResult.toString(true),
			                             id);
		} else if (!Strings::compare(node, commandInfo::modifyUser(), false)) {
			DSModifyUser userInfo(data);
			userMngrInterface.modifyUser(userInfo.userID.getValue(), userInfo.username.getValue(),
			                             userInfo.password.getValue(), userInfo.name.getValue());
		} else if (!Strings::compare(node, commandInfo::removeUser(), false)) {
			DSInteger userID;
			userID.fromString(data);
			userMngrInterface.removeUser(userID.value.getValue());
		} else if (!Strings::compare(node, commandInfo::getUserInfo(), false)) {
			DSInteger userId;
			userId.fromString(data);
			UMUserInfo temp = userMngrInterface.getUserInfo(userId.value.getValue());
			DSUserInfo userInfo(temp.id, temp.username, temp.name, temp.banned, temp.banReason, temp.isOnline);
			sm.communication.runCallback(node, userInfo.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::registerPermission(), false)) {
			DSRegPermission permissionInfo(data);
			DSInteger regResult;
			regResult.value = userMngrInterface.registerPermission(permissionInfo.name.value(),
			                                                       permissionInfo.title.getValue(),
			                                                       permissionInfo.description.getValue(),
			                                                       permissionInfo.parentID.getValue());
			sm.communication.runCallback(node, regResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::updatePermission(), false)) {
			DSUpdatePermission updateInfo(data);
			userMngrInterface.updatePermission(updateInfo.permissiosnID.getValue(), updateInfo.name.getValue(),
			                                   updateInfo.title.getValue(),
			                                   updateInfo.description.getValue(), updateInfo.parentID.getValue());
		} else if (!Strings::compare(node, commandInfo::removePermission(), false)) {
			DSInteger removeID;
			removeID.fromString(data);
			userMngrInterface.removePermission(removeID.value.getValue());
		} else if (!Strings::compare(node, commandInfo::registerUsergroup(), false)) {
			DSRegUsrGrp usrGrpInfo(data);
			DSInteger regResult;
			regResult.value = userMngrInterface.registerUsergroup(usrGrpInfo.title.getValue(),
			                                                      usrGrpInfo.parentID.getValue(),
			                                                      usrGrpInfo.description.getValue());
			sm.communication.runCallback(node, regResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::updateUsergroup(), false)) {
			DSUpdateUsrGrp usrGrpInfo(data);
			userMngrInterface.updateUsergroup(usrGrpInfo.usergroupID.getValue(), usrGrpInfo.title.getValue(),
			                                  usrGrpInfo.parentID.getValue(),
			                                  usrGrpInfo.description.getValue());
		} else if (!Strings::compare(node, commandInfo::removeUsergroup(), false)) {
			DSInteger usrGrpID;
			usrGrpID.fromString(data);
			userMngrInterface.removeUsergroup(usrGrpID.value.getValue());
		} else if (!Strings::compare(node, commandInfo::listUsers(), false)) {
			DSUserList usersList = userMngrInterface.listUsers();
			sm.communication.runCallback(node, usersList.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::listUsersByGroup(), false)) {
			DSInteger groupID;
			groupID.fromString(data);
			DSUserList usrListByGrp = userMngrInterface.listUsersByGroup(groupID.value.getValue());
			sm.communication.runCallback(node, usrListByGrp.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::listPermissions(), false)) {
			DSPermissionsList permsList = userMngrInterface.listPermissions();
			sm.communication.runCallback(node, permsList.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::listUsergroups(), false)) {
			DSUserGroupsList usrGrpList = userMngrInterface.listUsergroups();
			sm.communication.runCallback(node, usrGrpList.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::addUserUsergroup(), false)) {
			zeitoon::usermanagement::DSUserUsergroup regInfo(data);
			userMngrInterface.addUserUsergroup(regInfo.userID.getValue(), regInfo.groupID.getValue());

		} else if (!Strings::compare(node, commandInfo::removeUserUsergroup(), false)) {
			zeitoon::usermanagement::DSUserUsergroup regInfo(data);
			userMngrInterface.removeUserUsergroup(regInfo.userID.getValue(), regInfo.groupID.getValue());

		} else if (!Strings::compare(node, commandInfo::addUserPermission(), false)) {
			zeitoon::usermanagement::DSUserPermission regInfo(data);
			userMngrInterface.addUserPermission(regInfo.userID.getValue(), regInfo.permissionID.getValue(),
			                                    regInfo.permissionState.getValue());
		} else if (!Strings::compare(node, commandInfo::removeUserPermission(), false)) {
			zeitoon::usermanagement::DSUserPermission regInfo(data);
			userMngrInterface.removeUserPermission(regInfo.userID.getValue(), regInfo.permissionID.getValue());
		} else if (!Strings::compare(node, commandInfo::listUserPermissions(), false)) {
			DSInteger userID;
			userID.fromString(data);
			DSUserPermissionList permsList = userMngrInterface.listUserPermissions(userID.value.value());
			sm.communication.runCallback(node, permsList.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::addUsergroupPermission(), false)) {
			zeitoon::usermanagement::DSUsergroupPermission regInfo(data);
			userMngrInterface.addUsergroupPermission(regInfo.usergroupID.getValue(), regInfo.permissionID.getValue(),
			                                         regInfo.permissionState.getValue());
		} else if (!Strings::compare(node, commandInfo::removeUsergroupPermission(), false)) {
			zeitoon::usermanagement::DSUsergroupPermission regInfo(data);
			userMngrInterface.removeUsergroupPermission(regInfo.usergroupID.getValue(), regInfo.permissionID.getValue(),
			                                            regInfo.permissionState.getValue());
		} else if (!Strings::compare(node, commandInfo::listUsergroupPermissions(), false)) {
			DSInteger usergroupID;
			usergroupID.fromString(data);
			DSUsergroupPermissionList permsList = userMngrInterface.listUsergroupPermissions(usergroupID.value.value());
			sm.communication.runCallback(node, permsList.toString(true), id);
		}
	} catch (exceptionEx &errorInfo) {
		std::cerr << "\nUMCHI ERROR:\n" << errorInfo.what() << "\n";
		sm.communication.errorReport(node, id, errorInfo.what());
		userMngrInterface.systemLog.log("node: " + node + " id:" + id + " errMsg:" + errorInfo.what());
	} catch (exception &errorInfo) {
		sm.communication.errorReport(node, id, errorInfo.what());
		userMngrInterface.systemLog.log("node: " + node + " id:" + id + " errMsg:" + errorInfo.what());
	}
}

void UmCHI::onCallback(string node, string data, string id, string from) {

}

void UmCHI::onEvent(string node, string data, string from) {

}

void UmCHI::onInstall(string id) {
	string cpath = FileSystemUtility::getAppPath();
	//Addressing the file and checking Database tables
	string temp = cpath + "DBTableScripts.sql";
	std::cout << temp << endl;
	std::ifstream t(temp);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	int res;
	try {
		res = userMngrInterface.executeSync(str);
	} catch (exceptionEx &errorInfo) {
		EXTDBErrorI("Unable to create default tables for UM", errorInfo);
	}
	if (res == -1)
		EXTDBError("Sql error");
	int uid = userMngrInterface.addUser("admin", "admin", "admin");
	userMngrInterface.addUserPermission(uid, 0, 1);
	//set serviceID in confMgr
	UMconfig.serviceID = id;
	UMconfig.save();
}

void UmCHI::onEnable() {
	//--------register cmds
	std::string temp = "";
	size_t length = insInfo.commands.length();
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.commands[i]->name.toString();
	}
	sm.communication.registerCommand(temp);
	std::cout << temp << endl;

	//--------register events
	length = insInfo.events.length();
	temp = "";
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.events[i]->name.toString();
	}
	sm.communication.registerEvent(temp);
	std::cout << temp << endl;

	//--------register hooks
	length = insInfo.hooks.length();
	temp = "";
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.hooks[i]->name.toString();
	}
	sm.communication.registerHook(temp);
	std::cout << temp << endl;

	this->userMngrInterface.loadCaches();///todo: to be checked and trace-checked

}

void UmCHI::onDisable() {
}

void UmCHI::onUninstall() {//fixme: remove db things
	UMconfig.serviceID = "";
	UMconfig.save();
}

void UmCHI::onConnect() {
	std::cerr << "\n+UM Connected to server\n";
}

void UmCHI::onDisconnect() {
	std::cerr << "\n-UM Disconnected from server\n";
}

string UmCHI::getInstallInfo() {
	return insInfo.toString(true);
}

string UmCHI::getInstallID() {
	return UMconfig.serviceID.getValue();
}

string UmCHI::getServiceName() {
	return "UserManagement";
}

std::size_t UmCHI::getServiceVersion() {
	return 1;
}

string UmCHI::changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion,
                                    int &newVersion) {
	return "";
}

void UmCHI::onError(string node, string id, string description) {
	std::cerr << "Error:\t" << description << std::endl;
}

void UmCHI::onWarning(string level, string node, string id, string description) {
	std::cerr << "Warning:\t" << description << std::endl;
}

void UmCHI::checkDBTables() {
//Finding executable path
	char buf[1000];
	ssize_t s = readlink("/proc/self/exe", buf, 1000);
	buf[s] = 0;
	std::string temp = buf;
	size_t lastSlash = temp.find_last_of("/");
	temp.erase(lastSlash + 1, std::string::npos);
//Addressing the file
	temp += "DBTableScripts.sql";
	std::cout << temp << endl;
	std::ifstream t(temp);
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

}

void UmCHI::pong(string id, int miliseconds) {
	sm.send(
			"{\"type\" : \"internal\" , \"node\" : \"ping\" , \"name\" : \"" + getServiceName() +
			"\" , \"version\" : \"5\""
			+ (id.length() > 0 ? " , \"id\" : \"" + getInstallID() + "\"" : "") + "}");
}

void UmCHI::setInstallInfo() {

//---------set available commands info

	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::login(), DSLoginInfo::getStructName(),
			                                   DSLoginInfo::getStructVersion(),
			                                   DSLoginResult::getStructName(), DSLoginResult::getStructVersion()),
			true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::logout(), DSInteger::getStructName(),
	                                                        DSInteger::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::checkpermission(), DSChkPermission::getStructName(),
			                                   DSChkPermission::getStructVersion(),
			                                   DSBoolean::getStructName(), DSBoolean::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::checkpermissionByName(),
			                                   DSChkPermissionByName::getStructName(),
			                                   DSChkPermissionByName::getStructVersion(),
			                                   DSBoolean::getStructName(), DSBoolean::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::addUser(), DSAddUser::getStructName(),
			                                   DSAddUser::getStructVersion(), DSInteger::getStructName(),
			                                   DSInteger::getStructVersion()), true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::modifyUser(), DSModifyUser::getStructName(),
	                                                        DSModifyUser::getStructVersion(), "", 0),
	                     true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::removeUser(), DSInteger::getStructName(),
	                                                        DSInteger::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::getUserInfo(), DSInteger::getStructName(),
			                                   DSInteger::getStructVersion(), DSUserInfo::getStructName(),
			                                   DSUserInfo::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::registerPermission(), DSRegPermission::getStructName(),
			                                   DSRegPermission::getStructVersion(),
			                                   DSInteger::getStructName(), DSInteger::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::updatePermission(), DSUpdatePermission::getStructName(),
			                                   DSUpdatePermission::getStructVersion(), "",
			                                   0), true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::removePermission(), DSInteger::getStructName(),
	                                                        DSInteger::getStructVersion(), "", 0),
	                     true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::registerUsergroup(), DSRegUsrGrp::getStructName(),
			                                   DSRegUsrGrp::getStructVersion(),
			                                   DSInteger::getStructName(), DSInteger::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::updateUsergroup(), DSUpdateUsrGrp::getStructName(),
			                                   DSUpdateUsrGrp::getStructVersion(), "", 0), true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::removeUsergroup(), DSInteger::getStructName(),
	                                                        DSInteger::getStructVersion(), "", 0),
	                     true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::listUsers(), "", 0, DSUserList::getStructName(),
			                                   DSUserList::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::listUsersByGroup(), "", 0, DSUserList::getStructName(),
			                                   DSUserList::getStructVersion()),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::listPermissions(), "", 0,
			                                   DSPermissionsList::getStructName(),
			                                   DSPermissionsList::getStructVersion()),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::listUsergroups(), DSUserGroupsList::getStructName(),
			                                   DSUserGroupsList::getStructVersion(), "", 0),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::addUserUsergroup(), DSUserUsergroup::getStructName(),
			                                   DSUserUsergroup::getStructVersion(), "", 0),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::removeUserUsergroup(), DSUserUsergroup::getStructName(),
			                                   DSUserUsergroup::getStructVersion(), "", 0),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::addUserPermission(), DSUserPermission::getStructName(),
			                                   DSUserPermission::getStructVersion(), "", 0),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::removeUserPermission(), DSUserPermission::getStructName(),
			                                   DSUserPermission::getStructVersion(), "",
			                                   0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::listUserPermissions(), DSInteger::getStructName(),
			                                   DSInteger::getStructVersion(),
			                                   DSUserPermissionList::getStructName(),
			                                   DSUserPermissionList::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::addUsergroupPermission(),
			                                   DSUsergroupPermission::getStructName(),
			                                   DSUsergroupPermission::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::removeUsergroupPermission(),
			                                   DSUsergroupPermission::getStructName(),
			                                   DSUsergroupPermission::getStructVersion(), "",
			                                   0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::listUsergroupPermissions(), DSInteger::getStructName(),
			                                   DSInteger::getStructVersion(),
			                                   DSUsergroupPermissionList::getStructName(),
			                                   DSUsergroupPermissionList::getStructVersion()), true);

//--------set available events info

	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::loggedIn(), DSUserInfo::getStructName(),
	                                                    DSUserInfo::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::loggedOut(), DSUserInfo::getStructName(),
	                                                    DSUserInfo::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::userAdded(), DSUserInfo::getStructName(),
	                                                    DSUserInfo::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::userModified(), DSUserInfo::getStructName(),
	                                                    DSUserInfo::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::userRemoved(), DSUserInfo::getStructName(),
	                                                    DSUserInfo::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::permissionAdded(), DSUpdatePermission::getStructName(),
			                                 DSUpdatePermission::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::permissionModified(), DSUpdatePermission::getStructName(),
			                                 DSUpdatePermission::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::permissionRemoved(), DSUpdatePermission::getStructName(),
			                                 DSUpdatePermission::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::usergroupAdded(), DSUpdateUsrGrp::getStructName(),
	                                                    DSUpdateUsrGrp::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::usergroupModified(), DSUpdateUsrGrp::getStructName(),
	                                                    DSUpdateUsrGrp::getStructVersion()),
	                   true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::usergroupRemoved(), DSUpdateUsrGrp::getStructName(),
	                                                    DSUpdateUsrGrp::getStructVersion()),
	                   true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usersUsergroupRemoved(), DSUserUsergroup::getStructName(),
			                                 DSUserUsergroup::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usersUsergroupAdded(), DSUserUsergroup::getStructName(),
			                                 DSUserUsergroup::getStructVersion()),
			true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usersPermissionRemoved(), DSUserPermission::getStructName(),
			                                 DSUserPermission::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usersPermissionAdded(), DSUserPermission::getStructName(),
			                                 DSUserPermission::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usergroupPermissionAdded(), DSUserPermission::getStructName(),
			                                 DSUserPermission::getStructVersion()), true);

	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usergroupPermissionRemoved(), DSUserPermission::getStructName(),
			                                 DSUserPermission::getStructVersion()), true);


//-------set calls

	insInfo.calls.add(
			new DSInstallInfo::DSInstallInfoCommandCallDetail(zeitoon::pgdatabase::commandInfo::query(),
			                                                  zeitoon::pgdatabase::DSDBQuery::getStructName(),
			                                                  zeitoon::pgdatabase::DSDBQuery::getStructVersion(),
			                                                  zeitoon::pgdatabase::DSDBTable::getStructName(),
			                                                  zeitoon::pgdatabase::DSDBTable::getStructVersion()),
			true);
	insInfo.calls.add(
			new DSInstallInfo::DSInstallInfoCommandCallDetail(zeitoon::pgdatabase::commandInfo::execute(),
			                                                  zeitoon::pgdatabase::DSDBQuery::getStructName(),
			                                                  zeitoon::pgdatabase::DSDBQuery::getStructVersion(),
			                                                  zeitoon::datatypes::DSInteger::getStructName(),
			                                                  zeitoon::datatypes::DSInteger::getStructVersion()), true);
	insInfo.calls.add(
			new DSInstallInfo::DSInstallInfoCommandCallDetail(zeitoon::pgdatabase::commandInfo::singlefieldquery(),
			                                                  zeitoon::pgdatabase::DSDBQuery::getStructName(),
			                                                  zeitoon::pgdatabase::DSDBQuery::getStructVersion(),
			                                                  zeitoon::datatypes::DSString::getStructName(),
			                                                  zeitoon::datatypes::DSString::getStructVersion()), true);

//------set requirements

	insInfo.installRequirements.add(new DSInstallInfo::DSInstallInfoRequirementDetail("PGDatabase", 1), true);
	insInfo.enableRequirements.add(new DSInstallInfo::DSInstallInfoRequirementDetail("PGDatabase", 1), true);

//------set datatype names

	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSLoginInfo::getStructName(),
			                                                zeitoon::usermanagement::DSLoginInfo::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSLoginResult::getStructName(),
			                                                zeitoon::usermanagement::DSLoginResult::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSChkPermission::getStructName(),
			                                                zeitoon::usermanagement::DSChkPermission::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSAddUser::getStructName(),
			                                                zeitoon::usermanagement::DSAddUser::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSModifyUser::getStructName(),
			                                                zeitoon::usermanagement::DSModifyUser::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserInfo::getStructName(),
			                                                zeitoon::usermanagement::DSUserInfo::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSRegPermission::getStructName(),
			                                                zeitoon::usermanagement::DSRegPermission::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(
					zeitoon::usermanagement::DSUpdatePermission::getStructName(),
					zeitoon::usermanagement::DSUpdatePermission::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSRegUsrGrp::getStructName(),
			                                                zeitoon::usermanagement::DSRegUsrGrp::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUpdateUsrGrp::getStructName(),
			                                                zeitoon::usermanagement::DSUpdateUsrGrp::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserList::getStructName(),
			                                                zeitoon::usermanagement::DSUserList::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSPermissionsList::getStructName(),
			                                                zeitoon::usermanagement::DSPermissionsList::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserGroupsList::getStructName(),
			                                                zeitoon::usermanagement::DSUserGroupsList::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserUsergroup::getStructName(),
			                                                zeitoon::usermanagement::DSUserUsergroup::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserPermission::getStructName(),
			                                                zeitoon::usermanagement::DSUserPermission::getStructVersion()),
			true);
	///------------set available hooks

	insInfo.hooks.add(new DSInstallInfo::DSHookDetail(zeitoon::_core::eventInfo::onServiceUninstall(), "", 0), true);
}
} //usermanagement
} //zeitoon

