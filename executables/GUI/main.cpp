/*
 * main.cpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */
#include <utility/exceptions.hpp>
#include "GuiUtility.hpp"
#include "GuiCHI.hpp"

#include "GUIConfig.hpp"
#include "utility/logger.hpp"

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
    logger.enableFile("GUILog.log");
    logger.enableTerminalOut();
    GUIConfiguration.load();
    std::string argsTemp[3];
/*if (argc < 3 || argc > 1) {
		std::cout << "Invalid number of arguments provided" << std::endl;
		std::cout << "Required arguments: ServerIP ServerPort" << std::endl;
		return -1;
	} else if(argc == 0){
		std::cout << "Trying */
    if (argc < 4 && argc > 1) {
        logger.log("GUI", "Invalid number of arguments provided", zeitoon::utility::LogLevel::note);
        logger.log("GUI",
                   "Required arguments: ServerIP ServerPort WSPort", zeitoon::utility::LogLevel::note);
      return -1;
    } else if (argc >= 4){
        argsTemp[0] = argv[1];
        argsTemp[1] = argv[2];
        argsTemp[2] = argv[3];
    }else if(argc == 1){
        logger.log("GUI", "No Parameter provided. Trying to load from configuration" , zeitoon::utility::LogLevel::note);
        argsTemp[0] = GUIConfiguration.serverIP.getValue();
        argsTemp[1] = GUIConfiguration.serverPort.getValue();
        argsTemp[2] = GUIConfiguration.WSserverPort.getValue();
    }

    try {
        zeitoon::GUI::GuiCHI GUI(std::stoi(argsTemp[2]));
        if (not GUIConfiguration.exists("networkReconnectInterval")){
            GUIConfiguration.addConfig("networkReconnectInterval", GUI.sm.getDefaultNetReconnect());
            GUIConfiguration.save();
        }
        GUI.sm.setNetReconnectInterval(GUIConfiguration.get("networkReconnectInterval"));
        GUI.connect(argsTemp[0], std::stoi(argsTemp[1]));
        if (argc == 5) {
            if (std::string(argv[4]) == "-save") {
                GUIConfiguration.serverIP = argv[1];
                GUIConfiguration.serverPort = argv[2];
                GUIConfiguration.WSserverPort = argv[3];
                GUIConfiguration.save();
                logger.log("GUI", "Configuration saved", zeitoon::utility::LogLevel::note);

            }
        }
        GUI.sm.joinNet();
    } catch (zeitoon::utility::cantParseString &err) {
        logger.log("GUI", "ERROR: " + std::string(err.what()), zeitoon::utility::LogLevel::error);
    }
}

