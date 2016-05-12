//
// Created by ajl on 3/6/16.
//

#include <network/TCPServer.hpp>

using namespace zeitoon::utility;

void onmsg(size_t id, std::string msg);

TCPServer a(5456);

int main() {
	a.listen();
	a.registerOnMessageCB(onmsg);


	a.joinOnListenThread();
//	a.disconnect();
	return 0;
}

void onmsg(size_t id, std::string msg) {
	std::cerr << "\nMSG received from " << id << " l " << msg.size();
	a.clients[id]->send(msg);
}