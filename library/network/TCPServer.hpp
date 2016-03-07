//
// Created by ajl on 3/4/16.
//

#ifndef NETTEST_TCPSERVER_HPP
#define NETTEST_TCPSERVER_HPP

#include <string>
#include <thread>
#include <uv.h>
#include <map>
#include <utility/exceptionex.hpp>
#include <functional>

namespace zeitoon {
namespace utility {


class TCPServer {
public:
	class clientCollection {
		friend class TCPServer;

		class client {
			friend class clientCollection;

			friend class TCPServer;

		protected:

			size_t _id;
			uv_tcp_t *_client;
			std::string buff;
			TCPServer *_parent;

			client(TCPServer *parent, size_t id, uv_tcp_t *client) : _id(id), _client(client),
			                                                         _parent(parent), buff("") {
			}

		public:
			void send(std::string data);

			void stop();

			std::string getNameAndType() {
				return this->_parent->getNameAndType() + "::client[" + std::to_string(this->_id) + "]";
			}
		};

		typedef std::map<size_t, client *> ccmap;
		ccmap clients;
		TCPServer *_parent;
		size_t _curindx = 0;

		clientCollection(TCPServer *parent) {
			_parent = parent;
		}

		virtual client *add(uv_tcp_t *val) {
			size_t ni = _curindx++;
			while (clients.count(ni) > 0) { ni = _curindx++; }//make sure its unique
			client *c = new client(_parent, ni, val);
			clients[ni] = c;
			return c;
		}

		virtual void removeAt(size_t index) {
			auto i = clients.find(index);
			if (i == clients.end())
				EXToutOfRange("remove failed. id '" + std::to_string(index) + "' is not valid");
			clients.erase(index);
		}

		virtual void remove(client *val) {
			for (ccmap::iterator i = clients.begin(); i != clients.end(); i++)
				if (i->second == val) {
					clients.erase(i);
					break;
				}
		}

	public:

		virtual client *operator[](size_t index) {
			auto i = clients.find(index);
			if (i == clients.end())
				EXToutOfRange("operator[] failed. id '" + std::to_string(index) + "' is not valid");
			return i->second;
		}

		virtual size_t getIdAt(size_t index) {
			ccmap::iterator it = clients.begin();
			for (int i = 0; i < index; i++, it++);
			return it->first;
		}

		virtual size_t size() {
			return clients.size();
		}

		std::string getNameAndType() {
			return this->_parent->getNameAndType() + "::clientCollection";
		}
	};

	typedef std::function<void(size_t, std::string)> onMessageDLG;
	typedef std::function<void(size_t)> onClientConnectDLG;

	clientCollection clients;

	TCPServer(int port = -1);

	virtual ~TCPServer();

	void listen(int port = -1);

	void stop();

	void send(size_t clientId, std::string msg);

	void joinOnListenThread();

	void registerOnMessageCB(onMessageDLG cb) {
		_onMessage = cb;
	}

	void clearInMessageCB() {
		_onMessage = NULL;
	}

	void registerOnClientConnectCB(onClientConnectDLG cb) {
		_onClientConnect = cb;
	}

	void clearOnClientConnectCB() {
		_onClientConnect = NULL;
	}

	void registerOnClientDisconnectCB(onClientConnectDLG cb) {
		_onClientDisconnect = cb;
	}

	void clearOnClientDisconnectCB() {
		_onClientDisconnect = NULL;
	}

	std::string getNameAndType() {
		return std::string("TCPServer[") + std::to_string(_port) + "]";
	}

private:
	int _port;
	uv_loop_t loop;
	uv_tcp_t server;
	std::thread *listenTrd;
	onMessageDLG _onMessage;
	onClientConnectDLG _onClientConnect;
	onClientConnectDLG _onClientDisconnect;

	void _listen();

	static void on_new_connection(uv_stream_t *server, int status);

	static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

	static void on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf);

	static void on_client_write(uv_write_t *req, int status);
};


}//utility
}//zeitoon
#endif //NETTEST_TCPSERVER_HPP
