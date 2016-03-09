/*
 * main.cpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */
#include "utility/exceptionex.hpp"
#include "GuiUtility.hpp"
#include "GuiCHI.hpp"

int main(int argc, char *argv[]) {
	if (argc != 5) {
		std::cerr << "Invalid number of arguments provided\n";
		std::cerr << "Required arguments: ServerIP ServerPort FileSystemBaseDir WebSocketPortNo\n";
		return -1;
	}
	try {
		zeitoon::GUI::GuiCHI GUI(argv[1], std::atoi(argv[2]), argv[3], std::atoi(argv[4]));
		GUI.sm.joinNet();
	} catch (zeitoon::utility::cantParseString *err) {
		std::cout << err->what();
	}
}

