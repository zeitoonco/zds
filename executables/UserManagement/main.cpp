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
    // std::string serverIP = "", serverPort = "";
    if (argc < 3) {
        logger.log("UserMAnagement", "Invalid number of arguments provided", zeitoon::utility::LogLevel::note);
        logger.log("UserMAnagement", "Required arguments: ServerIP ServerPort", zeitoon::utility::LogLevel::note);
        logger.log("UserMAnagement", "Trying to load from configuration", zeitoon::utility::LogLevel::note);

    } else {
        UMconfig.serverIP = argv[1];
        UMconfig.serverPort = argv[2];
    }


    try {
        Umediator.connect(UMconfig.serverIP.getValue(), std::stoi(UMconfig.serverPort.getValue()));
        logger.log("UserMAnagement",
                   "Server Addr:  " + UMconfig.serverIP.getValue() + "      Port:  " + UMconfig.serverPort.getValue(),
                   zeitoon::utility::LogLevel::note);
        //  std::cout << "Server Addr:\t" << UMconfig.serverIP.getValue() << "\tPort:\t" <<
      //  UMconfig.serverPort.getValue() << std::endl;
        if (argc == 4) {
            if (std::string(argv[3]) == "-save") {
                UMconfig.save();
                logger.log("UserMAnagement", "Configuration saved", zeitoon::utility::LogLevel::note);
            }
        }
        Umediator.sm.joinNet();
    } catch (exceptionEx &err) {
        logger.log("UserMAnagement",  "ERROR: " + std::string(err.what()) , zeitoon::utility::LogLevel::error);
       //std::cout << "ERROR:\n" << err.what() << endl;
    } catch (exception &err) {
       // std::cout << "UnknownERROR:\n" << err.what() << std::endl;
        logger.log("UserMAnagement",  "UnknownERROR: " + std::string(err.what()) , zeitoon::utility::LogLevel::error);
    }

}