/*
 * WebSocketPort.hpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */

#ifndef WEBSOCKETPORT_HPP_
#define WEBSOCKETPORT_HPP_

#include"websocketpp/config/asio_no_tls.hpp"
#include"websocketpp/server.hpp"
#include<map>
#include<set>
#include<vector>
#include <functional>


typedef websocketpp::server<websocketpp::config::asio> websocketServer;

namespace zeitoon {
namespace GUI {


class WebSocketPort {
	websocketServer GUI_WebSocketServer;
	boost::asio::io_service ios;
	int port = 0;
	typedef std::function<void(int clientID, std::string data)> funcDLG;
	funcDLG onMessageCB;

	std::thread *listenThread;

	void listenThreads(int iport = 0);

	void on_open(websocketpp::connection_hdl hdl);

	void on_close(websocketpp::connection_hdl hdl);

	void on_message(websocketpp::connection_hdl HDL, websocketServer::message_ptr MSG);

	std::map<websocketpp::connection_hdl, int, std::owner_less<websocketpp::connection_hdl>> connectionList;

	void receivedThreads(websocketpp::connection_hdl client, std::string data);

	struct threadInfo {
		std::thread *thisThread;
		std::thread::id ID;

		//= thisThread->get_id();
		threadInfo(std::thread *ithisThread) {
			thisThread = ithisThread;
			ID = thisThread->get_id();

		}

	};

	int conIDCounter = 0;
	std::vector<std::thread *> threadsList;

	void sendo();

public:
	/*todo:hichi
	 *
	 */
	WebSocketPort(funcDLG);

	~WebSocketPort();

	void listen(int port = 0);

	void stop();


	void send(websocketpp::connection_hdl client, std::string data);

	void received(websocketpp::connection_hdl client, std::string data);

	void joinListenThread();

	void stopListening();

	void threadTerminator(std::thread myThread);

	websocketpp::connection_hdl ConHdlFinder(int ID);

	std::string getNameAndType();

};
}
}

//typedef boost::shared_ptr<WebSocketPort> chat_server_handler_ptr;
#endif /* WEBSOCKETPORT_HPP_ */
