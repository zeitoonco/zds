/*
 * pgmediator.cpp
 *
 *  Created on: Feb 14, 2016
 *      Author: inf
 */
#include <executables/PGdatabase/PGmediator.hpp>
#include "executables/PGdatabase/DTStructs.hpp"
#include "executables/PGdatabase/pgutility.hpp"
#include "utility/exceptionex.hpp"
#include <fstream>
#include <deque>

namespace zeitoon {
namespace pgdatabase {

PGmediator::PGmediator(std::string pgAdminUserName, std::string pgAdminPassWord,
                       std::string pgAdminHost, int pgAdminPort, std::string pgAdminDbname) :
		CommunicationHandlerInterface(this),
		conMgr(pgAdminUserName, pgAdminPassWord, pgAdminHost, pgAdminPort, pgAdminDbname, this),
		insInfo("PGDatabase", "PostgresDatabase", 1) {
	this->setInstallInfo();
	serviceID = "";
}

void PGmediator::onCommand(string node, string data, string id, string from) {
	try {
		if (!Strings::compare(node, zeitoon::pgdatabase::commandInfo::query(), false)) {
			this->sm.communication.runCallback(from, conMgr.query(from, data).toString(), id);
			this->sm.communication.runCallback(from, conMgr.query(from, data).toString(), id);
		} else if (!Strings::compare(node, zeitoon::pgdatabase::commandInfo::execute(), false)) {
			this->sm.communication.runCallback(from, std::to_string(conMgr.execute(from, data)), id);
		} else if (!Strings::compare(node, zeitoon::pgdatabase::commandInfo::singlefieldquery(), false)) {
			this->sm.communication.runCallback(from, conMgr.singleFieldQuery(from, data), id);
		}
	} catch (exceptionEx *errorInfo) {
		sm.communication.errorReport(node, id, errorInfo->what());
	} catch (exception &errorInfo) {
		sm.communication.errorReport(node, id, errorInfo.what());
	}
}

void PGmediator::onCallback(string node, string data, string id, string from) {
}

void PGmediator::onEvent(string node, string data, string from) {
}

void PGmediator::onInstall(string id) {//todo:didnt save InstallID to file
	string cpath = FileSystemUtility::getAppPath();
	this->serviceID = id;
	std::ofstream outFile;
	outFile.open(cpath + "pgMediatorConfig", std::ofstream::out);
	outFile << "serviceID : " + id << std::endl;
	outFile.close();
}

void PGmediator::onEnable() {
	std::string temp = "";
	size_t length = insInfo.commands.length();
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.commands[i]->name.toString();
	}
	sm.communication.registerCommand(temp);

	length = insInfo.events.length();
	temp.clear();
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.events[i]->name.toString();
	}
	sm.communication.registerEvent(temp);
}

void PGmediator::onDisable() {
	cerr<<"\nDisable!";
}

void PGmediator::onUninstall() { //remove Id from the configuration file
	string cpath = FileSystemUtility::getAppPath();
	std::fstream inFile(cpath + "pgMediatorConfig");
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
	outFile.open(cpath + "pgMediatorConfig", std::ofstream::trunc);
	for (std::deque<std::string>::iterator iter = deqTemp.begin(); iter != deqTemp.end(); iter++) {
		outFile << *iter << std::endl;
	}
	outFile.close();
}

void PGmediator::onConnect() {
	std::cerr << "\n+PG Connected to server\n";
}

void PGmediator::onDisconnect() {
	std::cerr << "\n-PG Disconnected from server\n";
}

string PGmediator::getInstallInfo() {
	return insInfo.toString(true);
}

string PGmediator::getInstallID() {
	if (serviceID == "") {
		string cpath = FileSystemUtility::getAppPath();
		std::fstream inFile(cpath + "pgMediatorConfig");
		std::string line;
		if (not inFile) {
			return "";
		}
		while (std::getline(inFile, line)) { //if line[0]
			std::string::size_type tempServiceID = line.find("serviceID : ");//todo: NEED FOR CONFIG MANAGER
			if (tempServiceID != std::string::npos) {
				if (line.find("#") < tempServiceID)
					break;
				serviceID = line.substr(line.find(" :") + 3);
				return serviceID;
			}
		}
	}
	return serviceID;
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
	std::cerr << "Error";
}

void PGmediator::onWarning(string level, string node, string id, string description) {
	std::cerr << "Warning";
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
}
}
}
