//
// Created by inf on 3/6/16.
//
#include "executables/UserManagement/UmCHI.hpp"

using namespace zeitoon::usermanagement;

int main(int argc, char *argv[]) {
	UmCHI Umediator;
	if (argc != 3) {
		try {
			std::cout << "Invalid number of arguments provided\nLoading via configuration file" << std::endl;
			Umediator.connect(UMconfig.serverIP.getValue(), std::stoi(UMconfig.serverPort.getValue()));
			Umediator.sm.joinNet();
		} catch (exceptionEx *err) {
			std::cerr << "Required arguments: ServerIP ServerPort\n";
			std::cerr << "ERROR:\n" << err->what() << std::endl;
			-1;
		} catch (exception &err) {
			std::cerr << "UnknownERROR:\n" << err.what();
		}

	}
	std::cout << "Server Addr:\t" << argv[1] << "\tPort:\t" << argv[2] << std::endl;

	try {
		UMconfig.serverIP = argv[1];
		UMconfig.serverPort = argv[2];
		Umediator.connect(UMconfig.serverIP.getValue(), std::stoi(UMconfig.serverPort.getValue()));
		Umediator.sm.joinNet();
	} catch (exceptionEx *err) {
		std::cerr << "ERROR:\n" << err->what();
	} catch (exception &err) {
		std::cerr << "UnknownERROR:\n" << err.what();
	}
}