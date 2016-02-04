#ifndef NETWORK_CLIENT_H_
#define NETWORK_CLIENT_H_

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

typedef void (*receiveFuncP)(void*, string);/**< define pointer to function */

/**class NetClient in kelass etelaate marboot be server yaa client ra dar khod zakhire mikonad. in etelaat shaamele name server ya client,
 * ip va port aan ast.
 *
 */
class NetClient {
public:
	string name;
	string ip;
	int port;
};

/**class NetworkHub_Client
 *
 * dar injaa piadezasie marboot be ijaade yek client gharar darad. cliente ijaad shode be soorate tcp be server connect
 * mishavad va data ra mifrestad. in client tcp version4 ra support mikonad.
 *
 */
class NetworkHub_Client {
private:
	NetClient server;/**< etelaate marboot be server ke be aan vasl mishavad dar in moteghaeir zakhire mishavad*/
	boost::asio::io_service io_service;/**< baraye estefade az ketabkhane boost.asio, bayad yek io_service sakht ta in io_service polli beine tavabe e libarary ba system bashad */
	tcp::socket socket;
	bool connected;
	void* receiveFuncOwner;
	receiveFuncP receiveFunc;/**< dar in moteghaeir, functioni ke az server daryaaft mishavad ra zakhire mikonad. in function neshan midahad ke data be kodaam fanction bayad ersal shavad*/

public:

	/**constructor baraye NetworkHub_Client
	 *
	 * dar in constructor socket meghdardehi mishavad va meghdare connected false darnazar grefte mishavad.
	 *
	 */
	NetworkHub_Client() :
			socket(io_service), connected(false) {
	}
	/**connect() piadezasi e connect shodan be server morede nazar.
	 *
	 * hengaami ke client be serveri ke dar private meghdardehi shode ast vasl shod, yek thread sakhte mishavad ta
	 * taabe read ra ejraa konad va taabe read ta vaghti connect ast run mishavad va motazere daryaft data az server
	 * ast.
	 *
	 */
	void connect() {
		tcp::resolver resolver(io_service);
		stringstream str;
		str << server.port;
		const char* port = str.str().c_str();
		tcp::resolver::query query(server.ip.c_str(), port);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		boost::asio::connect(socket, endpoint_iterator);
		connected = true;
		boost::thread readThread(&NetworkHub_Client::read, this);
	}
	/**read() baraye khandane data az server...
	 *
	 *in taabe ta vaghti client be server connect bashad run mishavad va data az server migirad va be taabe
	 *in receiveFunc mifrestad ta data be functioni ke bayad ersaal shavad, ersaal shavad.
	 *
	 */

	void read() {
		while (connected) {
			boost::system::error_code error;
			boost::array<char, 10240> buf;
			size_t len = socket.read_some(boost::asio::buffer(buf), error);
			if (receiveFunc != NULL && receiveFuncOwner != NULL)
				receiveFunc(receiveFuncOwner, string(buf.data(), len));
			//receiveFunc(string(buf.data(), len));

		}
	}

	/**connect() dar injaa ebteda moshakhaste serveri ke bayad client be aan vasl shavad daryaft mishavad va sepas taabe connect seda zade mishavad.
	 *
	 */
	void connect(NetClient temp) {
		server = temp;
		this->connect();
	}

	/**disconnect() ertebaat ba server ra ghat mikonad.
	 *
	 */
	void disconnect() {
		connected = false;
		socket.close();
	}

	/**isConnected() vasiate connect boodan be server ra moshakhas mikonad.
	 *
	 * @return true or false.
	 *
	 */
	bool isConnected() {
		return connected;
	}

	/**send() datayii ra ke migirad ra be server mifrestad.
	 *
	 *@param data stringi ke bayad ersal shavad.
	 *
	 */
	void send(string data) {
		boost::asio::write(socket, boost::asio::buffer(data));
	}

	/**send() datayii ra ke migirad ra be server mifrestad.
	 *
	 *@param data daade ie ke bayad ersal shavad.
	 *@param size size e daade
	 *
	 */
	void send(const char* data, size_t size) {
		boost::asio::write(socket, boost::asio::buffer(data, size));
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
#endif /* NETWORK_CLIENT_H_ */
