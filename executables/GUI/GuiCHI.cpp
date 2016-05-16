#include "GuiCHI.hpp"
#include "GuiUtility.hpp"
#include "DTStructs.hpp"
#include "utility/DTStructs.hpp"
#include <fstream>

using namespace zeitoon::datatypes;
namespace zeitoon {
namespace GUI {

void GuiCHI::onCommand(string node, string data, string id, string from) {
	sm.communication.errorReport(node, id, "there's no such command");
}


void GuiCHI::onCallback(string node, string data, string id, string from) {
	cerr << "\nCBRCHI " << id << " " << data;
	guiCore.callBackReceived(node, id, data);
}

void GuiCHI::onEvent(string node, string data, string from) {
	guiCore.eventReceived(node, data);
}

void GuiCHI::onInstall(string id) {
	GUIConfiguration.serviceID = id;
	GUIConfiguration.save();
	//todo:Config webserver, FTP server
}

void GuiCHI::onEnable() {
	//todo:Config webserver, FTP server
}

void GuiCHI::onDisable() {

}

void GuiCHI::onUninstall() {
	GUIConfiguration.serviceID = "";
	GUIConfiguration.save();
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
	return GUIConfiguration.serviceID.getValue();
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
	guiCore.errorReceived(node, id, description);
}

void GuiCHI::onWarning(string level, string node, string id, string description) {
	std::cerr << "Warning:\t" << description << std::endl;
}

void GuiCHI::pong(string id, int miliseconds) {
	sm.send(
			"{\"type\" : \"internal\" , \"node\" : \"ping\" , \"name\" : \"" + getServiceName() +
			"\" , \"version\" : \"1\""
			+ (id.length() > 0 ? " , \"id\" : \"" + getInstallID() + "\"" : "") + "}");//todo:@NAVIDI wtf?
}

void GuiCHI::setInstallInfo() {
	//--------set available Hooks info
	insInfo.hooks.add(new DSInstallInfo::DSHookDetail("ALL"), true);
	//-------set calls
	insInfo.calls.add(new DSInstallInfo::DSInstallInfoCommandCallDetail("ALL", "", 0, "", 0), true);

	insInfo.installRequirements.add(new DSInstallInfo::DSInstallInfoRequirementDetail("UserManagement", 1), true);
	insInfo.enableRequirements.add(new DSInstallInfo::DSInstallInfoRequirementDetail("UserManagement", 1), true);

}

}
}