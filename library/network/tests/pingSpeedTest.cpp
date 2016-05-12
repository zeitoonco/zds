#include <network/TCPClient.hpp>
#include <iostream>
#include <chrono>
#include <map>

using namespace zeitoon::utility;
using namespace std;

struct ping {
	int id;
	chrono::system_clock::time_point t;
};
typedef map<int, ping *> pmap;
pmap pp;

void onmsg(std::string msg) {
	int id = stoi(msg);
	ping *p = pp[id];
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(chrono::system_clock::now() - p->t);
	cerr << "\nP " << id << " " << dur.count();
}

int main() {
	TCPClient a("192.168.1.103", 5456);
	a.registerOnMessageCB(onmsg);
	a.connect();
	cout << "\nConnected\nN,D:";
	int n, d;
	cin >> n >> d;
	for (int i = 0; i < n; i++) {
		ping *p = new ping{i, chrono::system_clock::now()};
		pp[i] = p;
		a.send(to_string(i));
		this_thread::sleep_for(chrono::milliseconds(d));
	}


	a.disconnect();
	return 0;
}