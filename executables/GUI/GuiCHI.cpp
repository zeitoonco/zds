#include "GuiCHI.hpp"
#include "GuiUtility.hpp"
#include "DTStructs.hpp"
#include "utility/DTStructs.hpp"
#include <fstream>

using namespace zeitoon::datatypes;
namespace zeitoon {
namespace GUI {

void GuiCHI::onCommand(string node, string data, string id, string from) {
	try {
		if (!Strings::compare(node, commandInfo::getList(), false)) {
			DSString result(guiCore.getList(DSString(data, true).value.getValue()), false);
			sm.communication.runCallback(from, result.toString(true), id);
		}
		if (!Strings::compare(node, commandInfo::getListOfLists(), false)) {
			//NOT TESTED
			DSListOfLists result(zeitoon::datatypes::DatatypesUtility::toDTStringList(guiCore.getListOfLists()));
			sm.communication.runCallback(from, result.toString(true), id);
		}
		if (!Strings::compare(node, commandInfo::registerList(), false)) {
			DSRegList regInfo(data);
			guiCore.registerList(regInfo.name.getValue(), regInfo.content.getValue());
		}
		if (!Strings::compare(node, commandInfo::updateList(), false)) {
			DSRegList regInfo(data);
			guiCore.updateList(regInfo.name.getValue(), regInfo.content.getValue());
		}
		if (!Strings::compare(node, commandInfo::removeList(), false)) {
			guiCore.removeList(DSString(data, true).value.getValue());
		}
		if (!Strings::compare(node, commandInfo::getTemplate(), false)) {
			DSTemplateInfo tempInfo(data);
			DSString result(guiCore.getTemplate(tempInfo.templateName.getValue(), tempInfo.fileName.getValue()), false);
			sm.communication.runCallback(from, result.toString(true), id);
		}
		if (!Strings::compare(node, commandInfo::registerTemplate(), false)) {
			DSRegTemplate regTemp(data);
			guiCore.registerTemplate(regTemp.templateName.getValue(), regTemp.fileName.getValue(),
			                         regTemp.content.getValue());
		}
		if (!Strings::compare(node, commandInfo::updateTemplate(), false)) {
			DSRegTemplate regTemp(data);
			guiCore.updateTemplate(regTemp.templateName.getValue(), regTemp.fileName.getValue(),
			                       regTemp.content.getValue());
		}
		if (!Strings::compare(node, commandInfo::removeTemplate(), false)) {
			DSTemplateInfo tempInfo(data);
			guiCore.removeTemplate(tempInfo.templateName.getValue(), tempInfo.fileName.getValue());
		}
		if (!Strings::compare(node, commandInfo::getTemplateFilesList(), false)) {
			//NOT TESTED
			DSTemplateFilesList result(zeitoon::datatypes::DatatypesUtility::toDTStringList(
					guiCore.getTemplateFilesList(DSString(data, true).value.getValue())));
			sm.communication.runCallback(from, result.toString(true), id);

		}
		if (!Strings::compare(node, commandInfo::getTemplatesList(), false)) {
			//NOT TESTED
			DSTemplatesList result(zeitoon::datatypes::DatatypesUtility::toDTStringList(guiCore.getTemplatesList()));
			sm.communication.runCallback(from, result.toString(true), id);

		}

	} catch (exceptionEx *errorInfo) {
		sm.communication.errorReport(node, id, errorInfo->what());
	} catch (exception &errorInfo) {
		sm.communication.errorReport(node, id, errorInfo.what());
	}
}


void GuiCHI::onCallback(string node, string data, string id, string from) {

}

void GuiCHI::onEvent(string node, string data, string from) {
	guiCore.eventReceived(node, data);
}

void GuiCHI::onInstall(string id) {
	this->serviceID = id; //keeps serviceID in a member variable
	string cpath = FileSystemUtility::getAppPath();
	std::ofstream outFile;
	outFile.open(cpath + "GuiCHIConfig", std::ofstream::out);
	outFile << "serviceID : " + id << std::endl;
	outFile.close();
}

void GuiCHI::onEnable() {
	std::string temp = "";
	size_t length = insInfo.commands.length();
	for (size_t i = 0; i < length; i++) {
		temp += (i == 0 ? "" : ",") + insInfo.commands[i]->name.toString();
	}
	sm.communication.registerCommand(temp);
}

void GuiCHI::onDisable() {

}

void GuiCHI::onUninstall() {
	string cpath = FileSystemUtility::getAppPath();
	std::fstream inFile(cpath + "GuiCHIConfig");
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
	outFile.open(cpath + "GuiCHIConfig", std::ofstream::trunc);
	for (std::deque<std::string>::iterator iter = deqTemp.begin(); iter != deqTemp.end(); iter++) {
		outFile << *iter << std::endl;
	}
	outFile.close();
}

void GuiCHI::onConnect() {
	std::cerr << "\n+GUI Connected to server\n";
}

void GuiCHI::onDisconnect() {
	std::cerr << "\n-GUI Disconnected from server\n";
}

string GuiCHI::getInstallInfo() {
	return insInfo.toString(true);
}

string GuiCHI::getInstallID() {
	string cpath = FileSystemUtility::getAppPath();
	std::fstream inFile(cpath + "GuiCHIConfig");
	std::string line;
	if (not inFile) {
		return "";
	}
	while (std::getline(inFile, line)) { //if line[0]
		std::string::size_type tempServiceID = line.find("serviceID = ");
		if (tempServiceID != std::string::npos) {
			if (line.find("#") < tempServiceID)
				break;
			serviceID = line.substr(line.find(" =") + 3);
			return serviceID;
		}
	}
	return serviceID;
}

string GuiCHI::getServiceName() {
	return "GUI";
}

size_t GuiCHI::getServiceVersion() {
	return 1;
}

string GuiCHI::changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion,
                                     int &newVersion) {
	return "";
}

void GuiCHI::onError(string node, string id, string description) {
	std::cerr << "Error:\t" << description << std::endl;
}

void GuiCHI::onWarning(string level, string node, string id, string description) {
	std::cerr << "Warning:\t" << description << std::endl;
}

void GuiCHI::pong(string id, int miliseconds) {
	sm.send(
			"{\"type\" : \"internal\" , \"node\" : \"ping\" , \"name\" : \"" + getServiceName() +
			"\" , \"version\" : \"1\""
			+ (id.length() > 0 ? " , \"id\" : \"" + getInstallID() + "\"" : "") + "}");
}

void GuiCHI::setInstallInfo() {

	//---------set available commands info

	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::getList(), DSString::getStructName(),
			                                   DSString::getStructVersion(), DSString::getStructName(),
			                                   DSString::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::getListOfLists(), "", 0, DSListOfLists::getStructName(),
			                                   DSListOfLists::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::registerList(), DSRegList::getStructName(),
			                                   DSRegList::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::updateList(), DSRegList::getStructName(),
			                                   DSRegList::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::removeList(), DSString::getStructName(),
			                                   DSString::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::getTemplate(), DSTemplateInfo::getStructName(),
			                                   DSTemplateInfo::getStructVersion(), DSString::getStructName(),
			                                   DSString::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::registerTemplate(), DSRegTemplate::getStructName(),
			                                   DSRegTemplate::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::updateTemplate(), DSRegTemplate::getStructName(),
			                                   DSRegTemplate::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::removeTemplate(), DSTemplateInfo::getStructName(),
			                                   DSTemplateInfo::getStructVersion(), "", 0), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::getTemplateFilesList(), DSString::getStructName(),
			                                   DSString::getStructVersion(), DSTemplateFilesList::getStructName(),
			                                   DSTemplateFilesList::getStructVersion()), true);
	insInfo.commands.add(
			new DSInstallInfo::DSCommandDetail(commandInfo::getTemplatesList(), "", 0, DSTemplatesList::getStructName(),
			                                   DSTemplatesList::getStructVersion()), true);

	//------set datatype names

	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(DSListOfLists::getStructName(),
			                                                DSListOfLists::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(DSRegList::getStructName(), DSRegList::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(DSTemplateInfo::getStructName(),
			                                                DSTemplateInfo::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(DSRegTemplate::getStructName(),
			                                                DSRegTemplate::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(DSTemplateFilesList::getStructName(),
			                                                DSTemplateFilesList::getStructVersion()),
			true);
	insInfo.datatypes.add(
			new DSInstallInfo::DSInstallInfoDatatypesDetail(DSTemplatesList::getStructName(),
			                                                DSTemplatesList::getStructVersion()),
			true);

	//--------set available events info

	//--------set available Hooks info
	insInfo.hooks.add(new DSInstallInfo::DSHookDetail("ALL"), true);


	//-------set calls
	insInfo.calls.add(
			new DSInstallInfo::DSInstallInfoCommandCallDetail("ALL", "", 0, "", 0), true);

	//------set requirements



}
}
}