#include "network/UDPClient.hpp"
#include "utility/exceptionex.hpp"
#include <iostream>

using zeitoon::utility::exceptionEx;

int main() {
	char temp[60000];
	for (int i = 0; i < (60000); i++) {
		temp[i] = (char) (i % 10) + '0';
	}

	try {
		zeitoon::utility::UDPClient x;
		std::cout << "Now Sending" << endl;
		x.send("127.0.0.1", 15001, std::string(temp, 60000));
		x.disconnect(true);


		//x.send("127.0.0.1",15001,"Hellow");
		//x.listen("127.0.0.1",15001);
	} catch (exceptionEx *err) {
		std::cerr << err->what();
	}
}