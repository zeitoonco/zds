//
// Created by inf on 3/6/16.
//
#include "executables/UserManagement/UmCHI.hpp"

using namespace zeitoon::usermanagement;

int main(int argc, char *argv[]) {
	UmCHI Umediator;
	if (argc != 3) {
		std::cout << "Invalid number of arguments provided" << std::endl;
		std::cout << "Required arguments: ServerIP ServerPort" << std::endl;
		std::cout << "Trying to load from configuration" << std::endl;
		try {
			UMconfig.load();
			Umediator.connect(UMconfig.serverIP.getValue(), std::stoi(UMconfig.serverPort.getValue()));
			std::cout << "Server Addr:\t" << UMconfig.serverIP.getValue() << "\tPort:\t" <<
			UMconfig.serverPort.getValue() << std::endl;
			Umediator.sm.joinNet();
		} catch (exceptionEx *err) {
			std::cerr << "ERROR:\n" << err->what() << std::endl;
			return -1;
		} catch (exception &err) {
			std::cerr << "UnknownERROR:\n" << err.what();
		}
	} else {
		std::cout << "Server Addr:\t" << UMconfig.serverIP.getValue() << "\tPort:\t" <<
		UMconfig.serverPort.getValue() <<
		std::endl;

		try {
			UMconfig.serverIP = argv[1];
			UMconfig.serverPort = argv[2];
			UMconfig.save();
			Umediator.connect(argv[1], std::stoi(argv[2]));
			Umediator.sm.joinNet();
		} catch (exceptionEx *err) {
			std::cerr << "ERROR:\n" << err->what();
		} catch (exception &err) {
			std::cerr << "UnknownERROR:\n" << err.what();
		}
	}

}