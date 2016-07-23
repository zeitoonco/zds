//
// Created by inf on 3/6/16.
//
#include "executables/UserManagement/UmCHI.hpp"

using namespace zeitoon::usermanagement;


int main(int argc, char *argv[]) {
    logger.enableFile("UMLog.log");
    logger.enableTerminalOut();

    UmCHI Umediator;
    UMconfig.load();
    if (not UMconfig.exists("networkReconnectInterval")) {
        UMconfig.addConfig("networkReconnectInterval", Umediator.sm.getDefaultNetReconnect());
        UMconfig.save();
    }
    Umediator.sm.setNetReconnectInterval(UMconfig.get("networkReconnectInterval"));
    if (argc < 3 && argc > 1) {
        logger.log("UserMAnagement", "Invalid number of arguments provided", zeitoon::utility::LogLevel::note);
        logger.log("UserMAnagement", "Required arguments: ServerIP ServerPort", zeitoon::utility::LogLevel::note);
        return -1;
    } else if (argc == 3) {
        UMconfig.serverIP = argv[1];
        UMconfig.serverPort = argv[2];
    }else if(argc ==1){
        logger.log("UserMAnagement", "No Parameter provided. Trying to load from configuration", zeitoon::utility::LogLevel::note);
    }


    try {
        Umediator.connect(UMconfig.serverIP.getValue(), std::stoi(UMconfig.serverPort.getValue()));
        logger.log("UserMAnagement",
                   "Server Addr:  " + UMconfig.serverIP.getValue() + "      Port:  " + UMconfig.serverPort.getValue(),
                   zeitoon::utility::LogLevel::note);
        if (argc == 4) {
            if (std::string(argv[3]) == "-save") {
                UMconfig.save();
                logger.log("UserMAnagement", "Configuration saved", zeitoon::utility::LogLevel::note);
            }
        }
        Umediator.sm.joinNet();
    } catch (exceptionEx &err) {
        logger.log("UserMAnagement", "ERROR: " + std::string(err.what()), zeitoon::utility::LogLevel::error);
        //std::cout << "ERROR:\n" << err.what() << endl;
    } catch (exception &err) {
        // std::cout << "UnknownERROR:\n" << err.what() << std::endl;
        logger.log("UserMAnagement", "UnknownERROR: " + std::string(err.what()), zeitoon::utility::LogLevel::error);
    }

}