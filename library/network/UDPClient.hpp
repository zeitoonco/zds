//
// Created by inf on 3/10/16.
//

#ifndef TEST_UDPCLIENT_HPP
#define TEST_UDPCLIENT_HPP

#include <string>
#include <iostream>
#include "networkUtility.hpp"
#include <uv.h>
#include <map>
#include <thread>

typedef std::function<void(std::string)> onMessageDLG;


class UDPClient {
	onMessageDLG _onMessage;
	sockaddr *addr;
	bool _connected;
	uv_loop_t loop;
	uv_udp_t server;
	uv_udp_t client;
	std::thread *listenThread;

	void listenOnThread();

	static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);


public:
	~UDPClient();

	UDPClient();

	UDPClient(std::string serverAddr, std::string serviceName);

	UDPClient(std::string IP, int port);

	void registerOnMessageCB(onMessageDLG cb) {
		_onMessage = cb;
	}

	void clearInMessageCB() {
		_onMessage = NULL;
	}

	void joinListenThread();

	void listen();

	void listen(std::string IP, int port);

	void listen(std::string serverAddr, std::string serviceName);

	void disconnect(bool wait);

	bool isConnected() {
		return this->_connected;
	}

	static void on_send(uv_udp_send_t *req, int status);

	void send(std::string address, int port, std::string data);

	static void on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr,
	                    unsigned flags);

	std::string getNameAndType() {
		return "UDPClient";
	}
};

#endif //TEST_UDPCLIENT_HPP
