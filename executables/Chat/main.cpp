// Created by naeyrin on 5/8/16.
//

#include "executables/Chat/Chat.hpp"
#include "executables/Chat/chatCHI.hpp"
#include <iostream>

using namespace zeitoon::chat;

int main(int argc, char *argv[]) {

	zeitoon::chat::ChatCHI ChatMediator;
	Chatconfig.load();
	std::string serverIP = "", serverPort = "";
	if (argc != 3) {
		std::cout << "Invalid number of arguments provided" << std::endl;
		std::cout << "Required arguments: ServerIP ServerPort" << std::endl;
		std::cout << "Trying to load from configuration" << std::endl;
		try {
			serverIP = Chatconfig.serverIP.getValue();
			serverPort = Chatconfig.serverPort.getValue();
		} catch (exceptionEx &err) {
			std::cerr << "Error:\n" << err.what() << std::endl;
		}
	} else {
		serverIP = argv[1];
		serverPort = argv[2];
	}

	try {
		ChatMediator.connect(serverIP, std::stoi(serverPort));
		std::cout << "Server Addr:\t" << serverIP << "\tPort:\t" << serverPort << std::endl;
		ChatMediator.sm.joinNet();
	} catch (exceptionEx &err) {
		std::cout << "ERROR:\n" << err.what() << endl;
	} catch (exception &err) {
		std::cout << "UnknownERROR:\n" << err.what() << std::endl;
	}

}