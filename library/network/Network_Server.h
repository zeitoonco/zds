#ifndef NETWORK_SERVER_H_
#define NETWORK_SERVER_H_

/**
 * dar ghesmate include kardan, library haaye marboot be boost.asio ast raa include mikonim choon az in ketabkhoone baraye ijaade server va
 * client estefade mikonim.
 */
#include <iostream>
#include <boost/asio.hpp>
#include <string.h>
#include <boost/thread.hpp>
#include <boost/array.hpp>

namespace zeitoon {
namespace utility {

using namespace std;

using boost::asio::io_service;
using boost::asio::ip::tcp;

/**class NetClient in kelass etelaate marboot be server yaa client ra dar khod zakhire mikonad. in etelaat shaamele name server ya client,
 * ip va port aan ast.
 *
 */
class NetClient {

public:
	string name;
	string ip;
	int port;
	tcp::socket *socket;
};

//typedef void (*receiveFuncP)(NetClient*, string);/**< define pointer to function */
typedef void (*receiveFuncP)(void*, NetClient*, string);

/**class NetworkHub
 *
 * dar in class piadesaazie ijaade yek server gharar darad. in serever be soorate tcp ba client haayei ke be aan connect mishavand ertebat
 * bargharar mikonad va hamchin tcp version4 ra support mikonad.
 *
 */
class NetworkHub {

private:
	vector<NetClient> clients;/**< dar in vector listi az tamame clienthaayi ke be server motasel hastnad gharar darand */
	int listenPort;/**< shomare porti ke server ijaad mishavad */
	boost::asio::io_service io_service;/**< baraye estefade az ketabkhane boost.asio, bayad yek io_service sakht ta in io_service polli beine tavabe e libarary ba system bashad */
	tcp::acceptor acceptor;/**< baraye connect kardan va ghabool kardn etesaale client jadid be server */
	bool isListen;
	receiveFuncP receiveFunc;/**< dar in moteghaeir, functioni ke az client daryaaft mishavad ra zakhire mikonad. in function neshan midahad ke data be kodaam fanction bayad ersal shavad*/
	void* receiveFuncOwner;

public:

	/**constructor baraye NetworkHub
	 *
	 * shomare porti ke bayad dar aan server sakhte shvad ra daryaft mikonad va acceptor ra meghdardehi mikonad ke bayad be aan io_service va
	 * endpointi ke dar aan etelaate server gharar darad bgirad. moteghaeir isListn be soorate pishfarz true dar nazar grefte mishavad.
	 *
	 * @param port porte server
	 *
	 */
	NetworkHub(int port) :
			listenPort(port), acceptor(io_service,
					tcp::endpoint(tcp::v4(), listenPort)), isListen(true), receiveFunc(NULL) {
	}

	/**read() baraye khandane data az client...
	 *
	 * har clienti ke dar taabe listen() be server vasl mishavad, pas az connect shodan, taabe read dar yek thread seda zade mishavad va ta vaghti
	 * ke server isListrening ast ya client be server coonect ast in thread kar mikonad va server montazere daryafte data az client ast.
	 * pas inke data ra az client daryaft kard aan ra be taabe receivefunc mifrestad ta moshakhs shavad ke in data bayad be kodam taabe ersal shavad.
	 *
	 * @param temp NetCleint ke moshakhasaate clienti ke be server vasl shode ast ra dar khod zakhire karde ast.
	 *
	 */
	void read(NetClient& temp) {
		while (isListen) {
			boost::array<char, 10240> buf;
			boost::system::error_code error;
			size_t len = temp.socket->read_some(boost::asio::buffer(buf),
					error);
			if (error == boost::asio::error::eof)
				break;
			else if (error)
				throw boost::system::system_error(error);

			if (receiveFunc != NULL) {
				if (receiveFuncOwner != NULL)
					receiveFunc(receiveFuncOwner, &temp, string(buf.data(), len)); //TODO:run on new thread
				else
					receiveFunc(NULL, &temp, string(buf.data(), len)); //TODO:run on new thread
			}
			//receivefunc(&temp, string(buf.data(), len));
		}
	}

	/**listen()
	 *
	 * in taabe montazer ta vaghti server isListening ast run mishavad va montazer mimanad ta client request bedahad, sepas darkhast ra mipazirad
	 * va client be server vasl mishavad va client ra dar vector gharar midahad va yek thread ke dar aan taabe read ra seda mizanad ijad mikonad
	 * ta server daemn dar haale read kardn az clienti ke be aan vasl shode ast bashad.
	 *
	 */
	void listen() {
		isListen = true;
		while (isListen) {
			tcp::socket *socket = new tcp::socket(io_service);
			acceptor.accept(*socket);
			NetClient temp;
			temp.socket = socket;
			clients.push_back(temp);
			boost::thread readThread(&NetworkHub::read, this, temp);
		}
	}

	/**isListening() vaziate listening boodn server neshan midahad.
	 *
	 */
	bool isListening() {
		return isListen;
	}

	/**stop() faaliate server ra ghat mikonad.
	 *
	 */
	void stop() {
		isListen = false;
	}

	/**send() data ra be clienti ke daryaft mikonad miferstd.
	 *
	 * @param data daade iee ke bayad be client ersaal shavad.
	 * @param temp clienti ke data bayad be aan ersaal shavad.
	 *
	 */
	void send(NetClient& temp, string data) {
		boost::system::error_code ignored_error;
		boost::asio::write(*(temp.socket), boost::asio::buffer(data),
				ignored_error);
	}

	/**send() data ra be clienti ke daryaft mikonad miferstd.
	 *
	 * @param data daade iee ke bayad be client ersaal shavad.
	 * @param temp clienti ke data bayad be aan ersaal shavad.
	 * @param size size e data ra moshakhas mikonad.
	 *
	 */
	void send(NetClient& temp, const char* data, size_t size) {
		boost::system::error_code ignored_error;
		boost::asio::write(*(temp.socket), boost::asio::buffer(data, size),
				ignored_error);
	}

	/**registerReceiveFunc functioni ke migirad ra mosavi functione class gharar midahad.
	 *
	 */
	void registerReceiveFunc(void * owner, receiveFuncP func) {
		receiveFunc = func;
		receiveFuncOwner = owner;
	}

};

}
}
#endif /* NETWORK_SERVER_H_ */
