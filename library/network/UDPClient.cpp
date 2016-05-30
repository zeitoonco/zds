//
// Created by inf on 3/10/16.
//

#include <string.h>
#include "UDPClient.hpp"
#include <utility/exceptions.hpp>

using namespace zeitoon::utility;
namespace zeitoon {
namespace utility {

UDPClient::~UDPClient() {
	disconnect(false);
	if (isConnected())
		delete listenThread;

}

UDPClient::UDPClient() : _connected(false) {
	uv_loop_init(&loop);
	uv_udp_init(&loop, &client);
	uv_udp_init(&loop, &server);

	server.data = this;
	client.data = this;

}

UDPClient::UDPClient(std::string IP, int port) : UDPClient() {
	addr = networkUtility::uv_resolveAddress(&loop, IP, std::to_string(port));


}

UDPClient::UDPClient(std::string serverAddr, std::string serviceName) : UDPClient() {
	addr = networkUtility::uv_resolveAddress(&loop, serverAddr, serviceName);
}

void UDPClient::listen(std::string serverAddr, int port) {
	listen(serverAddr, std::to_string(port));
}

void UDPClient::listen(std::string serverAddr, std::string serviceName) {
	addr = networkUtility::uv_resolveAddress(&loop, serverAddr, serviceName);
	listen();
}

void UDPClient::listen() {
	listenThread = new std::thread(&UDPClient::listenOnThread, this);

}

void UDPClient::listenOnThread() {
	uv_udp_bind(&server, this->addr, UV_UDP_REUSEADDR);
	uv_udp_recv_start(&server, alloc_buffer, on_read);
	uv_run(&loop, UV_RUN_DEFAULT);

}

void UDPClient::joinListenThread() {
	if (listenThread->joinable())
		listenThread->join();
}


void UDPClient::disconnect(bool wait = false) {
	if (not isConnected()) {
		return;
	}
	int i = uv_udp_recv_stop(&server);
	uvEXT(i, "Unable to disconnect");

	if (wait) {
		joinListenThread();
	}
	_connected = false;
}

void UDPClient::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	buf->base = (char *) malloc(suggested_size), suggested_size;
	std::cerr << "buf:" << (long int) buf->base << std::endl;
	buf->len = suggested_size;


}


void UDPClient::on_read(uv_udp_t *req, ssize_t nread, const uv_buf_t *buf, const struct sockaddr *addr,
                        unsigned flags) {
	UDPClient *serverPtr = (UDPClient *) req->data;
	if (nread < 0) {
		if (nread == -4095) {
			std::cerr << "socket closed";
			serverPtr->disconnect(true);
		}
		fprintf(stderr, "Read error %s\n", uv_err_name(nread));
		uv_close((uv_handle_t *) req, NULL);
		return;
	} else if (nread == 0) {

		//nothing
	} else {

		char sender[17] = {0};
		uv_ip4_name((const struct sockaddr_in *) addr, sender, 16);
		if (serverPtr->_onMessage != NULL) {
			serverPtr->_onMessage(std::string(buf->base, nread), std::string(sender));
		}
		if (serverPtr->onMessageBin != NULL) {
			serverPtr->onMessageBin(buf->base, nread, std::string(sender));
		}
	}
	free(buf->base);
	std::cerr << "free" << std::endl;
}

void UDPClient::on_send(uv_udp_send_t *req, int status) {

}


void UDPClient::send(std::string address, int port, std::string data) {
	if (data.size() > 65000) {
		std::cerr << "Data size bigger than 65KB" << std::endl;
	}
	uv_udp_send_t send_req;

	uv_buf_t *bufw = (uv_buf_t *) malloc(sizeof(uv_buf_t));//alocated memory
	bufw->base = (char *) malloc(data.size());//alocated memory

	bufw->len = data.size();
	memcpy(bufw->base, data.c_str(), data.size());
	struct sockaddr *temp = networkUtility::uv_resolveAddress(&loop, address, std::to_string(port));
	uv_udp_send(&send_req, &client, bufw, 1, temp, on_send);
	uv_run(&loop, UV_RUN_DEFAULT);

	free(bufw->base);
	free(bufw);
	free(temp);

}

void UDPClient::send(std::string address, int port, char *data, int dataLength) {
	if (dataLength > 65000) {
		std::cerr << "Data size bigger than 65KB" << std::endl;
	}
	uv_udp_send_t send_req;

	uv_buf_t *bufw = (uv_buf_t *) malloc(sizeof(uv_buf_t));//alocated memory
	bufw->base = data;

	bufw->len = dataLength;
	//memcpy(bufw->base, data.c_str(), data.size());
	struct sockaddr *temp = networkUtility::uv_resolveAddress(&loop, address, std::to_string(port));
	uv_udp_send(&send_req, &client, bufw, 1, temp, on_send);
	uv_run(&loop, UV_RUN_DEFAULT);

	free(bufw->base);
	free(bufw);
	free(temp);
}
}
}