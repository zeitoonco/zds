//
// Created by inf on 3/6/16.
//
#include "executables/UserManagement/UmCHI.hpp"

using namespace zeitoon::usermanagement;

int main(int argc, char *argv[]) {
	UmCHI Umediator;
	UMconfig.load();
	std::string serverIP = "", serverPort = "";
	if (argc != 3) {
		std::cout << "Invalid number of arguments provided" << std::endl;
		std::cout << "Required arguments: ServerIP ServerPort" << std::endl;
		std::cout << "Trying to load from configuration" << std::endl;
		try {
			serverIP = UMconfig.serverIP.getValue();
			serverPort = UMconfig.serverPort.getValue();
		} catch (exceptionEx &err) {
			std::cerr << "Error:\n" << err.what() << std::endl;
		}
	} else {
		serverIP = argv[1];
		serverPort = argv[2];
	}


	try {
		Umediator.connect(serverIP, std::stoi(serverPort));
		std::cout << "Server Addr:\t" << serverIP << "\tPort:\t" << serverPort << std::endl;
		Umediator.sm.joinNet();
	} catch (exceptionEx &err) {
		std::cout << "ERROR:\n" << err.what() << endl;
	} catch (exception &err) {
		std::cout << "UnknownERROR:\n" << err.what() << std::endl;
	}


}