/*
 * CommunicationHandlerInterface.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#ifndef MEDIATOR_COMMUNICATIONHANDLERINTERFACE_HPP_
#define MEDIATOR_COMMUNICATIONHANDLERINTERFACE_HPP_

#include "utility/jsonParser.hpp"
#include "ServerMediator.hpp"
#include <chrono>

namespace zeitoon {
namespace utility {

class CommunicationHandlerInterface {
private:
	map<string, std::chrono::system_clock::time_point> pingtimes;

public:
	ServerMediator sm;

	CommunicationHandlerInterface(CommunicationHandlerInterface *owner) :
			sm(owner) {
	}

	virtual ~CommunicationHandlerInterface() {
	}

	virtual string ping(string id = "") {
		if (id.length() == 0)
			id = CommunicationUtility::getRandomID();
		pingtimes[id] = std::chrono::system_clock::now();
		sm.send("{\"type\" : \"internal\" , \"node\" : \"ping\" , \"id\" : \"" + id + "\"}");
		return id;
	}

	virtual void onCommand(string node, string data, string id, string from) = 0;

	virtual void onCallback(string node, string data, string id, string from) = 0;

	virtual void onEvent(string node, string data, string from) = 0;

	virtual void onInstall(string id) = 0;

	virtual void onEnable() = 0;

	virtual void onDisable() = 0;

	virtual void onUninstall() = 0;

	virtual void onConnect() = 0;

	virtual void onDisconnect() = 0;

	virtual string getInstallInfo() = 0;

	virtual string getInstallID() = 0;

	virtual string getServiceName() = 0;

	virtual size_t getServiceVersion() = 0;

	virtual string changeDatatypeVersion(string value, string datatype, int fromVersion, int toVersion,
	                                     int &newVersion) = 0;

	virtual void onError(string node, string id, string description) = 0;

	virtual void onWarning(string level, string node, string id, string description) = 0;

	virtual void pong(string id, int miliseconds) = 0;

	virtual bool datareceive(zeitoon::utility::JStruct &js) {
		//JStruct js(data);//todo:dont parse data field
		string type = js["type"].getValue();
		string node = js["node"].getValue();
		if (!Strings::compare(type, "internal", false)) {
			if (!Strings::compare(node, "pong", false)) {
				string id = js["node"].getValue();
				std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
				auto d = std::chrono::duration_cast<std::chrono::duration<int, ratio<1, 1000>>>(t - pingtimes[id]);
				pingtimes.erase(id);
				pong(id, d.count());
			} else if (!Strings::compare(node, "hello", false)) {
				string id = getInstallID();
				sm.send(
						"{\"type\" : \"internal\" , \"node\" : \"hello\" , \"name\" : \"" + getServiceName() +
						"\" , \"version\" : " + std::to_string(this->getServiceVersion())
						+ (id.length() > 0 ? " , \"id\" : \"" + id + "\"" : "") + "}");
			}
		} else {
			JItem *dataj = js.getField("data");
			JItem *fromj = js.getField("from");
			JItem *idj = js.getField("id");
			string data = (dataj == NULL ? "" : dataj->value->getValue());
			string from = (fromj == NULL ? "" : fromj->value->getValue());
			string id = (idj == NULL ? "" : idj->value->getValue());
			if (!Strings::compare(type, "fire", false)) { //communication
				this->onEvent(node, data, from);
			} else if (!Strings::compare(type, "callback", false)) { //communication
				this->onCallback(node, data, id, from);
			} else if (!Strings::compare(type, "call", false)) { //communication
				if (!Strings::compare(node, "onInstall", false)) {
					if (dataj == NULL)
						EXTinvalidParameter("onInstall: Data field is empty!");
					JStruct &jdata = *((JStruct *) dataj->value);
					this->onInstall(jdata["id"].getValue());
					//todo: receive a bool, that shows if install process was successful, for next line
					this_thread::sleep_for(chrono::milliseconds(200));
					sm.send(CommunicationUtility::makeCallback("onInstall", id, getServiceName(), "{\"success\":true}"));
				} else if (!Strings::compare(node, "onUninstall", false)) {
					this->onUninstall();
				} else if (!Strings::compare(node, "onEnable", false)) {
					this->onEnable();
					//todo: receive a bool, that shows if install process was successful, for next line
					this_thread::sleep_for(chrono::milliseconds(200));
					sm.send(CommunicationUtility::makeCallback("onEnable", id, getServiceName(), "{\"success\":true}"));
				} else if (!Strings::compare(node, "onDisable", false)) {
					this->onDisable();
				} else if (!Strings::compare(node, "error", false)) {
					if (dataj == NULL)
						EXTinvalidParameter("error: Data field is empty!");
					JStruct &jdata = *((JStruct *) dataj->value);
					this->onError(jdata["node"].getValue(), jdata["id"].getValue(), jdata["description"].getValue());
				} else if (!Strings::compare(node, "warning", false)) {
					if (dataj == NULL)
						EXTinvalidParameter("warning: Data field is empty!");
					JStruct &jdata = *((JStruct *) dataj->value);
					this->onWarning(jdata["level"].getValue(), jdata["node"].getValue(), jdata["id"].getValue(),
					          jdata["description"].getValue());
				} else if (!Strings::compare(node, "getInstallInfo", false)) {
					sm.send(CommunicationUtility::makeCallback("getInstallInfo", id, getServiceName(),
					                                           this->getInstallInfo()));
				} else if (!Strings::compare(node, "changeDatatypeVersion", false)) {
					if (dataj == NULL)
						EXTinvalidParameter("changeDatatypeVersion: Data field is empty!");
					JStruct &jdata = *((JStruct *) dataj->value);
					int newVer;
					string newdata = this->changeDatatypeVersion(
							jdata["value"].getValue(),
							jdata["datatype"].getValue(),
							((JVariable &) jdata["fromversion"]).toInt(),
							((JVariable &) jdata["toversion"]).toInt(), newVer);
					sm.send(
							CommunicationUtility::makeCallback("changeDatatypeVersion", id, getServiceName(),
							                                   "{ \"datatype\" : \"" + jdata["datatype"].getValue() +
							                                   "\" , \"newversion\" : \"" + Strings::itoa(newVer)
							                                   + "\" , \"value\" : " + newdata + "}"
							));
				} else
					this->onCommand(node, data, id, from);
			}
		}
		return false;
	}

	virtual void connect(string serverIP, int serverPort) {
		sm.connect(serverIP, serverPort);
	}

	virtual string getNameAndType() {
		return "CommunicationHandlerInterface";
	}
};

} /* namespace utility */
} /* namespace zeitoon */

#endif /* MEDIATOR_COMMUNICATIONHANDLERINTERFACE_HPP_ */
