// Created by naeyrin on 5/8/16.
//

#include "executables/Chat/Chat.hpp"
#include "executables/Chat/chatCHI.hpp"
#include <iostream>

using namespace zeitoon::chat;

int main(int argc, char *argv[]) {
	logger.enableFile("Chat_Log");
	logger.enableTerminalOut();
	zeitoon::chat::ChatCHI ChatMediator;
	Chatconfig.load();
	if (not Chatconfig.exists("networkReconnectInterval")){
		Chatconfig.addConfig("networkReconnectInterval", ChatMediator.sm.getDefaultNetReconnect());
		Chatconfig.save();
	}
	ChatMediator.sm.setNetReconnectInterval(Chatconfig.get("networkReconnectInterval"));
	std::string serverIP = "", serverPort = "";
	if (argc < 3) {
		std::cout << "Invalid number of arguments provided" << std::endl;
		std::cout << "Required arguments: ServerIP ServerPort" << std::endl;
		std::cout << "Trying to load from configuration" << std::endl;
	} else {
		Chatconfig.serverIP = argv[1];
		Chatconfig.serverPort = argv[2];
	}

	try {
		ChatMediator.connect(Chatconfig.serverIP.getValue(), std::stoi(Chatconfig.serverPort.getValue()));
		std::cout << "Server Addr:\t" << Chatconfig.serverIP.getValue()	 << "\tPort:\t" << Chatconfig.serverPort.getValue() << std::endl;
		if (argc == 4) {
			if (std::string(argv[3]) == "-save") {
				Chatconfig.save();
				std::cerr << "Configuration saved\n";
			}
		}
		ChatMediator.sm.joinNet();
	} catch (exceptionEx &err) {
		std::cout << "ERROR:\n" << err.what() << endl;
	} catch (exception &err) {
		std::cout << "UnknownERROR:\n" << err.what() << std::endl;
	}

}