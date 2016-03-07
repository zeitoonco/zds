//
// Created by ajl on 3/6/16.
//

#include <network/TCPClient.hpp>
#include <iostream>

using namespace zeitoon::utility;

void onmsg(std::string msg) {
	std::cerr << "\nMSG reeceived : " << msg << std::endl;
}

int main() {

	TCPClient a("127.0.0.1", 5456);
	a.registerOnMessageCB(onmsg);
	a.connect();

	std::string in = "";
	while (in != "q") {
		std::cin >> in;
		a.send(in);
	}

	a.disconnect();
	return 0;
}