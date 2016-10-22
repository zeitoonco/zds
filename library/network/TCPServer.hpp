//
// Created by ajl on 3/4/16.
//

#ifndef NETTEST_TCPSERVER_HPP
#define NETTEST_TCPSERVER_HPP

#include <iostream>
#include <string>
#include <thread>
#include <uv.h>
#include <map>
#include <utility/exceptions.hpp>
#include <functional>
#include <mutex>
#include <queue>
#include "utility/logger.hpp"
#include <condition_variable>
#include <atomic>

namespace zeitoon {
namespace utility {

using namespace std;

class TCPServer {
	void startDataTransmission();

	void stopDataTransmission();

public:
	void rxThread();

	void txThread();

	void txThreadMgr();

	void rxThreadMgr();

	void rxThreadMaker(int numberOfThreads = 1);

	void txThreadMaker(int numberOfThreads = 1);

	void _packetReceived(size_t &clientID);

	class clientCollection {
		friend class TCPServer;

	public:
		class client {
			friend class clientCollection;

			friend class TCPServer;

		protected:
			std::atomic<bool> send_busy;

			size_t _id;
			uv_tcp_t *_client;
			TCPServer *_parent;
			bool _isConnected;

			client(TCPServer *parent, size_t id, uv_tcp_t *client) :
					_id(id), _client(client), _parent(parent), _isConnected(false), send_busy(false) { }

		public:

			void send(std::string data);

			void stop();

			bool isConnected() {
				return _isConnected;
			}

			std::string getNameAndType() {
				return this->_parent->getNameAndType() + "::client[" + std::to_string(this->_id) + "]";
			}
		};

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

	private:
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
			client *c = new client(_parent, ni, val);//make sure to deallocate
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
					free(i->second);//newly added
					clients.erase(i);
					break;
				}
		}

	};

	typedef std::function<void(size_t, std::string)> onMessageDLG;
	typedef std::function<void(size_t)> onClientConnectDLG;
	typedef std::function<void()> onConnectDLG;

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

	void registerOnConnectCB(onConnectDLG cb) {
		_onConnect = cb;
	}

	void clearOnConnectCB() {
		_onConnect = NULL;
	}

	void registerOnDisconnectCB(onConnectDLG cb) {
		_onDisconnect = cb;
	}

	void clearOnDisconnectCB() {
		_onDisconnect = NULL;
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
	onConnectDLG _onConnect;
	onConnectDLG _onDisconnect;
	uv_timer_t flushTimer;

	void _listen();

	void _safeCaller(size_t id, std::string data);

	static void on_new_connection(uv_stream_t *server, int status);

	static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

	static void on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf);

	static void on_client_write(uv_write_t *req, int status);

	static void dataProcThreadMgrTimer(uv_timer_t *handle);

	static void keepAliveTimerCB(uv_timer_t *handle) {
		logger.flush();
	}
};


}//utility
}//zeitoon
#endif //NETTEST_TCPSERVER_HPP
