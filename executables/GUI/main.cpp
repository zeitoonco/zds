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
	if (argc != 4) {
		std::cerr << "Invalid number of arguments provided\n";
		std::cerr << "Required arguments: ServerIP ServerPort WebSocketPortNo\n";
		return -1;
	}
	try {
		zeitoon::GUI::GuiCHI GUI(std::atoi(argv[3]));
		GUI.connect(argv[1], std::atoi(argv[2]));
		GUI.sm.joinNet();
	} catch (zeitoon::utility::cantParseString *err) {
		std::cout << err->what();
	}
}

