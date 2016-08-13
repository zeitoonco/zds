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
#include <executables/UserManagement/DTStructs.hpp>
#include "utility/DTStructs.hpp"
#include "coreutility.hpp"
#include "executables/UserManagement/DTStructs.hpp"

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
			/*if (ext->state == ExtensionProfile::extensionState::upgrading) {
				lNote("Service " + ext->serviceInfo.name.getValue() + " is upgrading.");
				ext->state = ExtensionProfile::extensionState::installed;
			}*/
			extManager.save();
		} else if (type == "callback" && streq(node, "onInstall")) { //response from onInstall(success confirm)
			//todo:check success field! is it really a success?
			if (((JVariable &) packet["success"]).toBoolean()) {
				ext->state = ExtensionProfile::extensionState::installed;
				extManager.save();
				vector<ExtensionProfile *> elist = extManager.getByServiceType(
						datatypes::EnmServiceType::UserManager);
				if (elist.size() > 0 && elist[0]->isRunning()) { //UM avail
					/*TODO ELSE{ PLACE A METHOD TO STORE CEP IF UM IS UNAVAILABLE  AND  REG CEP WHEN UM
					 * COMES ONLINE*/
					registerServiceCEPermissions(
							ext);//fixme: seems that it happens twise, if um is running and we install a service
					//ext->CEPermissionsRegistered = ExtensionProfile::cepState::notRegistered;
				} else {
					ext->CEPermissionsRegistered = ExtensionProfile::cepState::notRegistered;
				}
				lNote("Service "+ext->serviceInfo.name.value()+" installed.");
				string dt = "{\"name\":\"" + ext->serviceInfo.name.value() + "\"}";
				comm.fireEvent(eventInfo::onServiceInstall(), dt, "_core");
			}else{
				ext->state = ExtensionProfile::extensionState::notInstalled;
				ext->installID = "";
			}

			extManager.save();

		} else if (type == "callback" && streq(node, "onEnable")) {
			if (((JVariable &) packet["data"]["success"]).toBoolean()) {
				ext->state = ExtensionProfile::extensionState::enabled;
				if (ext->CEPermissionsRegistered < ExtensionProfile::cepState::registered)
					registerCEP();
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
					if (id.size() > 0) {
						std::string success = packet["success"].getValue();
						comm.callCallback(id, idata, ext->serviceInfo.name.getValue(), success);
					}
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
		if (msgT == MessageTypes::MTCallback && success.size() > 0)
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
		    extManager[i]->CEPermissionsRegistered != ExtensionProfile::cepState::registered) {
			registerServiceCEPermissions(extManager[i]);
		}
	extManager.save();
}

void Router::registerServiceCEPermissions(ExtensionProfile *ext) {//WE have UM
	// PN: [serviceName].commands|events.[name]//if cep ==0 cep; if >
	vector<ExtensionProfile *> elist = extManager.getByServiceType(datatypes::EnmServiceType::UserManager);
	if (elist.size() > 0 && elist[0]->isRunning()) { //UM avail
		//get parent permisssions
		if (ext->CEPermissionsRegistered == ExtensionProfile::cepState::upgrading) {
			this->upgradeService(ext);
		} else {

			string pckt = "{\"name\":\"" + ext->serviceInfo.name.getValue() + "\",\"title\":\"" +
			              ext->serviceInfo.name2.getValue() + "\",\"description\":\"Service " +
			              ext->serviceInfo.name.getValue() + " Permissions\",\"parentID\":0}";
			datatypes::DSInteger idSrvs, idc, ide;
			idSrvs.fromString(
					comm.callCommandSync("userman.registerPermission", pckt, ext->serviceInfo.name, "REG1"));
			//commands and events group

			pckt = "{\"name\":\"" + ext->serviceInfo.name.getValue() +
			       ".commands\",\"title\":\"commands permissions\",\"description\":\"Permissions for running commands of service\",\"parentID\":" +
			       to_string(idSrvs.value.getValue()) + "}";
			idc.fromString(comm.callCommandSync("userman.registerPermission", pckt, ext->serviceInfo.name, "REG2"));
			pckt = "{\"name\":\"" + ext->serviceInfo.name.getValue() +
			       ".events\",\"title\":\"events permissions\",\"description\":\"Permissions for hooking to events of service\",\"parentID\":" +
			       to_string(idSrvs.value.getValue()) + "}";
			ide.fromString(comm.callCommandSync("userman.registerPermission", pckt, ext->serviceInfo.name, "REG3"));
			for (int ic = 0; ic < ext->serviceInfo.commands.length(); ic++) {
				datatypes::DSInstallInfo::DSCommandDetail *c = ext->serviceInfo.commands[ic];
				pckt = "{\"name\":\"" + c->name.getValue() + "\",\"title\":\"" + c->name.getValue() +
				       "\",\"description\":\"...\",\"parentID\":" + to_string(idc.value.getValue()) + "}";
				comm.callCommandSync("userman.registerPermission", pckt, ext->serviceInfo.name, c->name.getValue());
			}
			for (int ie = 0; ie < ext->serviceInfo.events.length(); ie++) {
				datatypes::DSInstallInfo::DSEventDetail *e = ext->serviceInfo.events[ie];
				pckt = "{\"name\":\"" + e->name.getValue() + "\",\"title\":\"" + e->name.getValue() +
				       "\",\"description\":\"...\",\"parentID\":" + to_string(ide.value.getValue()) + "}";
				comm.callCommandSync("userman.registerPermission", pckt, ext->serviceInfo.name, e->name.getValue());
			}
			ext->CEPermissionsRegistered = ExtensionProfile::cepState::registered;
		}
	} else {
		EXTresourceNotAvailable("UM extension is not available");
	}
	lNote("CEP registered for "+ext->serviceInfo.name.getValue());
}

bool Router::checkCoreRequirements() { //we need UM if we r gonna check permission of commands
	vector<ExtensionProfile *> elist = extManager.getByServiceType(datatypes::EnmServiceType::UserManager);
	if (elist.size() == 0)
		return true;
	else
		return elist[0]->isConnected() && elist[0]->state >= ExtensionProfile::extensionState::installed;
}

bool Router::enableService(ExtensionProfile *ext) {///-1
/*	if (!ext->CEPermissionsRegistered)
		upgradeService(ext);*/
	/*TODO:
	 * Uninstall all services
	 * Remove ZDS DB
	 * ReInstall one by one/
	 * Watch for changes in DB
	 * Watch for _core CEP reg
	 * Why CEP for _core & PG & UM is 0?
	 * These services try to reg CEP befor UM is enabled,
	 * Look for CEP registeration of _core and note when it regs its CEPs*/
	/*if (ext->state == ExtensionProfile::extensionState::enabled)
		return true;*/
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

void Router::upgradeService(ExtensionProfile *ext) {
	//todo: if um upgrade manualyy here!
	if (ext->state < ExtensionProfile::extensionState::installed) {
		lWarnig("Not upgrading. Please install the service first");
		return;
	}
	datatypes::DSInstallInfo siClone("", "", 0, 0, datatypes::EnmServiceType::other);
	siClone.fromString(ext->serviceInfo.toString(true));
	int smpID = 0, cmID = 0, evID = 0, rem = 0, ad = 0;
	zeitoon::datatypes::DSString tempStr;
	tempStr.value = siClone.name.getValue();
	string dummy = tempStr.toString(true);
	zeitoon::usermanagement::DSPermissionsList temp;

	temp.fromString(comm.callCommandSync("userman.listPermissions", dummy, "_core", "#REQ"));

/*	bool has;
	for (int cmit = 0; cmit < siClone.commands.length(); cmit++) {
		has = false;
		for (int cit = 0; cit < temp.permissionsList.length(); cit++) {
			if (temp.permissionsList[cit]->name.getValue() == siClone.commands[cmit]->name.getValue()) {
				has = true;
				break;
			}
		}
		if (has) {
			lWarnig("Upgrade aborted. no change in service permissions.");
			return;
		}
	}
	if (not has)
		for (int eit = 0; eit < siClone.events.length(); eit++) {
			has = false;
			for (int cit = 0; cit < temp.permissionsList.length(); cit++) {
				if (temp.permissionsList[cit]->name.getValue() == siClone.events[eit]->name.getValue()) {
					has = true;
					break;
				}
			}
			if (has) {
				lWarnig("Upgrade aborted. no change in service permissions.");
				return;
			}
		}*/

	std::map<std::string, int> pNames;
	for (int iter = 0; iter < temp.permissionsList.length(); iter++) {
		pNames[temp.permissionsList[iter]->name.getValue()] = temp.permissionsList[iter]->permissiosnID.getValue();
	}

	std::string exName = siClone.name.getValue();

	if (temp.permissionsList.length() < 1)
		return;
	for (int piter = 0; piter < temp.permissionsList.length(); piter++) {
		bool flag = false;
		for (int iter = 0; iter < siClone.commands.length(); iter++) {
			if (siClone.commands[iter]->name.getValue() == temp.permissionsList[piter]->name.getValue()) {
				siClone.commands.removeAt(iter);
				temp.permissionsList.removeAt(piter);
				iter--;
				piter--;
				flag = true;
				break;
			}
		}
		if (flag)
			continue;
		for (int iter = 0; iter < siClone.events.length(); iter++) {
			if (siClone.events[iter]->name.getValue() == temp.permissionsList[piter]->name.getValue()) {
				siClone.events.removeAt(iter);
				temp.permissionsList.removeAt(piter);
				iter--;
				piter--;
				flag = true;
				break;
			}
		}
		if (flag)
			continue;
		if (temp.permissionsList[piter]->name.getValue() == exName) {
			temp.permissionsList.removeAt(piter);
			piter--;
			continue;
		}
		if (temp.permissionsList[piter]->name.getValue() == exName + ".commands") {
			temp.permissionsList.removeAt(piter);
			piter--;
			continue;
		}
		if (temp.permissionsList[piter]->name.getValue() == exName + ".events") {
			temp.permissionsList.removeAt(piter);
			piter--;
			continue;


		}


	}
/*	for (int iter = 0; iter < siClone.commands.length(); iter++) {
		for (int oter = 0; oter < ext->oldServiceInfo.commands.length(); oter++) {
			if (siClone.commands[iter]->name.getValue() == ext->oldServiceInfo.commands[oter]->name.getValue()) {
				ext->oldServiceInfo.commands.removeAt(oter);
				siClone.commands.removeAt(iter);
				iter--;
				oter--;
				break;
			}
		}
	}
	for (int iter = 0; iter < siClone.events.length(); iter++) {
		for (int oter = 0; oter < ext->oldServiceInfo.events.length(); oter++) {
			if (siClone.events[iter]->name.getValue() == ext->oldServiceInfo.events[oter]->name.getValue()) {
				ext->oldServiceInfo.events.removeAt(oter);
				siClone.events.removeAt(iter);
				iter--;
				oter--;
				break;
			}
		}*//*TODO: shoudl not modify service info, nex time it will be emptyB*//*
	}*/
	cmID = pNames.at(exName + ".commands");
	evID = pNames.at(exName + ".events");

	std::cerr << "Removals:" <<
	(temp.permissionsList.length() > 1 ? "\n" + temp.permissionsList.toString(1) : "  None") << "\n";
	std::cerr << "Additions:\n\tCommands:" <<
	(siClone.commands.length() > 1 ? "\n" + siClone.commands.toString(1) : "  None") << "\n";
	std::cerr << "\tEvents:" << (siClone.events.length() > 1 ? "\n" + siClone.events.toString(1) : "  None") << "\n";
	int ID = 0;
	for (size_t iter = 0; iter < temp.permissionsList.length(); iter++) {
		zeitoon::datatypes::DSInteger tempI;
		tempI.value = pNames.at(temp.permissionsList[iter]->name.getValue());
		string pckt = tempI.toString(true);
		comm.callCommandSync("userman.removePermission", pckt, "_core", "servUpgrd" + std::to_string(++ID));
		rem++;
	}
	/*for (size_t iter = 0; iter < ext->oldServiceInfo.events.length(); iter++) {
		zeitoon::datatypes::DSInteger tempI;
		tempI.value = pNames.at(ext->oldServiceInfo.events[iter]->name.getValue());
		string pckt = tempI.toString(true);
		comm.callCommandSync("userman.removePermission", pckt, "_core", "servUpgrd" + iter);
		rem++;
	}*/
	for (size_t iter = 0; iter < siClone.commands.length(); iter++) {
		auto cm = siClone.commands[iter];
		zeitoon::usermanagement::DSRegPermission tempI(cm->name.getValue(), cm->name.getValue(), "...", cmID);
		string pckt = tempI.toString(true);
		comm.callCommandSync("userman.registerPermission", pckt, ext->serviceInfo.name.getValue(),
		                     "servUpgrd" + std::to_string(++ID));
		ad++;
	}
	for (size_t iter = 0; iter < siClone.events.length(); iter++) {
		auto ev = siClone.events[iter];
		zeitoon::usermanagement::DSRegPermission tempI(ev->name.getValue(), ev->name.getValue(), "...", evID);
		string pckt = tempI.toString(true);
		comm.callCommandSync("userman.registerPermission", pckt, ext->serviceInfo.name.getValue(),
		                     "servUpgrd" + std::to_string(++ID));
		ad++;
	}
	ext->oldServiceInfo.clear();
	lDebug("Database updated\n\tAddition: " + std::to_string(ad) + "  Removal: " + std::to_string(rem) +
	       "\nUpgrade completed.");

	ext->CEPermissionsRegistered = ExtensionProfile::cepState::registered;
}

bool Router::getInstallInfo(ExtensionProfile *ext) {
	string t;
	sendMessage(ext->serviceInfo.name, "_core", "getinstallInfo", t, MessageTypes::MTCall, "INSTALLID");
	return true;
}

bool Router::forceUninstallService(ExtensionProfile *ext) {
	if (!ext)
		return false;
	if (ext->isConnected()) {
		//Todo: kick service
	}
	std::string serName =ext->serviceInfo.name.getValue();//todo :whole thing doesnt do shit.
	string dt = "{\"name\":\"" + serName + "\"}";
	comm.cleanup(ext);
	extManager.remove(ext);
	extManager.save();
	this->disableIfReqNotSatisfied();
	comm.fireEvent(eventInfo::onServiceUninstall(), dt, "_core");
	std::string t;
	lWarnig("FORCE-UNINSTALL for service "+serName+" compeleted");
	return true;
}

bool Router::uninstallService(ExtensionProfile *ext) {
	if (!ext || ext->state < ExtensionProfile::extensionState::installed)
		return false;
	string t;
	sendMessage(ext->serviceInfo.name, "_core", "onuninstall", t, MessageTypes::MTCall, "");
	ext->state = ExtensionProfile::extensionState::notInstalled;
	ext->installID = "";
	ext->CEPermissionsRegistered = ExtensionProfile::cepState::notRegistered;
	extManager.save();
	string dt = "{\"name\":\"" + ext->serviceInfo.name.getValue() + "\"}";
	if (ext->state == ExtensionProfile::extensionState::enabled)
		disableService(ext);
	if (ext->serviceInfo.serviceType.getValue() == datatypes::EnmServiceType::UserManager) {
		for (int i = 0; i < extManager.size(); i++)
			extManager[i]->CEPermissionsRegistered = ExtensionProfile::cepState::notRegistered;
		extManager.save();
	}/*todo clear up extention --cep =notreged*/

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
	comm.cleanup(ext);
	extManager.save();
	comm.fireEvent(eventInfo::onServiceDisable(), dt, "_core");
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
		comm.callCallback(id, dt, "_core", "true");//ddd
	} else if (streq(node, "_core.getServiceInfo")) {
		string sname = jdata["name"].getValue();
		ExtensionProfile *ext = extManager[sname];
		if (ext != NULL) {
			string dt = ext->serviceInfo.toString();
			comm.callCallback(id, dt, "_core", "true");
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
