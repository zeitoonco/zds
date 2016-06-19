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

int main(int argc, char *argv[]) {//todo : load configurations
	GUIConfiguration.load();
	if (argc < 4) {
		std::cerr << "Invalid number of arguments provided\n";
		std::cerr << "Required arguments: ServerIP ServerPort WebSocketPortNo\n";
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
				std::cerr << "Configuration saved\n";
			}
		}
		GUI.sm.joinNet();
	} catch (zeitoon::utility::cantParseString *err) {
		std::cout << err->what();
	}
}

