//
// Created by ajl on 4/27/16.
//

#include <network/TCPServer.hpp>

using namespace zeitoon::utility;

void onmsg(size_t id, std::string msg);

TCPServer a(8787);
int counter=0;
int main() {
	try {
		a.listen();
		a.registerOnMessageCB(onmsg);
		//a.joinOnListenThread();
while (true){cin.get();cout<<counter<<"#";}

		a.stop();
	} catch (exceptionEx *e) {
		cerr<<"\nERRx:"<<e->what();
	} catch (exception &e) {
		cerr<<"\nERR:"<<e.what();
	}

	return 0;
}

void onmsg(size_t id, std::string msg) {
	counter++;
	std::cerr << "\n"<<to_string(counter)+"MSG " << id << " l " << msg.size() << "\n" <<
	(msg.size() > 40 ? msg.substr(0, 40) + "....." : msg);
}