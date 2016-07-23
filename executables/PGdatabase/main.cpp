//
// Created by inf on 3/5/16.
//
#include <utility/exceptions.hpp>
#include "PGmediator.hpp"
#include "pgutility.hpp"
#include "PGConfig.hpp"
#include "utility/logger.hpp"

using namespace zeitoon::pgdatabase;
void signalHandler(int s) {
    logger.log("MAIN", "Caught signal " + to_string(s), LogLevel::fatal);
    logger.flush();
    exit(1);
}

int main(int argc, char *argv[]) {
    //handle signals > ctrl+C
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signalHandler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    logger.enableFile("PGLog.log");
    logger.enableTerminalOut();
    PGconfiguration.load();

    std::string argsTemp[7];
    if (argc < 8 && argc >1) {
        logger.log("PGDatabase", "Invalid number of arguments provided", zeitoon::utility::LogLevel::note);
        logger.log("PGDatabase",
                   "Required arguments: ServerIP ServerPort DBUsername DBPassword DBAddress DBPort DBName",
                   zeitoon::utility::LogLevel::note);
        return -1;
    }
    if(argc ==1){
        logger.log("PGDatabase", "No Parameter provided. Trying to load from configuration", zeitoon::utility::LogLevel::note);
        argsTemp[0] = PGconfiguration.serverIP.getValue();
        argsTemp[1] = PGconfiguration.serverPort.getValue();
        argsTemp[4] = PGconfiguration.DBserverAddr.getValue();
        argsTemp[5] = PGconfiguration.DBserverPort.getValue();
        argsTemp[2] = PGconfiguration.adminUsrPGDB.getValue();
        argsTemp[3] = PGconfiguration.adminPwPGDB.getValue();
        argsTemp[6] = PGconfiguration.PGDBName.getValue();
    }
    else if(argc == 8){
        argsTemp[0] = argv[1];
        argsTemp[1] = argv[2];
        argsTemp[4] = argv[5];
        argsTemp[5] = argv[6];
        argsTemp[2] = argv[3];
        argsTemp[3] = argv[4];
        argsTemp[6] = argv[7];
    }

    try {
        PGmediator pg(argsTemp[2], argsTemp[3], argsTemp[4], std::stoi(argsTemp[5]), argsTemp[6]);
        if (not PGconfiguration.exists("networkReconnectInterval")) {
            PGconfiguration.addConfig("networkReconnectInterval", pg.sm.getDefaultNetReconnect());
            PGconfiguration.save();
        }
        pg.sm.setNetReconnectInterval(PGconfiguration.get("networkReconnectInterval"));
        pg.connect(argsTemp[0], std::stoi(argsTemp[1]));
        logger.log("PGDatabase", "Server Addr:   " + std::string(argsTemp[0]) + "Port:   " + std::string(argsTemp[1]),
                   zeitoon::utility::LogLevel::note);
        logger.log("PGDatabase", "Database Addr:   " + std::string(argsTemp[4]) + "Port:   " + std::string(argsTemp[5]),
                   zeitoon::utility::LogLevel::note);
        if (argc == 9) {
            if (std::string(argv[8]) == "-save") {
                PGconfiguration.serverIP = argsTemp[0];
                PGconfiguration.serverPort = argsTemp[1];
                PGconfiguration.DBserverAddr = argsTemp[4];
                PGconfiguration.DBserverPort = argsTemp[5];
                PGconfiguration.adminUsrPGDB = argsTemp[2];
                PGconfiguration.adminPwPGDB = argsTemp[3];
                PGconfiguration.PGDBName = argsTemp[6];
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
