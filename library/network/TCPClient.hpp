//
// Created by ajl on 3/4/16.
//

#ifndef NETTEST_TCPCLIENT_HPP
#define NETTEST_TCPCLIENT_HPP

#include <string>
#include <uv.h>
#include <functional>
#include <thread>
#include <iostream>
#include "datatypes/dtmultifieldtypes.hpp"
#include <queue>
#include <mutex>
#include <library/utility/logger.hpp>
#include <atomic>
#include <condition_variable>

extern bool send_is_busy;

namespace zeitoon {
namespace utility {


class ReconnectConfig {
	zeitoon::datatypes::DTSet<zeitoon::datatypes::DTInteger<>> timing = {"timing"};
	int i, j;
public: //to be removed
	~ReconnectConfig() {
	}

	ReconnectConfig() {
		this->resetInterval();
	}

	ReconnectConfig(std::string JSONData) : ReconnectConfig() {
		this->timing.fromString(JSONData);
	}

	size_t timingSize() {
		return timing.length();
	}

	void setTiming(std::string JSON) {
		this->timing.fromString(JSON);
	}

	std::string getTiming() {
		return timing.toString();
	}

	void clearTiming() {
		this->timing.clear();
	}

	int getNextInterval();

	void resetInterval() {
		i = 0;
		j = 0;
	}

	std::string getNameAndType() {
		return "ReconnectConfig ";
	}
};


class TCPClient {
	uv_idle_t txHandler;
	uv_timer_t rxTimer;
	uv_timer_t txTimer;

public:
	std::atomic<int> threadCounter;
	short counter = 0;
	bool reduceRXthread = false;
	typedef std::function<void(std::string)> onMessageDLG;
	typedef std::function<void(void)> onConnectDLG;
	long long int tt = 0, xt = 0;

	void startTXThread();

	std::thread txThread;
	std::vector<std::thread *> txThreadList;
	std::mutex rxMtx, txMtx;
	int dataQ_Pops = 0, dataQ_Pushes = 0, lastDataQSize = 0;
	//SEND
	int txDataQ_Pops = 0, txDataQ_Pushes = 0, txlastDataQSize = 0, txThreadCounter = 0;
	short txCounter = 0;
	std::atomic<bool> txReady;
	std::atomic<bool> txRemoveThread;
	std::condition_variable txNotification;

	void txThreadMaker(int threadsNumber);

	//
	bool __stopDataProcess = true;
	bool stopSendt = false;
	std::atomic<bool> received;

	int testRX = 0, testTX = 0;
	std::queue<std::string> pendingBuffs;
	std::queue<std::string> receivedDataQ;
	std::condition_variable readNotification;

	static void on_client_write(uv_write_t *req, int status);

	void rxProcessor();

	void txProcessor();

	~TCPClient();

	TCPClient();

	static void keepAliveTimerCB(uv_timer_t *handle) {
		logger.flush();
	}

	std::vector<std::thread *> dataThreadPool;

	/*static*/ void rxThreadMgr(/*uv_timer_t *handle*/);

	void rxThradMaker(int numberOfThreads);

	TCPClient(std::string ip, int port);

	TCPClient(std::string address, std::string service);

	void connect();

	void connect(std::string ip, int port);

	void connect(std::string address, std::string service);

	void disconnect();

	bool isConnected();

	void joinOnConnectionThread();

	void send(std::string msg);

	void registerOnMessageCB(onMessageDLG cb) {
		_onMessage = cb;
	}

	void clearInMessageCB() {
		_onMessage = NULL;
	}

	void registerOnClientConnectCB(onConnectDLG cb) {
		_onConnect = cb;
	}

	void clearOnClientConnectCB() {
		_onConnect = NULL;
	}

	void registerOnClientDisconnectCB(onConnectDLG cb) {
		_onDisconnect = cb;
	}

	void clearOnClientDisconnectCB() {
		_onDisconnect = NULL;
	}

	std::string getNameAndType() {
		return std::string("TCPClient");
	}

	void setReconnectInterval(std::string JSON);

	std::string getReconnectInterval();

	std::string defaultReconnInterval();

private:
	uv_timer_t Rtimer_req;


//too cunstructor null shavad, to calback bad az delet null. dar new check shavad agar null = khata
//todo: create  a struct for thread variables.


	zeitoon::utility::ReconnectConfig reconnectOptions;
	uv_timer_t mainTimer;

	uv_loop_t loop;

	uv_tcp_t client;
	sockaddr *addr;
	std::thread listenTrd;
	onMessageDLG _onMessage;
	onConnectDLG _onConnect;
	onConnectDLG _onDisconnect;
	std::string _buff;
	size_t _lastPacketLen = 0;
	bool _connected;

	void runLoop();

	void reconnect();


	static void reconnTimerCB(uv_timer_t *handle);

	static void on_connect(uv_connect_t *req, int status);

	static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

	static void on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf);

	static void TX(uv_timer_t *handle);
	void txThreadMgr();
	void _safeCaller(std::string data);

	void _packetReceived();
//	static void close_cb()
};

}//utility
}//zeitoon

#endif //NETTEST_TCPCLIENT_HPP


