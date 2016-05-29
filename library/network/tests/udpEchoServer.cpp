//
// Created by inf on 3/15/16.
//

#include "network/UDPClient.hpp"
#include <iostream>

using namespace zeitoon::utility;

void printer(std::string DATA, std::string IP) {
	std::cout << "Size of data:  " << DATA.size() << "  From: " << IP << std::endl;
	std::cout << "Data:\t" << DATA << std::endl;
}

void printerBuf(char *buffer, int nreads, std::string IP) {
	std::stringstream temp;
	temp << std::hex << std::uppercase;
	for (int i = 0; i < nreads; i++) {
		if ((uint8_t) (buffer[i]) < 16) { temp << 0; }
		temp << (uint) (uint8_t) (buffer[i]) << " ";
	}
	temp << std::dec << std::nouppercase;

	std::cout << "Size of data:  " << nreads << "  From: " << IP << std::endl;

	std::cout << std::endl;
	std::cout << temp.str();
}

int main() {
	try {
		UDPClient x;
		x.registerOnMessage(printer);
		//	x.registerOnMessageBinary(printerBuf);
		x.listen("127.0.0.1", "15001");

		//UDPClient x("127.0.0.1", 15001);
		//x.registerOnMessageCB(printer);
		//x.listen();

		x.joinListenThread();

	} catch (exceptionEx &err) {
		std::cerr << err->what();
	}
}