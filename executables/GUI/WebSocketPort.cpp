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

using websocketpp::server;
using namespace zeitoon::utility;
using namespace std;

typedef std::map<websocketpp::connection_hdl, int, std::owner_less<websocketpp::connection_hdl>>::iterator ConnetionListIterator;

namespace zeitoon {
namespace GUI {

WebSocketPort::WebSocketPort(funcDLG onMsgPtr) {
	onMessageCB = onMsgPtr;
	GUI_WebSocketServer.clear_access_channels(websocketpp::log::alevel::all); //
	GUI_WebSocketServer.init_asio();
	GUI_WebSocketServer.set_reuse_addr(true);
	GUI_WebSocketServer.set_open_handler(std::bind(&WebSocketPort::on_open, this, websocketpp::lib::placeholders::_1));
	GUI_WebSocketServer.set_close_handler(
			std::bind(&WebSocketPort::on_close, this, websocketpp::lib::placeholders::_1));
	GUI_WebSocketServer.set_message_handler(
			std::bind(&WebSocketPort::on_message, this, websocketpp::lib::placeholders::_1,
			          websocketpp::lib::placeholders::_2));
	listenThread = new std::thread(&WebSocketPort::sendo, this);
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
		cerr << "WS.Error.OnReceive: " << ex.what() << endl;
	} catch (exception &ex) {
		cerr << "WS.sysError.OnReceive: " << ex.what() << endl;
	} catch (...) {
		cerr << "WS.uncaughtError.OnReceive: " << endl;//todo:use logger
	}
}

void WebSocketPort::joinListenThread() {
	listenThread->join();
}

void WebSocketPort::listen(int portIn) {
	listenThread = new std::thread(&WebSocketPort::listenThreads, this, portIn);

}

void WebSocketPort::sendo() {//fixme:@navidi:WTF!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	std::string MSg = "";
	int IDNum = 0;
	while (MSg != "\\NO") {
		std::cout << "ID#> " << std::endl;
		std::cin >> IDNum;
		std::cout << "Enter ur MSG#>" << std::endl;
		std::cin >> MSg;
		for (ConnetionListIterator it = this->connectionList.begin(); it != this->connectionList.end(); it++) {
			if (it->second == IDNum) {
				this->send(it->first, MSg);
			}
		}
	}
}

void WebSocketPort::on_close(websocketpp::connection_hdl hdl) {
	this->connectionList.erase(hdl);
}

void WebSocketPort::on_open(websocketpp::connection_hdl hdl) {
	std::cout << "It has been Opened" << std::endl;
	this->connectionList[hdl] = ++conIDCounter;
	for (ConnetionListIterator it = this->connectionList.begin(); it != this->connectionList.end(); it++) {
		std::cout << "ID: " << it->second << "\t HDL: " << &it->first << std::endl;
		this->send(it->first, std::to_string(conIDCounter) + " Has joined the room.");
	}
}

void WebSocketPort::on_message(websocketpp::connection_hdl HDL, websocketServer::message_ptr MSG) {
	std::cout << "MSG: " << MSG->get_payload() << std::endl;
	this->received(HDL, MSG->get_payload());


}

void WebSocketPort::stop() {
	GUI_WebSocketServer.stop();
}

void WebSocketPort::stopListening() {
	GUI_WebSocketServer.stop_listening();
}

void WebSocketPort::send(websocketpp::connection_hdl client, std::string data) {

	GUI_WebSocketServer.send(client, data, websocketpp::frame::opcode::text);

}

void WebSocketPort::received(websocketpp::connection_hdl client, std::string data) { //client
	//if (sendStruct == NULL)//jaaaye in che konim baraye check??
	//return;
	std::thread *receivedThread = new std::thread(&WebSocketPort::receivedThreads, this, client, data);//fixme:free mem?
	//lock//scopelock
	this->threadsList.push_back(receivedThread);
	std::cout << "\nID:" << receivedThread->get_id();

}

void WebSocketPort::receivedThreads(websocketpp::connection_hdl client, std::string data) {
	try {
		int ID = this->connectionList.at(client);
		if (onMessageCB == NULL) {
			return;
		}
		onMessageCB(ID, data);
		std::cout << "\nRemoving Threat from the list" << std::endl;
		for (unsigned int i = 0; i < this->threadsList.size(); i++) {
			if (this_thread::get_id() == this->threadsList[i]->get_id()) {//fixme:free memory of thread?
				this->threadsList.erase(this->threadsList.begin() + i);
			}
		}
	} catch (exceptionEx &ex) {
		cerr << "WSR.Error.OnReceive: " << ex.what() << endl;
	} catch (exception &ex) {
		cerr << "WSR.sysError.OnReceive: " << ex.what() << endl;
	} catch (...) {
		cerr << "WSR.uncaughtError.OnReceive: " << endl;
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
	return ("[WebSocketPort]");
}


}
}