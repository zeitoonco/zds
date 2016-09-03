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

namespace zeitoon {
namespace utility {

using namespace std;

class TCPServer {


public:
	class clientCollection {
		friend class TCPServer;

	public:
		class client {
			friend class clientCollection;

			friend class TCPServer;

		protected:

			size_t _id;
			uv_tcp_t *_client;
/*			std::string _buff;
			size_t _lastPacketLen;*/
			TCPServer *_parent;
			bool _isConnected;

			client(TCPServer *parent, size_t id, uv_tcp_t *client) :
					_id(id), _client(client), _parent(parent), /*_buff(""), _lastPacketLen(0), */_isConnected(false) { }


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

	class Transmiter {


	public:
		std::queue<std::pair<size_t, std::string>> pendingBuffs;
		std::queue<std::pair<size_t, std::string>> receivedDataQ;
		std::string rxBuff;
		size_t rxLastDataQSize = 0, txLastDataQSize = 0, _lastPacketLen;
		Transmiter(zeitoon::utility::TCPServer *iparentClass) : parentClass(iparentClass),rxBuff("") {
		}

		~Transmiter() {
			stopTransmission();
			freeThreadPool();
		}

		void startTransmission() {
			std::cerr<<"TCP trasmitter started\n";

			this->startReceive();
			this->startSend();
		}

		void stopTransmission() {
			this->stopSend();
			this->stopReceive();
			freeThreadPool();
		}

		std::string getNameAndType() {
			return "TCPClient::dataTransmiter";
		}


	private:

		TCPServer *parentClass;
		std::mutex rxMtx, txMtx;
		std::vector<std::thread *> rxDataThreadPool;
		std::vector<std::thread *> txDataThreadPool;
		uv_timer_t rxTimer;
		uv_timer_t txTimer;
		int rxDataQ_Pops = 0, rxDataQ_Pushes = 0;
		int rxCheck2 = 0;
		bool __StopRx;
		bool __StopTx;
		int txDataQ_Pops = 0, txDataQ_Pushes = 0;
		int txCheck2 = 0;

		void rxTrdMaker(int threads) {
			//std::cerr<<"TCP-RX THrd Maker\n";
			for (int i = 0; i < threads; i++) {
				std::thread *temp = new std::thread(&zeitoon::utility::TCPServer::Transmiter::rxProcessor, this);
				rxDataThreadPool.push_back(temp);
			}
		}

		void txTrdMaker(int threads) {
			//std::cerr<<"TCP-TX THrd Maker\n";
			for (int i = 0; i < threads; i++) {
				std::thread *temp = new std::thread(&zeitoon::utility::TCPServer::Transmiter::txProcessor, this);
				txDataThreadPool.push_back(temp);
			}
		}

		void txProcessor() {
			std::cerr<<"Starting TCP-TX\n";
			std::unique_lock<std::mutex> lck(txMtx);
			lck.unlock();
			while (not __StopTx) {
				//std::cerr<< "TX\n";
				lck.lock();
				if (pendingBuffs.size() > 0) {
					auto temp = this->pendingBuffs.front();
					this->pendingBuffs.pop();
					this->txDataQ_Pops++;
					lck.unlock();
					this->UVsnd(temp.first, temp.second);
				} else {
					lck.unlock();
					std::this_thread::sleep_for(std::chrono::microseconds(1));
				}
			}
		}

		void rxProcessor() {
			std::cerr<<"Starting TCP-RX\n";
			std::unique_lock<std::mutex> lck(rxMtx);
			lck.unlock();
			while (not __StopRx) {
				//std::cerr<< "RX\n";
				lck.lock();
				if (receivedDataQ.size() > 0) {
					auto temp = this->receivedDataQ.front();
					this->receivedDataQ.pop();
					this->rxDataQ_Pops++;
					lck.unlock();
					this->parentClass->_safeCaller(temp.first, temp.second);
				} else {
					lck.unlock();
					std::this_thread::sleep_for(std::chrono::microseconds(1));
				}
			}
		}

		static void rxThreadMgr(uv_timer_t *handle) {
			//std::cerr<<"TCP-RX Thread Mgr\n";

			TCPServer::Transmiter *c = (TCPServer::Transmiter *) handle->data;
			if (c->rxDataQ_Pops == 0 && c->rxLastDataQSize > 0) {
				c->rxTrdMaker(1);
				c->rxCheck2 = 0;
			} else if (c->rxDataQ_Pushes > c->rxDataQ_Pops) {
				if (c->rxCheck2 == 5) {
					c->rxTrdMaker(1);
					c->rxCheck2 = 0;
				} else {
					c->rxCheck2++;
				}
			} else if (c->rxDataQ_Pops > c->rxDataQ_Pushes &&
			           c->rxDataThreadPool.size() > 4) {
				c->rxDataThreadPool.erase(c->rxDataThreadPool.begin() + 4);
			} else {
				c->rxCheck2 = 0;
			}
			c->rxLastDataQSize = c->receivedDataQ.size();
			c->rxDataQ_Pushes = 0;
			c->rxDataQ_Pops = 0;
		}

		static void txThreadMgr(uv_timer_t *handle) {
			//std::cerr<<"TCP-TX Thread Mgr\n";
			TCPServer::Transmiter *c = (TCPServer::Transmiter *) handle->data;
			if (c->txDataQ_Pops == 0 && c->txLastDataQSize > 0) {
				c->txTrdMaker(1);
				c->txCheck2 = 0;
			} else if (c->txDataQ_Pushes > c->txDataQ_Pops) {
				if (c->txCheck2 == 5) {
					c->txTrdMaker(1);
					c->txCheck2 = 0;
				} else {
					c->txCheck2++;
				}
			} else if (c->txDataQ_Pops > c->txDataQ_Pushes &&
			           c->txDataThreadPool.size() > 4) {
				c->txDataThreadPool.erase(c->txDataThreadPool.begin() + 4);
			} else {
				c->txCheck2 = 0;
			}
			c->txLastDataQSize = c->pendingBuffs.size();
			c->txDataQ_Pushes = 0;
			c->txDataQ_Pops = 0;

		}

		void startReceive() {
			rxBuff = "";
			rxTimer.data = this;
			uv_timer_init(&parentClass->loop, &rxTimer);
			//this->dataProcThreadMaker(4);//swap if error
			__StopRx = false;
			uv_timer_start(&rxTimer, rxThreadMgr, 0, 500);
		}

		void startSend() {
			__StopTx = false;
			txTimer.data = this;
			uv_timer_init(&parentClass->loop, &txTimer);

			uv_timer_start(&txTimer, txThreadMgr, 0, 500);

		}

		void stopReceive() {
			uv_timer_stop(&this->rxTimer);
			/*	while (this->receivedDataQ.size() > 0)//wait for dataProcessor to finish off
					std::this_thread::sleep_for(std::chrono::milliseconds(200));*/
			//TODO what if servers has been flooded with data? above code?reconsider?
			__StopRx = true;//stop fetching from the receivedDataQ
		}

		void stopSend() {
			uv_timer_stop(&this->rxTimer);
			__StopTx = true;
			this->pendingBuffs.empty();
			lDebug("TCP send process terminated");
		}

		void freeThreadPool() {
			if (not __StopRx || __StopTx)
				EXTnetworkFailure("Free TCP thread pool aborted. Data processor is still active");
			auto rxLength = rxDataThreadPool.size();
			for (auto i = 0; i < rxLength; i++) {
				rxDataThreadPool[i]->detach();
				delete rxDataThreadPool[i];
			}
			rxDataThreadPool.clear();
			auto txLength = txDataThreadPool.size();
			for (auto i = 0; i < txLength; i++) {
				txDataThreadPool[i]->detach();
				delete txDataThreadPool[i];
			}
			txDataThreadPool.clear();
		}

		void UVsnd(size_t clientId, std::string msg) {
			parentClass->clients[clientId]->send(msg);
		}

	};

/*	struct receivedData {
		size_t clientID;
		std::string data;
	};
	std::queue<receivedData> receivedDataQ;

	void dataProcessor();

	void dataProcThreadMaker(int numberOfThreads = 4);

	void freeThreadPool();

	int dataQ_Pops = 0, dataQ_Pushes = 0, lastDataQSize = 0, check2 = 0;
	bool stopDataProcess = false;*/
	Transmiter transmiter;
	std::mutex mtx;
	uv_timer_t mainTimer;
	std::vector<std::thread *> dataThreadPool;
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
