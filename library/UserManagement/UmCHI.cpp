/*
 * UmCHI.cpp
 *
 *  Created on: Feb 18, 2016
 *      Author: inf
 */
#include <UserManagement/UmCHI.hpp>
#include <fstream>
#include "UMCore.hpp"
#include "DSStructs.hpp"
#include "PGdatabase/DTStructs.hpp"
#include "PGdatabase/pgutility.hpp"

#include "datatypes/dtsingletypes.hpp"

namespace zeitoon {
namespace usermanagement {

UmCHI::UmCHI(std::string serverIP, int serverPort) :
		CommunicationHandlerInterface(this, serverIP, serverPort), userMngrInterface(this), insInfo("UserManagement", "User Management", 1) {
	setInstallInfo();
}

void UmCHI::onCommand(string node, string data, string id, string from) {
	try {
		if (!Strings::compare(node, commandInfo::login, true)) {
			DSLoginInfo logInfo(data);
			int sessionID;
			std::string description;
			std::string UMlogResString = UMLoginResult::typeString[userMngrInterface.login(logInfo.username.getValue(), logInfo.password.getValue(), sessionID,
					description)];
			DSLoginResult logResult(UMlogResString, description, sessionID);
			sm.communication.runCallback(from, logResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::logout, true)) {
			DSInteger sessionID;
			sessionID.fromString(data);
			userMngrInterface.logout(sessionID.value.getValue());
		} else if (!Strings::compare(node, commandInfo::checkpermission, true)) {
			DSChkPermission checkInfo(data);
			DSBoolean checkResult;
			checkResult.value = userMngrInterface.checkPermission(checkInfo.sessionID.getValue(), checkInfo.permissionID.getValue());
			sm.communication.runCallback(from, checkResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::addUser, true)) {
			DSAddUser adUsrInfo(data);
			DSInteger addResult;
			addResult.value = userMngrInterface.addUser(adUsrInfo.username.getValue(), adUsrInfo.password.getValue(), adUsrInfo.name.getValue());
		} else if (!Strings::compare(node, commandInfo::modifyUser, true)) {
			DSModifyUser userInfo(data);
			userMngrInterface.modifyUser(userInfo.userID.getValue(), userInfo.username.getValue(), userInfo.password.getValue(), userInfo.name.getValue());
		} else if (!Strings::compare(node, commandInfo::removeUser, true)) {
			DSInteger userID;
			userID.fromString(data);
			userMngrInterface.removeUser(userID.value.getValue());
		} else if (!Strings::compare(node, commandInfo::getUserInfo, true)) {
			DSInteger userId;
			userId.fromString(data);
			UMUserInfo temp = userMngrInterface.getUserInfo(userId.value.getValue());
			DSUserInfo userInfo(temp.id, temp.username, temp.name, temp.banned, temp.banReason);
			sm.communication.runCallback(from, userInfo.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::registerPermission, true)) {
			DSRegPermission permissionInfo(data);
			DSInteger regResult;
			regResult.value = userMngrInterface.registerPermission(permissionInfo.name.value(), permissionInfo.title.getValue(),
					permissionInfo.description.getValue(), permissionInfo.parentID.getValue());
			sm.communication.runCallback(from, regResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::updatePermission, true)) {
			DSUpdatePermission updateInfo(data);
			userMngrInterface.updatePermission(updateInfo.permissiosnID.getValue(), updateInfo.name.getValue(), updateInfo.title.getValue(),
					updateInfo.description.getValue(), updateInfo.parentID.getValue());
		} else if (!Strings::compare(node, commandInfo::removePermission, true)) {
			DSInteger removeID;
			removeID.fromString(data);
			userMngrInterface.removePermission(removeID.value.getValue());
		} else if (!Strings::compare(node, commandInfo::registerUsergroup, true)) {
			DSRegUsrGrp usrGrpInfo(data);
			DSInteger regResult;
			regResult.value = userMngrInterface.registerUsergroup(usrGrpInfo.title.getValue(), usrGrpInfo.parentID.getValue(),
					usrGrpInfo.description.getValue());
			sm.communication.runCallback(from, regResult.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::updateUsergroup, true)) {
			DSUpdateUsrGrp usrGrpInfo(data);
			userMngrInterface.updateUsergroup(usrGrpInfo.usergroupID.getValue(), usrGrpInfo.title.getValue(), usrGrpInfo.parentID.getValue(),
					usrGrpInfo.description.getValue());
		} else if (!Strings::compare(node, commandInfo::removeUsergroup, true)) {
			DSInteger usrGrpID;
			usrGrpID.fromString(data);
			userMngrInterface.removeUsergroup(usrGrpID.value.getValue());
		} else if (!Strings::compare(node, commandInfo::listUsers, true)) {
			DSUserList usersList(userMngrInterface.listUsers());
			sm.communication.runCallback(from, usersList.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::listUsersByGroup, true)) {
			DSInteger groupID;
			groupID.fromString(data);
			DSUserList usrListByGrp(userMngrInterface.listUsersByGroup(groupID.value.getValue()));
			sm.communication.runCallback(from, usrListByGrp.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::listPermissions, true)) {
			DSPermissionsList permsList(userMngrInterface.listPermissions());
			sm.communication.runCallback(from, permsList.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::listUsergroups, true)) {
			DSUserGroupsList usrGrpList(userMngrInterface.listUsergroups());
			sm.communication.runCallback(from, usrGrpList.toString(true), id);
		} else if (!Strings::compare(node, commandInfo::addUserUsergroup, true)) {
			zeitoon::usermanagement::DSUserUsergroup regInfo(data);
			userMngrInterface.addUserUsergroup(regInfo.userID.getValue(), regInfo.groupID.getValue());

		} else if (!Strings::compare(node, commandInfo::removeUserUsergroup, true)) {
			zeitoon::usermanagement::DSUserUsergroup regInfo(data);
			userMngrInterface.removeUserUsergroup(regInfo.userID.getValue(), regInfo.groupID.getValue());

		} else if (!Strings::compare(node, commandInfo::addUserPermission, true)) {
			zeitoon::usermanagement::DSUserPermission regInfo(data);
			userMngrInterface.addUserPermission(regInfo.userID.getValue(), regInfo.permissionID.getValue(), regInfo.permissionState.getValue());
		} else if (!Strings::compare(node, commandInfo::removeUserPermission, true)) {
			zeitoon::usermanagement::DSUserPermission regInfo(data);
			userMngrInterface.removeUserPermission(regInfo.userID.getValue(), regInfo.permissionID.getValue(), regInfo.permissionState.getValue());
		}
	} catch (exceptionEx *errorInfo) {
		sm.communication.errorReport(node, id, errorInfo->what());
		userMngrInterface.systemLog.log("node: " + node + " id:" + id + " errMsg:" + errorInfo->what());
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
	this->serviceID = id; //keeps serviceID in a member variable
	std::ofstream outFile;
	outFile.open("UmCHIConfig", std::ofstream::out);
	outFile << "serviceID : " + id << std::endl;
	outFile.close();
	//Finding executable path to db scripts
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
	try {
		userMngrInterface.executeSync(str);
	} catch (exceptionEx * errorInfo) {

	}
}

void UmCHI::onEnable() {
	std::string temp = "";
	size_t length = insInfo.commands.length();
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.commands[i]->name.toString();
	}
	sm.communication.registerCommand(temp);
std::cout <<temp<<endl;
	length = insInfo.events.length();
	temp ="";
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.events[i]->name.toString();
	}
	sm.communication.registerEvent(temp);
	std::cout <<temp<<endl;
}

void UmCHI::onDisable() {
}

void UmCHI::onUninstall() {
	std::fstream inFile("UmCHIConfig");
	std::string line;
	std::deque<std::string> deqTemp;
	if (not inFile) {
		EXTinvalidName("unable to find or open requested file");
	}
	while (std::getline(inFile, line)) {
		deqTemp.push_back(line);
	}

	for (std::deque<std::string>::iterator iter = deqTemp.begin(); iter != deqTemp.end(); iter++) {
		if (std::string(*iter).find(("serviceID")) != std::string::npos) {
			deqTemp.erase(iter);
			break;
		}
	}
	std::ofstream outFile;
	outFile.open("UmCHIConfig", std::ofstream::trunc);
	for (std::deque<std::string>::iterator iter = deqTemp.begin(); iter != deqTemp.end(); iter++) {
		outFile << *iter << std::endl;
	}
	outFile.close();
}

void UmCHI::onConnect() {

}

void UmCHI::onDisconnect() {

}

string UmCHI::getInstallInfo() {
	return insInfo.toString(true);
}

string UmCHI::getInstallID() {
	if (serviceID == "") {
		std::fstream inFile("UmCHIConfig");
		std::string line;
		if (not inFile) {
			return "";
		}
		while (std::getline(inFile, line)) {
			std::string::size_type tempServiceID = line.find("serviceID = ");
			if (tempServiceID != std::string::npos) {
				if (line.find("#") < tempServiceID)
					break;
				serviceID = line.substr(line.find(" =") + 3);
				return serviceID;
			}
		}
	}
	return serviceID;
}

string UmCHI::getServiceName() {
	return "UserManagement";
}

std::size_t UmCHI::getServiceVersion() {
	return 1;
}

string UmCHI::changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion, int& newVersion) {
	return "";
}

void UmCHI::onError(string node, string id, string description) {
	std::cerr << "Error";
}

void UmCHI::onWarning(string level, string node, string id, string description) {
	std::cerr << "Warning";
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
			"{\"type\" : \"internal\" , \"node\" : \"ping\" , \"name\" : \"" + getServiceName() + "\" , \"version\" : \"5\""
					+ (id.length() > 0 ? " , \"id\" : \"" + getInstallID() + "\"" : "") + "}");
}
void UmCHI::setInstallInfo() {

//---------set available commands info

	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::login, DSLoginInfo::getStructName(), DSLoginInfo::getStructVersion(),
					DSLoginResult::getStructName(), DSLoginResult::getStructVersion()), true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::logout, DSInteger::getStructName(), DSInteger::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::checkpermission, DSChkPermission::getStructName(), DSChkPermission::getStructVersion(),
					DSBoolean::getStructName(), DSBoolean::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::addUser, DSAddUser::getStructName(), DSAddUser::getStructVersion(), DSInteger::getStructName(),
					DSInteger::getStructVersion()), true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::modifyUser, DSModifyUser::getStructName(), DSModifyUser::getStructVersion(), "", 0),
	true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::removeUser, DSInteger::getStructName(), DSInteger::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::getUserInfo, DSInteger::getStructName(), DSInteger::getStructVersion(), DSUserInfo::getStructName(),
					DSUserInfo::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::registerPermission, DSRegPermission::getStructName(), DSRegPermission::getStructVersion(),
					DSInteger::getStructName(), DSInteger::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::updatePermission, DSUpdatePermission::getStructName(), DSUpdatePermission::getStructVersion(), "",
					0), true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::removePermission, DSInteger::getStructName(), DSInteger::getStructVersion(), "", 0),
	true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::registerUsergroup, DSRegUsrGrp::getStructName(), DSRegUsrGrp::getStructVersion(),
					DSInteger::getStructName(), DSInteger::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::updateUsergroup, DSUpdateUsrGrp::getStructName(), DSUpdateUsrGrp::getStructVersion(), "", 0), true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::removeUsergroup, DSInteger::getStructName(), DSInteger::getStructVersion(), "", 0),
	true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::listUsers, "", 0, DSUserList::getStructName(), DSUserList::getStructVersion()), true);
	insInfo.commands.add(new DSInstallInfo::DSCommandDetail(commandInfo::listUsersByGroup, "", 0, DSUserList::getStructName(), DSUserList::getStructVersion()),
	true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::listPermissions, "", 0, DSPermissionsList::getStructName(), DSPermissionsList::getStructVersion()),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::listUsergroups, DSUserGroupsList::getStructName(), DSUserGroupsList::getStructVersion(), "", 0),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::addUserUsergroup, DSUserUsergroup::getStructName(), DSUserUsergroup::getStructVersion(), "", 0),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::removeUserUsergroup, DSUserUsergroup::getStructName(), DSUserUsergroup::getStructVersion(), "", 0),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::addUserPermission, DSUserPermission::getStructName(), DSUserPermission::getStructVersion(), "", 0),
			true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::removeUserPermission, DSUserPermission::getStructName(), DSUserPermission::getStructVersion(), "",
					0), true);

//--------set available events info

	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::loggedIn, DSUserInfo::getStructName(), DSUserInfo::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::loggedOut, DSUserInfo::getStructName(), DSUserInfo::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::userAdded, DSUserInfo::getStructName(), DSUserInfo::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::userModified, DSUserInfo::getStructName(), DSUserInfo::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::userRemoved, DSUserInfo::getStructName(), DSUserInfo::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::permissionAdded, DSUpdatePermission::getStructName(), DSUpdatePermission::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::permissionModified, DSUpdatePermission::getStructName(), DSUpdatePermission::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::permissionRemoved, DSUpdatePermission::getStructName(), DSUpdatePermission::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::usergroupAdded, DSUpdateUsrGrp::getStructName(), DSUpdateUsrGrp::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::usergroupModified, DSUpdateUsrGrp::getStructName(), DSUpdateUsrGrp::getStructVersion()),
	true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::usergroupRemoved, DSUpdateUsrGrp::getStructName(), DSUpdateUsrGrp::getStructVersion()),
	true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usersUsergroupRemoved, DSUserUsergroup::getStructName(), DSUserUsergroup::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::usersUsergroupAdded, DSUserUsergroup::getStructName(), DSUserUsergroup::getStructVersion()),
	true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usersPermissionRemoved, DSUserPermission::getStructName(), DSUserPermission::getStructVersion()), true);
	insInfo.events.add(
			new DSInstallInfo::DSEventDetail(eventInfo::usersPermissionAdded, DSUserPermission::getStructName(), DSUserPermission::getStructVersion()), true);

//-------set calls

	insInfo.calls.add(
			new DSInstallInfo::DSInstallInfoCommandCallDetail(zeitoon::pgdatabase::commandInfo::query, zeitoon::pgdatabase::DSDBQuery::getStructName(),
					zeitoon::pgdatabase::DSDBQuery::getStructVersion(), zeitoon::pgdatabase::DSDBTable::getStructName(),
					zeitoon::pgdatabase::DSDBTable::getStructVersion()), true);
	insInfo.calls.add(
			new DSInstallInfo::DSInstallInfoCommandCallDetail(zeitoon::pgdatabase::commandInfo::execute, zeitoon::pgdatabase::DSDBQuery::getStructName(),
					zeitoon::pgdatabase::DSDBQuery::getStructVersion(), zeitoon::datatypes::DSInteger::getStructName(),
					zeitoon::datatypes::DSInteger::getStructVersion()), true);
	insInfo.calls.add(
			new DSInstallInfo::DSInstallInfoCommandCallDetail(zeitoon::pgdatabase::commandInfo::singlefieldquery, zeitoon::pgdatabase::DSDBQuery::getStructName(),
					zeitoon::pgdatabase::DSDBQuery::getStructVersion(), zeitoon::datatypes::DSString::getStructName(),
					zeitoon::datatypes::DSString::getStructVersion()), true);

//------set requirements

	insInfo.requirements.add(new DSInstallInfo::DSInstallInfoRequirementDetail("PGdatabase", 1), true);

//------set datatype names

	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSLoginInfo::getStructName(),
					zeitoon::usermanagement::DSLoginInfo::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSLoginResult::getStructName(),
					zeitoon::usermanagement::DSLoginResult::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSChkPermission::getStructName(),
					zeitoon::usermanagement::DSChkPermission::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSAddUser::getStructName(),
					zeitoon::usermanagement::DSAddUser::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSModifyUser::getStructName(),
					zeitoon::usermanagement::DSModifyUser::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserInfo::getStructName(),
					zeitoon::usermanagement::DSUserInfo::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSRegPermission::getStructName(),
					zeitoon::usermanagement::DSRegPermission::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUpdatePermission::getStructName(),
					zeitoon::usermanagement::DSUpdatePermission::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSRegUsrGrp::getStructName(),
					zeitoon::usermanagement::DSRegUsrGrp::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUpdateUsrGrp::getStructName(),
					zeitoon::usermanagement::DSUpdateUsrGrp::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserList::getStructName(),
					zeitoon::usermanagement::DSUserList::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSPermissionsList::getStructName(),
					zeitoon::usermanagement::DSPermissionsList::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserGroupsList::getStructName(),
					zeitoon::usermanagement::DSUserGroupsList::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserUsergroup::getStructName(),
					zeitoon::usermanagement::DSUserUsergroup::getStructVersion()), true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(zeitoon::usermanagement::DSUserPermission::getStructName(),
					zeitoon::usermanagement::DSUserPermission::getStructVersion()), true);

}
} //usermanagement
} //zeitoon

