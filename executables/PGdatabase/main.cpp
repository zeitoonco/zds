//
// Created by inf on 3/5/16.
//
#include "utility/exceptionex.hpp"
#include "PGmediator.hpp"
#include "pgutility.hpp"
#include "PGConfig.hpp"

using namespace zeitoon::pgdatabase;

int main(int argc, char *argv[]) {// 1:Server IP  2:Server Port  3:
	PGconfiguration.load();
	std::string serverIP = "", serverPort = "", UsrPGDB = "", PwPGDB = "", DBAddr = "", DBPort = "", DBName = "";
	if (argc != 8) {
		std::cout << "Invalid number of arguments provided" << std::endl;
		std::cout << "Required arguments: ServerIP ServerPort DBUsername DBPassword DBAddress DBPort DBName" <<
		std::endl;
		std::cout << "Trying to load from configuration" << std::endl;
		serverIP = PGconfiguration.serverIP.getValue();
		serverPort = PGconfiguration.serverPort.getValue();
		UsrPGDB = PGconfiguration.adminUsrPGDB.getValue();
		PwPGDB = PGconfiguration.adminPwPGDB.getValue();
		DBAddr = PGconfiguration.DBserverAddr.getValue();
		DBPort = PGconfiguration.DBserverPort.getValue();
		DBName = PGconfiguration.PGDBName.getValue();

	}
	else {
		std::cout << "Server Addr:\t" << argv[1] << "\tPort:\t" << argv[2] << std::endl;
		std::cout << "Database Addr:\t" << argv[5] << "\tPort:\t" << argv[6] << std::endl;
		try {
			serverIP = argv[1];
			serverPort = argv[2];
			DBAddr = argv[5];
			DBPort = argv[6];
			UsrPGDB = argv[3];
			PwPGDB = argv[4];
			DBName = argv[7];

		} catch (exceptionEx *err) {
			std::cerr << "ERROR:\n" << err->what();
		} catch (exception &err) {
			std::cerr << "UnknownERROR:\n" << err.what();
		}
	}
	try {
		PGmediator pg(UsrPGDB, PwPGDB, DBAddr, std::stoi(DBPort), DBName);
		pg.connect(serverIP, std::stoi(serverPort));
		pg.sm.joinNet();
	} catch (exceptionEx *err) {
		std::cerr << "ERROR:\n" << err->what();
	} catch (exception &err) {
		std::cerr << "UnknownERROR:\n" << err.what();
	}
	return 0;
}
