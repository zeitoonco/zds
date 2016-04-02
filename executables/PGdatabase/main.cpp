//
// Created by inf on 3/5/16.
//
#include "utility/exceptionex.hpp"
#include "PGmediator.hpp"
#include "pgutility.hpp"
#include "PGConfig.hpp"

using namespace zeitoon::pgdatabase;

int main(int argc, char *argv[]) {// 1:Server IP  2:Server Port  3:
	if (argc != 8) {
		std::cout << "Invalid number of arguments provided" << std::endl;
		std::cout << "Required arguments: ServerIP ServerPort DBUsername DBPassword DBAddress DBPort DBName" <<
		std::endl;
		std::cout << "Trying to load from configuration" << std::endl;
		try {
			PGconfiguration.load();
			PGmediator pg(PGconfiguration.adminUsrPGDB.getValue(), PGconfiguration.adminPwPGDB.getValue(),
			              PGconfiguration.DBserverAddr.getValue(), std::stoi(PGconfiguration.DBserverPort.getValue()),
			              PGconfiguration.PGDBName.getValue());
			pg.connect(PGconfiguration.serverIP.getValue(), std::stoi(PGconfiguration.serverPort.getValue()));
			pg.sm.joinNet();
		} catch (exceptionEx *err) {
			std::cerr << "ERROR:\n" << err->what();
			return -1;
		} catch (exception &err) {
			std::cerr << "UnknownERROR:\n" << err.what();
		}

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
			PGconfiguration.save();
			PGmediator pg(argv[3], argv[4], argv[5], std::atoi(argv[6]), argv[7]);
			pg.connect(argv[1], std::atoi(argv[2]));
			pg.sm.joinNet();
		} catch (exceptionEx *err) {
			std::cerr << "ERROR:\n" << err->what();
		} catch (exception &err) {
			std::cerr << "UnknownERROR:\n" << err.what();
		}
	}
	return 0;
}
