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

int main(int argc, char *argv[]) {//todo : load configurations
    logger.enableFile("GUILog.log");
    logger.enableTerminalOut();
    GUIConfiguration.load();
    if (argc < 4) {
        logger.log("GUI", "Invalid number of arguments provided", zeitoon::utility::LogLevel::note);
        logger.log("GUI",
                   "Required arguments: ServerIP ServerPort WSPort", zeitoon::utility::LogLevel::note);
    } else {
        GUIConfiguration.serverIP = argv[1];
        GUIConfiguration.serverPort = argv[2];
        GUIConfiguration.WSserverPort = argv[3];
    }
    try {
        zeitoon::GUI::GuiCHI GUI(std::stoi(GUIConfiguration.WSserverPort));
        GUI.connect(GUIConfiguration.serverIP, std::stoi(GUIConfiguration.serverPort.getValue()));
        if (argc == 5) {
            if (std::string(argv[4]) == "-save") {
                GUIConfiguration.save();
                logger.log("GUI", "Configuration saved", zeitoon::utility::LogLevel::note);

            }
        }
        GUI.sm.joinNet();
    } catch (zeitoon::utility::cantParseString &err) {
        logger.log("GUI", "ERROR: " + std::string(err.what()), zeitoon::utility::LogLevel::error);
    }
}

