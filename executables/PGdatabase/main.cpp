//
// Created by inf on 3/5/16.
//
#include <utility/exceptions.hpp>
#include "PGmediator.hpp"
#include "pgutility.hpp"
#include "PGConfig.hpp"
#include "utility/logger.hpp"

using namespace zeitoon::pgdatabase;

int main(int argc, char *argv[]) {// 1:Server IP  2:Server Port  3:
    logger.enableFile("PGLog.log");
    logger.enableTerminalOut();
    PGconfiguration.load();
    if (argc < 8) {
        logger.log("PGDatabase", "Invalid number of arguments provided", zeitoon::utility::LogLevel::note);
        logger.log("PGDatabase",
                   "Required arguments: ServerIP ServerPort DBUsername DBPassword DBAddress DBPort DBName",
                   zeitoon::utility::LogLevel::note);
        logger.log("PGDatabase", "Trying to load from configuration", zeitoon::utility::LogLevel::note);
    }
    else {
        logger.log("PGDatabase", "Server Addr:   " + std::string(argv[1]) + "Port:   " + std::string(argv[2]),
                   zeitoon::utility::LogLevel::note);
        logger.log("PGDatabase", "Database Addr:   " + std::string(argv[5]) + "Port:   " + std::string(argv[6]),
                   zeitoon::utility::LogLevel::note);

        PGconfiguration.serverIP = argv[1];
        PGconfiguration.serverPort = argv[2];
        PGconfiguration.DBserverAddr = argv[5];
        PGconfiguration.DBserverPort = argv[6];
        PGconfiguration.adminUsrPGDB = argv[3];
        PGconfiguration.adminPwPGDB = argv[4];
        PGconfiguration.PGDBName = argv[7];
    }
    try {
        PGmediator pg(PGconfiguration.adminUsrPGDB.getValue(), PGconfiguration.adminPwPGDB.getValue(),
                      PGconfiguration.DBserverAddr.getValue(), std::stoi(PGconfiguration.DBserverPort.getValue()),
                      PGconfiguration.PGDBName.getValue());
        pg.connect(PGconfiguration.serverIP.getValue(), std::stoi(PGconfiguration.serverPort.getValue()));
        if (argc == 9) {
            if (std::string(argv[8]) == "-save") {
                PGconfiguration.save();
                logger.log("PGDatabase", "Configuration saved", zeitoon::utility::LogLevel::note);
            }
        }
        pg.sm.joinNet();
    } catch (exceptionEx &err) {
        logger.log("PGDatabase", "ERROR: " + std::string(err.what()), zeitoon::utility::LogLevel::error);
    } catch (exception &err) {
        logger.log("PGDatabase", "ERROR: " + std::string(err.what()), zeitoon::utility::LogLevel::error);
    }
    return 0;
}
