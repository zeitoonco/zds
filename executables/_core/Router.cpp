/*
 * Router.cpp
 *
 *  Created on: Sep 5, 2015
 *      Author: ajl
 */

#include "Router.hpp"
#include <string>
#include <mutex>
#include <thread>
#include <unordered_map>
#include "Profiles.hpp"
#include <vector>
#include "utility/utility.hpp"
#include "utility/jsonParser.hpp"
#include <functional>
#include <datatypes/DTTableString.hpp>
#include <utility/jsonParser.hpp>
#include <library/mediator/CommunicationHandlerInterface.hpp>
#include "utility/DTStructs.hpp"
#include "coreutility.hpp"

#define helloPacket "{\"type\" : \"internal\" , \"node\" : \"hello\"}"

namespace zeitoon {
namespace _core {

Router::Router(int port) :
		extManager(&db),
		comm(bind(&Router::sendMessage, this, placeholders::_1, placeholders::_2, placeholders::_3,
		          placeholders::_4,
		          placeholders::_5, placeholders::_6, placeholders::_7, placeholders::_8)),
		net(port) {
	// TODO Auto-generated constructor stub
	net.registerOnMessageCB(bind(&Router::_onDataReceive, this, std::placeholders::_1, std::placeholders::_2));
	net.registerOnClientConnectCB(bind(&Router::_onClientConnect, this, std::placeholders::_1));
	net.registerOnClientDisconnectCB(bind(&Router::_onClientDisconnect, this, std::placeholders::_1));
	db.connect(FileSystemUtility::getAppPath() + "data.db3");
	db.execute("CREATE TABLE IF NOT EXISTS \"services\"(\n"
			           "\t\"name\"      Text NOT NULL PRIMARY KEY,\n"
			           "\t\"version\"   Integer NOT NULL,\n"
			           "\t\"installid\" Text,\n"
			           "\t\"state\"     Integer NOT NULL DEFAULT 0,\n"
			           "\t\"data\"      Text,\n"
			           "\t\"CEPReg\"    Integer NOT NULL DEFAULT 0,\n"
			           "CONSTRAINT \"unique_name\" UNIQUE ( \"name\" ) );");
	try {
		net.listen();
	} catch (exceptionEx &err) {
		lError("NETWORK ERROR: " + std::string(err.what()));
	}
	extManager.load();
	registerCore();
}

Router::~Router() {
	extManager.save();
	db.disconnect();
}

void Router::_onClientConnect(size_t id) {
	net.send(id, helloPacket);
	lNote("New Client. ID: " + std::to_string(id));
}

void Router::_onClientDisconnect(size_t id) {
	ExtensionProfile *t = extManager.getByNetID(id);
	if (t != NULL) {
		string dt = "{\"name\":\"" + t->serviceInfo.name.value() + "\"}";
		comm.fireEvent(eventInfo::onServiceDisconnect(), dt, "_core");
		comm.cleanup(t);
		if (t->state <= ExtensionProfile::extensionState::installing) {
			extManager.remove(t);
			delete t;
		} else
			t->netClientId = -1;
	}
	this->disableIfReqNotSatisfied();
	//std::cerr << "\n_onClientDisconnect Unhandled situation\n";
}

void Router::_onDataReceive(size_t id, std::string data) {
	ExtensionProfile *e = extManager.getByNetID(id);//so now e might be NULL
/*
	if (e == NULL) {
		e = new ExtensionProfile;
		e->netClientId = id;
		e->name = "";
		e->version = 0;
		e->state = ExtensionProfile::extensionState::unknown;
		e->istate = ExtensionProfile::extensioniState::unknown;
		e->stateDesc = "";
		extManager.remove(id);
		extManager.add(id, e);
	}
*/
	packetReceived(data, e, id);
}

void Router::packetReceived(string data, ExtensionProfile *ext, size_t netid) {
	using zeitoon::datatypes::DTTableString;
	JStruct packet(data);
	string type, node, idata, id, sessionid;
	type = packet["type"].getValue();
	node = packet["node"].getValue();
	if (ext == NULL && (!streq(type, "internal") || !streq(node, "hello")))
		return;
	if (streq(type, "internal")) {
		if (streq(node, "ping")) {
			id = packet["node"].getValue();
			net.send(netid, "{\"type\":\"internal\",\"node\":\"pong\",\"id\":\"" + id + "\"}");
		} else if (streq(node, "pong")) {
			auto _ttl = std::chrono::system_clock::now() - ext->pingStart;
			lNote("PING " + ext->serviceInfo.name2.getValue() + ". latency: " +
			      std::to_string(chrono::duration_cast<chrono::microseconds>(_ttl).count()) + "μs.");
		} else if (streq(node, "hello")) {
			string name = packet["name"].getValue();
			if (name.length() == 0 || !Strings::isValidName(name)) {//check valid name
				net.send(netid,
				         utility::CommunicationUtility::makeError(node, "", "Invalid service name"));
				net.clients[netid]->stop();
				return;
			}
			int version = atoi(packet["version"].getValue().c_str());
			if (packet.contains("id")) {//installed. maybe!
				string iid = packet["id"].getValue();//install id
				ExtensionProfile *t1 = extManager[name];
				if (t1 != NULL && iid == t1->installID) {//yep, hes right
					if (version == t1->serviceInfo.serviceVersion.getValue()) {//right version
						ext = t1;
						ext->netClientId = netid;
						lNote("Client \" " + std::to_string(netid) + " identified as " + "' v" +
						      std::to_string(ext->serviceInfo.serviceVersion.getValue()) +
						      ". Installed with id '" + iid +
						      "'");
						string dt = "{\"name\":\"" + name + "\"}";
						comm.fireEvent(eventInfo::onServiceConnect(), dt, "_core");
						if (ext->state == ExtensionProfile::extensionState::enabled) {
							this->enableService(ext);/*
                            ext->requirementsSatisfied = extManager.isReqSatisfied(
                                    ext->serviceInfo.enableRequirements);
                            if (ext->requirementsSatisfied)

                            extManager.save();
                        }else{
                            ext->requirementsSatisfied =false;
                            ext->state = ExtensionProfile::extensionState::enabled;*/
						}
						/*try {
								enableService(t1);
							} catch (resourceNotAvailable *ex) {
								ext->requirementsSatisfied = false;
							}*/
					} else {
						net.send(netid,
						         utility::CommunicationUtility::makeError(
								         node, "", "Invalid version. Installed version: " +
								                   ext->serviceInfo.serviceVersion.getValue()));
						net.clients[netid]->stop();
					}
				} else {//wrong
					net.send(netid,
					         utility::CommunicationUtility::makeError(node, "",
					                                                  "Invalid service or service ID."));
					net.clients[netid]->stop();
				}

			} else {//not installed
				ExtensionProfile *t1 = extManager[name];
				if (t1 == NULL || t1->state == ExtensionProfile::extensionState::installing ||
				    t1->state == ExtensionProfile::extensionState::notInstalled) {
					ExtensionProfile *e = new ExtensionProfile(name, version,
					                                           ExtensionProfile::extensionState::notInstalled,
					                                           netid);
					extManager.add(e);
					lNote("Client " + std::to_string(netid) + " identified as '" + name + "' v" +
					      std::to_string(version) + ". Not installed.");
				} else {//another service with this name exist
					net.send(netid,
					         utility::CommunicationUtility::makeError(node, "",
					                                                  "A service with this name exists."));
					net.clients[netid]->stop();
				}
			}
		}
	} else {
		if (packet.contains("data"))
			idata = packet["data"].toString();
		if (packet.contains("id"))
			id = packet["id"].getValue();
		if (packet.contains("session"))
			sessionid = packet["session"].getValue();
		if (type == "callback" && streq(node, "getInstallInfo")) { //response from getInstallinfo
			lNote("Got Install Info from '" + std::to_string(netid) + ">" + ext->serviceInfo.name.getValue() +
			      ": " + data);
			ext->serviceInfo.fromString(idata);
			extManager.save();
		} else if (type == "callback" && streq(node, "onInstall")) { //response from onInstall(success confirm)
			//todo:check success field! is it really a success?
			ext->state = ExtensionProfile::extensionState::installed;
			vector<ExtensionProfile *> elist = extManager.getByServiceType(
					datatypes::EnmServiceType::UserManager);
			if (elist.size() > 0 && elist[0]->isRunning()) { //UM avail
				registerServiceCEPermissions(
						ext);//fixme: seems that it happens twise, if um is running and we install a service
				ext->CEPermissionsRegistered = true;
			} else {
				ext->CEPermissionsRegistered = false;
			}
			extManager.save();
			string dt = "{\"name\":\"" + ext->serviceInfo.name.value() + "\"}";
			comm.fireEvent(eventInfo::onServiceInstall(), dt, "_core");
		} else if (type == "callback" && streq(node, "onEnable")) {
			if (((JVariable &) packet["data"]["success"]).toBoolean()) {
				ext->state = ExtensionProfile::extensionState::enabled;
				string dt = "{\"name\":\"" + ext->serviceInfo.name.value() + "\"}";
				comm.fireEvent(eventInfo::onServiceEnable(), dt, "_core");
				this->enableIfReqSatisfied();
			} else {
				ext->state = ExtensionProfile::extensionState::installed;
			}
			if (ext->serviceInfo.serviceType.getValue() == datatypes::EnmServiceType::UserManager)
				registerCEP();
			extManager.save();
		} else {//todo: if core.req is not satisfied, stop. except locals
			try {
				if (type == "call")
					comm.callCommand(node, idata, ext->serviceInfo.name.getValue(), id, sessionid);
				else if (type == "fire")
					comm.fireEvent(node, idata, ext->serviceInfo.name.getValue());
				else if (type == "callback") {
					std::string success = packet["success"].getValue();
					comm.callCallback(id, idata, ext->serviceInfo.name.getValue(), success);
				}
			} catch (exceptionEx &ex) {
				lError("Err on Packet Received. " + std::string(ex.what()));
				string errp = CommunicationUtility::makeError(node, id, "Error occured: " + ex.toString());
				this->sendPacket(errp, ext);
			} catch (exception &ex) {
				lError("Err on Packet Received. UNKNOWN.  " + std::string(ex.what()));
				string errp = CommunicationUtility::makeError(node, id, string("Error occured: ") + ex.what());
				this->sendPacket(errp, ext);
			}
		}
	}
}

void Router::sendPacket(string &data, ExtensionProfile *extension) {
	if (extension->netClientId >= 0)
		net.send((size_t) extension->netClientId, data);
	else
		lTrace("sendPacket Unhandled situation." + extension->installID + " [DATA]: " + data);
}

void Router::sendMessage(string extension, string source, string node, string &data,
                         MessageTypes::MessageTypes_ msgT,
                         string id, string session, std::string success) {
	//check permission
	if (session.length() > 0) {
		vector<ExtensionProfile *> elist = extManager.getByServiceType(datatypes::EnmServiceType::UserManager);
		if (elist.size() > 0) {
			if (elist[0]->isRunning()) { //UM avail
				datatypes::DSBoolean res;
				string cmddata = "{\"sessionID\":" + session + ",\"name\":\"" + node + "\"}";
				res.fromString(
						comm.callCommandSync("userman.checkPermissionByName", cmddata, "_core",
						                     CommunicationUtility::getRandomID()));
				if (!res.value.getValue())
					EXTaccessDenied("Command '" + node + "' is not available for this user");
			} else {
				EXTresourceNotAvailable("UM in not running. can't check permission.");
			}
		}
	}
	if (streq(extension, "_core")) {
		switch (msgT) {
			case MessageTypes::MTCall://todo:check access here.? name->ac level id?
				callCommandLocal(node, data, source, id, session);
				break;
			case MessageTypes::MTCallback:
				callCallbackLocal(node, data, source, id);
				break;
			case MessageTypes::MTFire:
				fireHookLocal(node, data, source);
				break;
			default:
				EXTinvalidParameter("invalid MessageType '" + Strings::itoa(msgT) + "'");
		}
	} else {
		JStruct msg;
		msg.add("type", MessageTypes::toString(msgT));
		msg.add("node", node);
		msg.add("from", source);
		if (id.length() > 0)
			msg.add("id", id);
		if (session.length() > 0)
			msg.add("session", session);
		if (msgT==MessageTypes::MTCallback && success.size() > 0)
			msg.add("success", success);
		if (data.length() > 0)
			msg.addIgnored("data", data);
		string msgs = msg.toString();
		this->sendPacket(msgs, extManager[extension]);
	}
}

void Router::populateInstallInfo() {
	ecore->serviceInfo.name = "_core";
	ecore->serviceInfo.name2 = "_core";
	ecore->serviceInfo.serviceVersion = 1;
	ecore->serviceInfo.minSupportedVersion = 1;
	ecore->serviceInfo.serviceType = datatypes::EnmServiceType::Core;

	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::registerCommand(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::removeCommand(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::registerEvent(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::removeEvent(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::registerHook(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::removeHook(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::error(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::warning(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::registerSetting(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::removeSetting(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::setSetting(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::getSetting(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::updateSetting(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::resetSetting(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::getListOfServices(), "", 0, "", 0),
			true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::getServiceInfo(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::installService(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::uninstallService(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::enableService(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::disableService(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::kickService(), "", 0, "", 0), true);
	ecore->serviceInfo.commands.add(
			new datatypes::DSInstallInfo::DSCommandDetail(commandInfo::pingService(), "", 0, "", 0), true);
	ecore->serviceInfo.events.add(
			new datatypes::DSInstallInfo::DSEventDetail(eventInfo::onSettingUpdate(), "", 0), true);
	ecore->serviceInfo.events.add(
			new datatypes::DSInstallInfo::DSEventDetail(eventInfo::onServiceConnect(), "", 0), true);
	ecore->serviceInfo.events.add(
			new datatypes::DSInstallInfo::DSEventDetail(eventInfo::onServiceDisconnect(), "", 0), true);
	ecore->serviceInfo.events.add(
			new datatypes::DSInstallInfo::DSEventDetail(eventInfo::onServiceEnable(), "", 0), true);
	ecore->serviceInfo.events.add(
			new datatypes::DSInstallInfo::DSEventDetail(eventInfo::onServiceDisable(), "", 0), true);
	ecore->serviceInfo.events.add(
			new datatypes::DSInstallInfo::DSEventDetail(eventInfo::onServiceInstall(), "", 0), true);
	ecore->serviceInfo.events.add(
			new datatypes::DSInstallInfo::DSEventDetail(eventInfo::onServiceUninstall(), "", 0), true);
}

void Router::registerCore() {
	ecore = extManager["_core"];
	if (ecore == NULL) {
		ecore = new ExtensionProfile("_core", 0, ExtensionProfile::extensionState::enabled, -2);
		populateInstallInfo();
		extManager.add(ecore);
	}
	for (int i = 0; i < ecore->serviceInfo.commands.length(); i++) {
		CommandProfile c(ecore->serviceInfo.name.getValue(), ecore->serviceInfo.commands[i]->name.getValue());
		comm.registerCommand(c);
	}
	for (int i = 0; i < ecore->serviceInfo.events.length(); i++) {
		EventProfile c;
		c.name = ecore->serviceInfo.events[i]->name.getValue();
		c.extension = ecore->serviceInfo.name.getValue();
		comm.registerEvent(c);
	}
	ecore->requirementsSatisfied = checkCoreRequirements();//stop communication if not sat
}

void Router::registerCEP() {
	for (int i = 0; i < extManager.size(); i++)
		if (extManager[i]->state >= ExtensionProfile::extensionState::installed &&
		    !extManager[i]->CEPermissionsRegistered) {
			registerServiceCEPermissions(extManager[i]);
		}
	extManager.save();
}

void Router::registerServiceCEPermissions(ExtensionProfile *ext) {//WE have UM
	// PN: [serviceName].commands|events.[name]
	vector<ExtensionProfile *> elist = extManager.getByServiceType(datatypes::EnmServiceType::UserManager);
	if (elist.size() > 0 && elist[0]->isRunning()) { //UM avail
		//get parent permisssions
		string pckt = "{\"name\":\"" + ext->serviceInfo.name.getValue() + "\",\"title\":\"" +
		              ext->serviceInfo.name2.getValue() + "\",\"description\":\"Service " +
		              ext->serviceInfo.name.getValue() + " Permissions\",\"parentID\":0}";
		datatypes::DSInteger idSrvs, idc, ide;
		idSrvs.fromString(comm.callCommandSync("userman.registerPermission", pckt, "_core", "REG1"));
		//commands and events group
		pckt = "{\"name\":\"" + ext->serviceInfo.name.getValue() +
		       ".commands\",\"title\":\"commands permissions\",\"description\":\"Permissions for running commands of service\",\"parentID\":" +
		       to_string(idSrvs.value.getValue()) + "}";
		idc.fromString(comm.callCommandSync("userman.registerPermission", pckt, "_core", "REG2"));
		pckt = "{\"name\":\"" + ext->serviceInfo.name.getValue() +
		       ".events\",\"title\":\"events permissions\",\"description\":\"Permissions for hooking to events of service\",\"parentID\":" +
		       to_string(idSrvs.value.getValue()) + "}";
		ide.fromString(comm.callCommandSync("userman.registerPermission", pckt, "_core", "REG3"));
		for (int ic = 0; ic < ext->serviceInfo.commands.length(); ic++) {
			datatypes::DSInstallInfo::DSCommandDetail *c = ext->serviceInfo.commands[ic];
			pckt = "{\"name\":\"" + c->name.getValue() + "\",\"title\":\"" + c->name.getValue() +
			       "\",\"description\":\"...\",\"parentID\":" + to_string(idc.value.getValue()) + "}";
			comm.callCommandSync("userman.registerPermission", pckt, "_core", c->name.getValue());
		}
		for (int ie = 0; ie < ext->serviceInfo.events.length(); ie++) {
			datatypes::DSInstallInfo::DSEventDetail *e = ext->serviceInfo.events[ie];
			pckt = "{\"name\":\"" + e->name.getValue() + "\",\"title\":\"" + e->name.getValue() +
			       "\",\"description\":\"...\",\"parentID\":" + to_string(ide.value.getValue()) + "}";
			comm.callCommandSync("userman.registerPermission", pckt, "_core", e->name.getValue());
		}
		ext->CEPermissionsRegistered = true;
	} else {
		EXTresourceNotAvailable("UM extension is not available");
	}
}

bool Router::checkCoreRequirements() { //we need UM if we r gonna check permission of commands
	vector<ExtensionProfile *> elist = extManager.getByServiceType(datatypes::EnmServiceType::UserManager);
	if (elist.size() == 0)
		return true;
	else
		return elist[0]->isConnected() && elist[0]->state >= ExtensionProfile::extensionState::installed;
}

bool Router::enableService(ExtensionProfile *ext) {///-1
	if (!ext || ext->state < ExtensionProfile::extensionState::installed)
		return false;
	ext->requirementsSatisfied = extManager.isReqSatisfied(ext->serviceInfo.enableRequirements);
	if (!ext->requirementsSatisfied) {
		ext->requirementsSatisfied = false;
		ext->state = ExtensionProfile::extensionState::enabled;
		lWarnig("onEnable sent to " + ext->serviceInfo.name.getValue());
		return true;
	} else {
		ext->requirementsSatisfied = true;
		ext->state = ExtensionProfile::extensionState::enabling;
		string t;
		sendMessage(ext->serviceInfo.name, "_core", "onenable", t, MessageTypes::MTCall, "");
		extManager.save();
		return true;
	}
}

bool Router::installService(ExtensionProfile *ext) {
	if (!ext || ext->state >= ExtensionProfile::extensionState::installed)
		return false;
	if (!extManager.isReqSatisfied(ext->serviceInfo.installRequirements))
		EXTresourceNotAvailable("Install failed: Requirements not satisfied");
	ext->state = ExtensionProfile::extensionState::installing;
	string niid = utility::CommunicationUtility::getRandomID();
	ext->installID = niid;
	extManager.save();
	net.send(ext->netClientId,
	         utility::CommunicationUtility::makeCommand("onInstall", "ONINSTALL", "_core",
	                                                    "{\"id\":\"" + niid + "\"}"));
	//todo: timeout install phase after a while(~60 sec) (maybe we can add a "im on it" packet later, to reset timer
	return true;
}

bool Router::getInstallInfo(ExtensionProfile *ext) {
	string t;
	sendMessage(ext->serviceInfo.name, "_core", "getinstallInfo", t, MessageTypes::MTCall, "INSTALLID");
	return true;
}

bool Router::uninstallService(ExtensionProfile *ext) {
	if (!ext || ext->state < ExtensionProfile::extensionState::installed)
		return false;
	if (ext->state == ExtensionProfile::extensionState::enabled)
		disableService(ext);
	if (ext->serviceInfo.serviceType.getValue() == datatypes::EnmServiceType::UserManager) {
		for (int i = 0; i < extManager.size(); i++)
			extManager[i]->CEPermissionsRegistered = false;
		extManager.save();
	}
	string t;
	sendMessage(ext->serviceInfo.name, "_core", "onuninstall", t, MessageTypes::MTCall, "");
	ext->state = ExtensionProfile::extensionState::notInstalled;
	ext->installID = "";
	extManager.save();
	string dt = "{\"name\":\"" + ext->serviceInfo.name.getValue() + "\"}";
	comm.fireEvent(eventInfo::onServiceUninstall(), dt, "_core");
	//todo:cleanup UM,PGDB
	return true;
}

bool Router::disableService(ExtensionProfile *ext) {
	if (!ext || ext->state != ExtensionProfile::extensionState::enabled)
		return false;
	string t;
	sendMessage(ext->serviceInfo.name, "_core", "ondisable", t, MessageTypes::MTCall, "");
	ext->state = ExtensionProfile::extensionState::installed;
	string dt = "{\"name\":\"" + ext->serviceInfo.name.getValue() + "\"}";
	comm.fireEvent(eventInfo::onServiceDisable(), dt, "_core");
	comm.cleanup(ext);
	extManager.save();
	this->disableIfReqNotSatisfied();
	return true;
}

void Router::ping(ExtensionProfile *exte) {
	this->net.send(exte->netClientId,
	               "{\"type\" : \"internal\" , \"node\" :\"ping\" "
	               + (exte->installID.size() > 0 ? " , \"id\" : \"" + CommunicationUtility::getRandomID() + "\""
	                                             : "") + "}");
	exte->pingStart = std::chrono::system_clock::now();
}

void Router::callCommandLocal(string node, string &data, string from, string id, string session) {
	JStruct jdata;
	if (data != "")
		jdata.fromString(data);
	if (streq(node, "_core.registerCommand")) {
		utility::JStruct j(data);
		utility::JArray &ja = (JArray &) j["names"];
		for (int i = 0; i < ja.size(); i++) {
			CommandProfile c(from, ja[i].getValue());
			comm.registerCommand(c);
		}
	} else if (streq(node, "_core.registerEvent")) {
		utility::JStruct j(data);
		utility::JArray &ja = (JArray &) j["names"];
		for (int i = 0; i < ja.size(); i++) {
			EventProfile c;
			c.name = ja[i].getValue();
			c.extension = from;
			comm.registerEvent(c);
		}
	} else if (streq(node, "_core.registerHook")) {
		utility::JStruct j(data);
		utility::JArray &ja = (JArray &) j["names"];
		for (int i = 0; i < ja.size(); i++) {
			HookProfile c;
			c.eventName = ja[i].getValue();
			c.extension = from;
			comm.registerHook(c);
		}
	} else if (streq(node, "_core.removeCommand")) {
		string inode = ((JVariable &) jdata["node"]).getValue();
		comm.removeCommand(inode);
	} else if (streq(node, "_core.removeEvent")) {
		string inode = ((JVariable &) jdata["node"]).getValue();
		comm.removeEvent(inode);
	} else if (streq(node, "_core.removeHook")) {
		string inode = ((JVariable &) jdata["node"]).getValue();
		HookProfile h{((JVariable &) jdata["event"]).getValue(), from, node, 0};
		comm.removeHook(h);
	} else if (streq(node, "_core.getListOfServices")) {
		string dt = "{\"extensions\":[";
		for (int i = 0; i < extManager.size(); i++) {
			ExtensionProfile *e = extManager[i];
			dt += string(i == 0 ? "" : ",") + "{\"name\":\"" + e->serviceInfo.name.getValue() +
			      "\",\"version\":" +
			      to_string(e->serviceInfo.serviceVersion.getValue()) + ",\"state\": " +
			      to_string((int) e->state) + "}";
		}
		dt += "]}";
		comm.callCallback(id, dt, "_core");
	} else if (streq(node, "_core.getServiceInfo")) {
		string sname = jdata["name"].getValue();
		ExtensionProfile *ext = extManager[sname];
		if (ext != NULL) {
			string dt = ext->serviceInfo.toString();
			comm.callCallback(id, dt, "_core");
		} else
			EXTinvalidName("No service with name '" + sname + "' exist.");
	} else if (streq(node, "_core.installService")) {
		string sname = jdata["name"].getValue();
		ExtensionProfile *ext = extManager[sname];
		if (ext != NULL && ext->isConnected()) {
			installService(ext);
		} else
			EXTinvalidName("No service with name '" + sname + "' exist.");
	} else if (streq(node, "_core.uninstallService")) {
		string sname = jdata["name"].getValue();
		ExtensionProfile *ext = extManager[sname];
		if (ext != NULL && ext->isConnected()) {
			uninstallService(ext);
		} else
			EXTinvalidName("No service with name '" + sname + "' exist.");
	} else if (streq(node, "_core.enableService")) {
		string sname = jdata["name"].getValue();
		ExtensionProfile *ext = extManager[sname];
		if (ext != NULL && ext->isConnected()) {
			enableService(ext);
		} else
			EXTinvalidName("No service with name '" + sname + "' exist.");
	} else if (streq(node, "_core.disableService")) {
		string sname = jdata["name"].getValue();
		ExtensionProfile *ext = extManager[sname];
		if (ext != NULL && ext->isConnected()) {
			disableService(ext);
		} else
			EXTinvalidName("No service with name '" + sname + "' exist.");
	} else if (streq(node, "_core.kickService")) {
		string sname = jdata["name"].getValue();
		ExtensionProfile *ext = extManager[sname];
		if (ext != NULL && ext->isConnected()) {
			net.clients[ext->netClientId]->stop();
		} else
			EXTinvalidName("No service with name '" + sname + "' exist.");
	} else if (streq(node, "_core.pingService")) {
		//todo: later
	} else if (streq(node, "error")) {
		lError("ERR RCV " + from + " : " + data);
		if (jdata.contains("id"))
			comm.callCallbackError(data, from);
	}
}


void Router::fireHookLocal(string node, string &data, string from) {
//todo:implement!
}

void Router::callCallbackLocal(string node, string &data, string from, string id) {
//todo:implement!
}

void Router::changeDatatypeVersion(string data, string extension, int vfrom, int vto) {
//todo:implement!
}

void Router::enableIfReqSatisfied() {
	for (int i = 0; i < this->extManager.size(); i++) {
		if (extManager[i]->isConnected() && extManager[i]->state == ExtensionProfile::extensionState::enabled &&
		    !extManager[i]->requirementsSatisfied) {
			extManager[i]->requirementsSatisfied = extManager.isReqSatisfied(
					extManager[i]->serviceInfo.enableRequirements);
			if (extManager[i]->requirementsSatisfied)
				this->enableService(extManager[i]);
		}
	}
}

void Router::disableIfReqNotSatisfied() {
	for (int i = 0; i < this->extManager.size(); i++) {
		if (extManager[i]->state == ExtensionProfile::extensionState::enabled &&
		    extManager[i]->requirementsSatisfied) {
			extManager[i]->requirementsSatisfied = extManager.isReqSatisfied(
					extManager[i]->serviceInfo.enableRequirements);
			if (!extManager[i]->requirementsSatisfied) {
				this->disableService(extManager[i]);
				extManager[i]->state = ExtensionProfile::extensionState::enabled;
				extManager.save();
			}
		}
	}
}


} /* namespace Core */
} /* namespace zeitoon */
