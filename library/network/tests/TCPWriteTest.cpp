//
// Created by ajl on 4/27/16.
//
#include <network/TCPClient.hpp>
#include <iostream>

using namespace zeitoon::utility;
using namespace std;

void onmsg(std::string msg) {
	std::cerr << "\nMSG reeceived : " << msg << std::endl;
}

int main() {
	//TCPClient a("127.0.0.1", 8787);
	TCPClient a("192.168.1.13", 5456);
	a.registerOnMessageCB(onmsg);
	a.connect();
	while (!a.isConnected()) { }
	cout << "\nConnected\n";
	std::string msg1 = "";
	std::string msg2 = "1234567890";
	//a.send(msg1);
	for (int i = 0; i < 1000000; i++) {
		a.send(to_string(i));
		//this_thread::sleep_for(chrono::milliseconds(1));
		if (i % 1000 == 0) cerr << i << "done\n";
	}
	a.joinOnConnectionThread();
	//this_thread::sleep_for(chrono::seconds(1));
	cout << "Done.";
	a.disconnect();
	return 0;
}