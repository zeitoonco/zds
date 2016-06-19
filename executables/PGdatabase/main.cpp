//
// Created by inf on 3/5/16.
//
#include <utility/exceptions.hpp>
#include "PGmediator.hpp"
#include "pgutility.hpp"
#include "PGConfig.hpp"

using namespace zeitoon::pgdatabase;

int main(int argc, char *argv[]) {// 1:Server IP  2:Server Port  3:
	PGconfiguration.load();
	std::string serverIP = "", serverPort = "", UsrPGDB = "", PwPGDB = "", DBAddr = "", DBPort = "", DBName = "";
	if (argc < 8) {
		std::cout << "Invalid number of arguments provided" << std::endl;
		std::cout << "Required arguments: ServerIP ServerPort DBUsername DBPassword DBAddress DBPort DBName" <<
		std::endl;
		std::cout << "Trying to load from configuration" << std::endl;
		/*	serverIP = PGconfiguration.serverIP.getValue();
			serverPort = PGconfiguration.serverPort.getValue();
			UsrPGDB = PGconfiguration.adminUsrPGDB.getValue();
			PwPGDB = PGconfiguration.adminPwPGDB.getValue();
			DBAddr = PGconfiguration.DBserverAddr.getValue();
			DBPort = PGconfiguration.DBserverPort.getValue();
			DBName = PGconfiguration.PGDBName.getValue();*/

	}
	else {
		std::cout << "Server Addr:\t" << argv[1] << "\tPort:\t" << argv[2] << std::endl;
		std::cout << "Database Addr:\t" << argv[5] << "\tPort:\t" << argv[6] << std::endl;
		try {
			PGconfiguration.serverIP = argv[1];
			PGconfiguration.serverPort = argv[2];
			PGconfiguration.DBserverAddr = argv[5];
			PGconfiguration.DBserverPort = argv[6];
			PGconfiguration.adminUsrPGDB = argv[3];
			PGconfiguration.adminPwPGDB = argv[4];
			PGconfiguration.PGDBName = argv[7];

		} catch (exceptionEx &err) {
			std::cerr << "ERROR:\n" << err.what();
		} catch (exception &err) {
			std::cerr << "UnknownERROR:\n" << err.what();
		}
	}
	try {
		PGmediator pg(PGconfiguration.adminUsrPGDB.getValue(), PGconfiguration.adminPwPGDB.getValue(),
		              PGconfiguration.DBserverAddr.getValue(), std::stoi(PGconfiguration.DBserverPort.getValue()),
		              PGconfiguration.PGDBName.getValue());
		pg.connect(PGconfiguration.serverIP.getValue(), std::stoi(PGconfiguration.serverPort.getValue()));
		if (argc == 9) {
			if (std::string(argv[8]) == "-save") {
				PGconfiguration.save();
				std::cerr << "Configuration saved\n";
			}
		}
		pg.sm.joinNet();
	} catch (exceptionEx &err) {
		std::cerr << "ERROR:\n" << err.what();
	} catch (exception &err) {
		std::cerr << "UnknownERROR:\n" << err.what();
	}
	return 0;
}
