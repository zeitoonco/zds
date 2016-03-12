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

#define helloPacket "{\"type\" : \"internal\" , \"node\" : \"hello\"}"

namespace zeitoon {
namespace _core {

Router::Router(int port) :
		comm(bind(&Router::sendMessage, this, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4,
		          placeholders::_5, placeholders::_6)), net(port) { //, net(102030) {
	// TODO Auto-generated constructor stub
	net.registerOnMessageCB(bind(&Router::_onDataReceive, this, std::placeholders::_1, std::placeholders::_2));
	net.registerOnClientConnectCB(bind(&Router::_onClientConnect, this, std::placeholders::_1));
	net.registerOnClientDisconnectCB(bind(&Router::_onClientDisconnect, this, std::placeholders::_1));
	string cpath = FileSystemUtility::getAppPath();
	db.connect(cpath + "data.db3");
	net.listen();
}

Router::~Router() {
	db.disconnect();
}

void Router::_onClientConnect(size_t id) {
	extManager.add(id);
	net.send(id, helloPacket);
	cerr << "\nNew Client: " << id;
}

void Router::_onClientDisconnect(size_t id) {
	comm.cleanup(extManager[id]);
	extManager.remove(id);
	//todo:clean commMangr (things of this ext)
}

void Router::_onDataReceive(size_t id, std::string data) {
	ExtensionProfile *e = extManager[id];
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
	packetReceived(data, e);
}

void Router::packetReceived(string data, ExtensionProfile *ext) {
	cerr << "\nNetR(" << ext->netClientId << "):" << data;
	using zeitoon::datatypes::DTTableString;
	JStruct packet(data);
	string type, node, idata, id;
	type = packet["type"].getValue();
	node = packet["node"].getValue();

	if (seq(type, "internal")) {
		if (seq(node, "ping")) {
			id = packet["node"].getValue();
			net.send(ext->netClientId, "{\"type\":\"internal\",\"node\":\"pong\",\"id\":\"" + id + "\"}");
		} else if (seq(node, "pong")) {
			//um..what?! we even dont have a function to send ping yet!!!
		} else if (seq(node, "hello")) {
			string name = packet["name"].getValue();
			if (name.length() < 3) {
				net.send(ext->netClientId,
				         utility::CommunicationUtility::makeError(node, "", "Invalid service name"));
				net.clients[ext->netClientId]->stop();
			}
			int version = atoi(packet["version"].getValue().c_str());
			//todo::validate version too
			if (packet.contains("id")) {//installed. maybe!
				string iid = packet["id"].getValue();//install id
				DTTableString dt = db.query("select installid,version,state from services where name=\"" + name + "\"");
				if (dt.rowCount() > 0 && iid == dt.fieldValue(0, 0)) {//yep, hes right
					if (version == atoi(dt.fieldValue(0, 1).c_str())) {//right version
						ext->name = name;
						ext->version = version;
						ext->state = (ExtensionProfile::extensionState) atoi(dt.fieldValue(0, 2).c_str());
						ext->istate = ExtensionProfile::extensioniState::running;//todo:check requirements!
						cerr << "\nClient " << ext->netClientId << " identified as '" << ext->name << "' v" <<
						ext->version << ". Installed with id '" << iid << "'";
					} else {
						net.send(ext->netClientId,
						         utility::CommunicationUtility::makeError(node, "",
						                                                  "Invalid version. Installed version: " +
						                                                  dt.fieldValue(0, 1)));
						net.clients[ext->netClientId]->stop();
					}
				} else {//wrong
					net.send(ext->netClientId,
					         utility::CommunicationUtility::makeError(node, "", "Invalid service or service ID."));
					net.clients[ext->netClientId]->stop();
				}

			} else {//not installed
				DTTableString dt = db.query("select count(*) from services where name=\"" + name + "\"");
				if (atoi(dt.fieldValue(0, 0).c_str()) == 0) {
					ext->name = name;
					ext->version = version;
					ext->state = ExtensionProfile::extensionState::notInstalled;
					ext->istate = ExtensionProfile::extensioniState::running;
					cerr << "\nClient " << ext->netClientId << " identified as '" << ext->name << "' v" <<
					ext->version << ". Not installed.";
				} else { //another service with this name exist
					net.send(ext->netClientId,
					         utility::CommunicationUtility::makeError(node, "", "A service with this name exists."));
					net.clients[ext->netClientId]->stop();
				}
			}
		}
	} else {
		idata = packet["data"].toString();
		if (packet.contains("id"))
			id = packet["id"].getValue();
		if (seq(node, "getInstallInfo")) {
			cerr << "\nGot Install Info from '" << ext->netClientId << ">" << ext->name << ": " << data;
			ext->state = ExtensionProfile::extensionState::installed;
			string niid = utility::CommunicationUtility::getRandomID();
			db.execute("insert into services(name,version,state,installid) values(\"" + ext->name + "\",\"" +
			           to_string(ext->version) + "\",\"" + to_string((int) ext->state) + "\",\"" + niid + "\")");
			net.send(ext->netClientId,
			         utility::CommunicationUtility::makeCommand("onInstall", "", "_core", "{\"id\":\"" + niid + "\"}"));
		} else {
			try {
				if (type == "call")
					comm.callCommand(node, idata, ext->name, id);
				else if (type == "fire")
					comm.fireEvent(node, idata, ext->name);
				else if (type == "callbackList")
					comm.callCallback(node, idata, ext->name, id);
			} catch (exceptionEx *ex) {
				cerr << "\nERROR : " << ex->what();//todo:send error?
			} catch (exception &ex) {
				cerr << "\nERROR unk : " << ex.what();
			}
		}
	}

}

void Router::sendPacket(string &data, ExtensionProfile *extension) {
	net.send(extension->netClientId, data);
}

void Router::sendMessage(string extension, string source, string node, string &data, MessageTypes::MessageTypes_ msgT,
                         string id) {
	if (seq(extension, "_core")) {
		switch (msgT) {
			case MessageTypes::MTCall:
				callCommandLocal(node, data, source, id);
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
		if (data.length() < 0)
			msg.addIgnored("data", data);
		size_t extid = extManager.idOf(extension);
		string msgs = msg.toString();
		this->sendPacket(msgs, extManager[extid]);
	}
}

void Router::callCommandLocal(string node, string &data, string from, string id) {
	JStruct jdata;
	jdata.fromString(data);
	if (seq(node, "_core.registerCommand")) {
		utility::JStruct j(data);
		utility::JArray &ja = (JArray &) j["names"];
		for (int i = 0; i < ja.size(); i++) {
			CommandProfile c(from, ja[i].getValue());
			comm.registerCommand(c);
		}
	} else if (seq(node, "_core.registerEvent")) {
		utility::JStruct j(data);
		utility::JArray &ja = (JArray &) j["names"];
		for (int i = 0; i < ja.size(); i++) {
			EventProfile c;
			c.name = ja[i].getValue();
			c.extension = from;
			comm.registerEvent(c);
		}
	} else if (seq(node, "_core.registerHook")) {
		utility::JStruct j(data);
		utility::JArray &ja = (JArray &) j["names"];
		for (int i = 0; i < ja.size(); i++) {
			HookProfile c;
			c.eventName = ja[i].getValue();
			c.extension = from;
			comm.registerHook(c);
		}
	} else if (seq(node, "_core.removeCommand")) {
		string inode = ((JVariable &) jdata["node"]).getValue();
		comm.removeCommand(inode);
	} else if (seq(node, "_core.removeEvent")) {
		string inode = ((JVariable &) jdata["node"]).getValue();
		comm.removeEvent(inode);
	} else if (seq(node, "_core.removeHook")) {
		string inode = ((JVariable &) jdata["node"]).getValue();
		HookProfile h{((JVariable &) jdata["event"]).getValue(), from, node, 0};
		comm.removeHook(h);
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

} /* namespace Core */
} /* namespace zeitoon */
