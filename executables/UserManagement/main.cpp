//
// Created by inf on 3/6/16.
//
#include "executables/UserManagement/UmCHI.hpp"

using namespace zeitoon::usermanagement;

int main(int argc, char *argv[]) {
	if (argc != 3) {
		std::cerr << "Invalid number of arguments provided\n";
		std::cerr << "Required arguments: ServerIP ServerPort\n";
		return -1;
	}
	std::cout << "Server Addr:\t" << argv[1] << "\tPort:\t" << argv[2] << std::endl;

	try {
		UmCHI Umediator;
		Umediator.connect(argv[1], std::atoi(argv[2]));
		Umediator.sm.joinNet();
	} catch (exceptionEx *err) {
		std::cerr << "ERROR:\n" << err->what();
	} catch (exception &err) {
		std::cerr << "UnknownERROR:\n" << err.what();
	}
}