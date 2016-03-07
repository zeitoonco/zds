//
// Created by ajl on 3/4/16.
//

#ifndef NETTEST_TCPCLIENT_HPP
#define NETTEST_TCPCLIENT_HPP

#include <string>
#include <uv.h>
#include <functional>
#include <thread>

namespace zeitoon {
namespace utility {

class TCPClient {
public:
	typedef std::function<void(std::string)> onMessageDLG;
	typedef std::function<void()> onConnectDLG;

	TCPClient();

	TCPClient(std::string ip, int port);

	TCPClient(std::string address, std::string service);

	void connect();

	void connect(std::string ip, int port);

	void connect(std::string address, std::string service);

	void disconnect();

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

private:
	uv_loop_t loop;
	uv_tcp_t server;
	sockaddr *addr;
	std::thread *listenTrd;
	onMessageDLG _onMessage;
	onConnectDLG _onConnect;
	onConnectDLG _onDisconnect;
	std::string buff;

	void _listen();

	static void on_connect(uv_connect_t *req, int status);

	static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

	static void on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf);

	static void on_client_write(uv_write_t *req, int status);


};

}//utility
}//zeitoon

#endif //NETTEST_TCPCLIENT_HPP
