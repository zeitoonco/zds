//
// Created by inf on 3/5/16.
//
#include "utility/exceptionex.hpp"
#include "PGmediator.hpp"
#include "pgutility.hpp"

using namespace zeitoon::pgdatabase;

int main(int argc, char *argv[]) {// 1:Server IP  2:Server Port  3:
	if (argc != 8) {
		std::cerr << "Invalid number of arguments provided\n";
		std::cerr << "Required arguments: ServerIP ServerPort DBUsername DBPassword DBAddress DBPort DBName\n";
		return -1;
	}
	std::cout << "Server Addr:\t" << argv[1] << "\tPort:\t" << argv[2] << std::endl;
	std::cout << "Database Addr:\t" << argv[5] << "\tPort:\t" << argv[6] << std::endl;
	try {
		PGmediator pg(argv[3], argv[4], argv[5], std::atoi(argv[6]), argv[7]);
		pg.connect(argv[1], std::atoi(argv[2]));
		//cout<<pg.getInstallID();
		pg.sm.joinNet();
	} catch (exceptionEx *err) {
		std::cerr << "ERROR:\n" << err->what();
	} catch (exception &err) {
		std::cerr << "UnknownERROR:\n" << err.what();
	}
}
