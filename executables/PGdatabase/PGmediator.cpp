/*
 * pgmediator.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: inf
 */
#include <executables/PGdatabase/PGmediator.hpp>
#include "executables/PGdatabase/DTStructs.hpp"
#include "executables/PGdatabase/pgutility.hpp"
#include "PGConfig.hpp"
#include <fstream>
#include "executables/_core/coreutility.hpp"
#include "utility/logger.hpp"

#include <deque>

namespace zeitoon {
namespace pgdatabase {

PGmediator::PGmediator(std::string pgAdminUserName, std::string pgAdminPassWord,
                       std::string pgAdminHost, int pgAdminPort, std::string pgAdminDbname) :
		CommunicationHandlerInterface(this),
		conMgr(pgAdminUserName, pgAdminPassWord, pgAdminHost, pgAdminPort, pgAdminDbname, this),
		insInfo("PGDatabase", "PostgresDatabase", 1, 1, EnmServiceType::Database) {
	this->setInstallInfo();
}

bool PGmediator::onCommand(string node, string data, string id, string from, std::string &resultStr) {
	try {
		if (!Strings::compare(node, zeitoon::pgdatabase::commandInfo::query(), false)) {
			DSDBQuery sql(data, true);
			DTTableString result(conMgr.query(from, sql.query.getValue()).toString(), "");
			resultStr = result.toString(true);
		} else if (!Strings::compare(node, zeitoon::pgdatabase::commandInfo::execute(), false)) {
			DSDBQuery sql(data, true);
			DSInteger result;
			result.value = conMgr.execute(from, sql.query.getValue());
			resultStr = result.toString(true);
		} else if (!Strings::compare(node, zeitoon::pgdatabase::commandInfo::singlefieldquery(), false)) {
			DSDBQuery sql(data, true);
			DSString result(conMgr.singleFieldQuery(from, sql.query.getValue()), false);
			resultStr = result.toString(true);
		}
	} catch (exceptionEx &errorInfo) {
		lError("node: " + node + " id:" + id + " errMsg:" + errorInfo.what());
		sm.communication.errorReport(node, id, errorInfo.what());
		return false;
	} catch (exception &errorInfo) {
		lError("node: " + node + " id:" + id + " errMsg:" + errorInfo.what());
		sm.communication.errorReport(node, id, errorInfo.what());
		return false;
	}
	return true;
}

void PGmediator::onCallback(string node, string data, string id, string from,  std::string success) {
}

void PGmediator::onEvent(string node, string data, string from) {
	try {
		JStruct temp(data);
		if (!Strings::compare(node, zeitoon::_core::eventInfo::onServiceUninstall(), false)) {
			std::string d = temp["name"].getValue();
			conMgr.removeExtension(d);
		}
	} catch (zeitoon::utility::exceptionEx &err) {
		lError("onEvent err:  node: " + node + " from: " + from + " errMsg:" + err.what());
	}
}

void PGmediator::onInstall(string id) {
	PGconfiguration.serviceID = id;
	PGconfiguration.save();
	lNote("Service installed");
}

void PGmediator::onEnable() {
//--------register cmds
	std::string temp = "";
	size_t length = insInfo.commands.length();
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.commands[i]->name.toString();
	}
	sm.communication.registerCommand(temp);
	lNote("Commands list: "+temp);

	//--------register events
	length = insInfo.events.length();
	temp = "";
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.events[i]->name.toString();
	}
	sm.communication.registerEvent(temp);
	lNote("Events list: "+temp);

	//--------register hooks
	length = insInfo.hooks.length();
	temp = "";
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.hooks[i]->name.toString();
	}
	sm.communication.registerHook(temp);
	lNote("Hooks list: "+temp);
	lNote("Service enabled");
}

void PGmediator::onDisable() {
	lWarnig("Service disabled");
}

void PGmediator::onUninstall() { //fixme: remove db things
	PGconfiguration.serviceID = "";
	PGconfiguration.save();
	lWarnig("Service Uninstalled");

}

void PGmediator::onConnect() {
	lNote("+PG Connected to server");
}

void PGmediator::onDisconnect() {
	lNote("-PG Disconnected from server");
}

string PGmediator::getInstallInfo() {
	return insInfo.toString(true);
}

string PGmediator::getInstallID() {
	return PGconfiguration.serviceID.getValue();
}

string PGmediator::getServiceName() {
	return "PGDatabase";
}

size_t PGmediator::getServiceVersion() {
	return 1;
}

string PGmediator::changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion,
                                         int &newVersion) {
	return "";
}

void PGmediator::onError(string node, string id, string description) {
	lError("Error from: "+node+"OperationID: "+id+" desc: "+description);
}

void PGmediator::onWarning(string level, string node, string id, string description) {
	lWarnig("Warning from: "+node+"OperationID: "+id+" desc: "+description);
}

void PGmediator::pong(string id, int miliseconds) {
	sm.send(
			"{\"type\" : \"internal\" , \"node\" : \"ping\" , \"name\" : \"" + getServiceName() +
			"\" , \"version\" : \"1\""
			+ (id.length() > 0 ? " , \"id\" : \"" + getInstallID() + "\"" : "") + "}");
}

void PGmediator::setInstallInfo() {

	//---------set available commands info

	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::query(), DSDBQuery::getStructName(),
			                                   DSDBQuery::getStructVersion(), DSDBTable::getStructName(),
			                                   DSDBTable::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::execute(), DSDBQuery::getStructName(),
			                                   DSDBQuery::getStructVersion(), DSInteger::getStructName(),
			                                   DSInteger::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::singlefieldquery(), DSDBQuery::getStructName(),
			                                   DSDBQuery::getStructVersion(),
			                                   DSString::getStructName(), DSString::getStructVersion()), true);

	//--------set available events info

	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::newUser(), DSString::getStructName(),
	                                                    DSString::getStructVersion()), true);
	insInfo.events.add(new DSInstallInfo::DSEventDetail(eventInfo::userLogin(), DSString::getStructName(),
	                                                    DSString::getStructVersion()), true);

	//------set datatype names

	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(DSDBQuery::getStructName(), DSDBQuery::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(DSDBTable::getStructName(), DSDBTable::getStructVersion()),
			true);
	///------------set available hooks

	insInfo.hooks.add(new DSInstallInfo::DSHookDetail(zeitoon::_core::eventInfo::onServiceUninstall(), "", 0), true);
	insInfo.hooks.add(new DSInstallInfo::DSHookDetail(zeitoon::_core::eventInfo::onServiceDisable(), "", 0), true);

}
}
}
