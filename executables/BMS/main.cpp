/*
 * main.cpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */
#include <utility/exceptions.hpp>
#include "localNetUDP.hpp"
#include "HDLGate.hpp"


using namespace zeitoon::BMS;
using namespace zeitoon::utility;

int main(int argc, char *argv[]) {
	/*if (argc != 4) {
		std::cerr << "Invalid number of arguments provided\n";
		std::cerr << "Required arguments: ServerIP ServerPort WebSocketPortNo\n";
		return -1;
	}*/
		localNetUDP udp(6000);
		HDLOneIP hdlO(udp,"192.168.1.111");


}

