#include "GuiCHI.hpp"
#include "GuiUtility.hpp"
#include "DTStructs.hpp"
#include "utility/DTStructs.hpp"
#include <fstream>

using namespace zeitoon::datatypes;
namespace zeitoon {
namespace GUI {

bool GuiCHI::onCommand(string node, string data, string id, string from, std::string &resultStr) {
	sm.communication.errorReport(node, id, "there's no such command");
	lError("onCommand called(GUI ONCMD HAS NO COMMAND) From: " + from + " To: " + node + " ID:" + id +
	       " Data: " + data);
}


void GuiCHI::onCallback(string node, string data, string id, string from, std::string success) {

	lDebug("Callback ID:" + id + " Node: " + node + " received from: " + from + ". Data: " + data + " success: " +
	       success);
	guiCore.callBackReceived(node, id, data,success);
}

void GuiCHI::onEvent(string node, string data, string from) {
	lDebug("evenReceived. From: " + from + " Node:" + node + " Data: " + data);
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
	lWarnig("Service disabled");
}

void GuiCHI::onUninstall() {
	GUIConfiguration.serviceID = "";
	GUIConfiguration.save();
}

void GuiCHI::onConnect() {
	lNote("+GUI Connected to server");
}

void GuiCHI::onDisconnect() {
	lNote("-GUI Disconnected from server");
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
	lError("Warning from: " + node + "OperationID: " + id + " desc: " + description);
	guiCore.errorReceived(node, id, description);
}

void GuiCHI::onWarning(string level, string node, string id, string description) {
	lError("Warning from: " + node + "OperationID: " + id + " desc: " + description);

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

	insInfo.installRequirements.add(new DSInstallInfo::DSInstallInfoRequirementDetail("UserManagement", 1),
	                                true);
	insInfo.enableRequirements.add(new DSInstallInfo::DSInstallInfoRequirementDetail("UserManagement", 1),
	                               true);

}

}
}