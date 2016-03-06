//
// Created by inf on 3/5/16.
//
#include "library/utility/exceptionex.hpp"
#include "PGmediator.hpp"
#include "pgutility.hpp"

using namespace zeitoon::pgdatabase;

//todo: Avoid termination of this program while ...
int main(int argc, char *argv[]) {
	std::cout << "Server Addr:\t" << argv[1] << "\tPort:\t" << argv[2] << std::endl;
	std::cout << "Database Addr:\t" << argv[5] << "\tPort:\t" << argv[6] << std::endl;
	try {
		PGmediator dummy(argv[1], std::atoi(argv[2]), argv[3], argv[4], argv[5], std::atoi(argv[6]), argv[7]);
	} catch (exceptionEx *err) {
		std::cerr << err->what();
	}
}
