/*
 * WebSocketPort.cpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */
#include "WebSocketPort.hpp"
#include <utility/exceptions.hpp>
#include <thread>
#include <iostream>
#include <executables/GUI/websocketpp/common/connection_hdl.hpp>

using websocketpp::server;
using namespace zeitoon::utility;
using namespace std;

typedef std::map<websocketpp::connection_hdl, int, std::owner_less<websocketpp::connection_hdl>>::iterator ConnetionListIterator;

namespace zeitoon {
namespace GUI {

WebSocketPort::WebSocketPort() {
	GUI_WebSocketServer.
			clear_access_channels(websocketpp::log::alevel::all); //
	GUI_WebSocketServer.

			init_asio();

	GUI_WebSocketServer.set_reuse_addr(true);
	GUI_WebSocketServer.set_open_handler(std::bind(&WebSocketPort::on_open, this, websocketpp::lib::placeholders::_1));
	GUI_WebSocketServer.set_close_handler(
			std::bind(&WebSocketPort::on_close, this, websocketpp::lib::placeholders::_1));
	GUI_WebSocketServer.set_message_handler(
			std::bind(&WebSocketPort::on_message, this, websocketpp::lib::placeholders::_1,
			          websocketpp::lib::placeholders::_2));
	//listenThread = new std::thread(&WebSocketPort::sendo, this);
}

WebSocketPort::~WebSocketPort() {
//delete allocated mem for threads


}

void WebSocketPort::listenThreads(int iport) {
	try {
		this->port = iport == 0 ? this->port : iport;
		GUI_WebSocketServer.listen(this->port);
		GUI_WebSocketServer.start_accept();
		GUI_WebSocketServer.run();
	} catch (exceptionEx &ex) {
		lError("WS.Error.OnReceive: " + std::string(ex.what()));
	} catch (exception &ex) {
		lError("WS.sysError.OnReceive: " + std::string(ex.what()));
	} catch (...) {
		lError("WS.uncaughtError.OnReceive: UNKNOWN");
	}
}

void WebSocketPort::joinListenThread() {
	listenThread->join();
}

void WebSocketPort::listen(int portIn) {
	listenThread = new std::thread(&WebSocketPort::listenThreads, this, portIn);

}

void WebSocketPort::on_close(websocketpp::connection_hdl hdl) {
	lNote("WS Connection Removed");
	int ID = this->connectionList.at(hdl);
	this->connectionList.erase(hdl);
	if (_onClientDisconnect != NULL)
		_onClientDisconnect(ID);
}

void WebSocketPort::on_open(websocketpp::connection_hdl hdl) {
	lNote("New Client " + std::to_string(conIDCounter + 1));
	this->connectionList[hdl] = ++conIDCounter;
	if (_onClientConnect != NULL)
		_onClientConnect(conIDCounter);
}

void WebSocketPort::on_message(websocketpp::connection_hdl HDL, websocketServer::message_ptr MSG) {
	lDebug("WS-R: " + MSG->get_payload());
	this->received(HDL, MSG->get_payload());
}

void WebSocketPort::stop() {
	GUI_WebSocketServer.stop();
	lWarnig("Websocket stopped");
}

void WebSocketPort::stopListening() {
	GUI_WebSocketServer.stop_listening();
	lWarnig("Websocket stopped listening");
}

void WebSocketPort::send(websocketpp::connection_hdl client, std::string data) {

	GUI_WebSocketServer.send(client, data, websocketpp::frame::opcode::text);
	lDebug("WS-S: " + data);
}

void WebSocketPort::received(websocketpp::connection_hdl client, std::string data) { //client
	if (_onMessage == NULL) {
		return;
	}
	//if (sendStruct == NULL)//jaaaye in che konim baraye check??
	//return;
	std::thread *receivedThread = new std::thread(&WebSocketPort::receivedThreads, this, client, data);//fixme:free mem?
	//lock//scopelock
	this->threadsList.push_back(receivedThread);
}

void WebSocketPort::receivedThreads(websocketpp::connection_hdl client, std::string data) {
	try {
		int ID = this->connectionList.at(client);
		if (_onMessage == NULL) {
			return;
		}
		_onMessage(ID, data);
		for (unsigned int i = 0; i < this->threadsList.size(); i++) {
			if (this_thread::get_id() == this->threadsList[i]->get_id()) {//fixme:free memory of thread?
				this->threadsList.erase(this->threadsList.begin() + i);
			}
		}
	} catch (exceptionEx &ex) {
		lNote("WSR.Error.OnReceive: " + std::string(ex.what()));
	} catch (exception &ex) {
		lNote("WSR.Error.OnReceive: " + std::string(ex.what()));
	} catch (...) {
		lNote("WSR.Error.OnReceive: UNKNOWN");
	}
}

websocketpp::connection_hdl WebSocketPort::ConHdlFinder(int ID) {

	for (ConnetionListIterator it = this->connectionList.begin(); it != this->connectionList.end(); it++) {
		if (it->second == ID) {
			return it->first;
		}
	}
	EXTconnectionError("No corresponding connection for the given ID on connectionList!");
}

std::string WebSocketPort::getNameAndType() {
	return ("WebSocketPort");
}


}
}