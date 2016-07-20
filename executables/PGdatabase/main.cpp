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

    std::string argsTemp[7];
    if (argc < 8) {
        logger.log("PGDatabase", "Invalid number of arguments provided", zeitoon::utility::LogLevel::note);
        logger.log("PGDatabase",
                   "Required arguments: ServerIP ServerPort DBUsername DBPassword DBAddress DBPort DBName",
                   zeitoon::utility::LogLevel::note);
        logger.log("PGDatabase", "Trying to load from configuration", zeitoon::utility::LogLevel::note);
        argsTemp[1] = PGconfiguration.serverIP.getValue();
        argsTemp[2] = PGconfiguration.serverPort.getValue();
        argsTemp[5] = PGconfiguration.DBserverAddr.getValue();
        argsTemp[6] = PGconfiguration.DBserverPort.getValue();
        argsTemp[3] = PGconfiguration.adminUsrPGDB.getValue();
        argsTemp[4] = PGconfiguration.adminPwPGDB.getValue();
        argsTemp[7] = PGconfiguration.PGDBName.getValue();

    }/*        PGconfiguration.serverIP = argv[1];
        PGconfiguration.serverPort = argv[2];
        PGconfiguration.DBserverAddr = argv[5];
        PGconfiguration.DBserverPort = argv[6];
        PGconfiguration.adminUsrPGDB = argv[3];
        PGconfiguration.adminPwPGDB = argv[4];
        PGconfiguration.PGDBName = argv[7];*/
    else {
        logger.log("PGDatabase", "Server Addr:   " + std::string(argv[1]) + "Port:   " + std::string(argv[2]),
                   zeitoon::utility::LogLevel::note);
        logger.log("PGDatabase", "Database Addr:   " + std::string(argv[5]) + "Port:   " + std::string(argv[6]),
                   zeitoon::utility::LogLevel::note);

        argsTemp[1] = argv[1];
        argsTemp[2] = argv[2];
        argsTemp[5] = argv[5];
        argsTemp[6] = argv[6];
        argsTemp[3] = argv[3];
        argsTemp[4] = argv[4];
        argsTemp[7] = argv[7];
    }
    try {
        PGmediator pg(argsTemp[3], argsTemp[4], argsTemp[5], std::stoi(argsTemp[6]), argsTemp[7]);
        if (not PGconfiguration.exists("networkReconnectInterval")) {
            PGconfiguration.addConfig("networkReconnectInterval", pg.sm.getDefaultNetReconnect());
            PGconfiguration.save();
        }
        pg.sm.setNetReconnectInterval(PGconfiguration.get("networkReconnectInterval"));
        pg.connect(argsTemp[1], std::stoi(argsTemp[2]));
        if (argc == 9) {
            if (std::string(argv[8]) == "-save") {
                PGconfiguration.serverIP = argsTemp[1];
                PGconfiguration.serverPort = argsTemp[2];
                PGconfiguration.DBserverAddr = argsTemp[5];
                PGconfiguration.DBserverPort = argsTemp[6];
                PGconfiguration.adminUsrPGDB = argsTemp[3];
                PGconfiguration.adminPwPGDB = argsTemp[4];
                PGconfiguration.PGDBName = argsTemp[7];
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
