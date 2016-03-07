//
// Created by ajl on 3/4/16.
//

#include "TCPClient.hpp"
#include "networkUtility.hpp"

namespace zeitoon {
namespace utility {

TCPClient::TCPClient() : addr(NULL), _connected(false) {
	int r;
	r = uv_loop_init(&loop);
	uvEXT(r, "uv_loop_init failed")
	r = uv_tcp_init(&loop, &server);
	uvEXT(r, "uv_tcp_init failed")
	server.data = this;
}

TCPClient::TCPClient(std::string ip, int port) : TCPClient(ip, std::to_string(port)) { }

TCPClient::TCPClient(std::string address, std::string service) : TCPClient() {
	addr = networkUtility::uv_resolveAddress(&this->loop, address, service);
}

void TCPClient::connect() {
	if (addr == NULL)
		EXTinvalidParameter("No valid address provided");
	uv_connect_t *connect = (uv_connect_t *) malloc(sizeof(uv_connect_t));
	connect->data = this;
	int r = uv_tcp_connect(connect, &server, addr, on_connect);
	uvEXT(r, "Connect failed(uv_tcp_connect)");
	listenTrd = new std::thread(&TCPClient::_listen, this);
}

void TCPClient::connect(std::string ip, int port) {
	connect(ip, std::to_string(port));
}

void TCPClient::connect(std::string address, std::string service) {
	addr = networkUtility::uv_resolveAddress(&this->loop, address, service);
	connect();
}

void TCPClient::disconnect() {
	uv_close((uv_handle_t *) &this->server, NULL);//todo:check more about disconnecting/ed
}

bool TCPClient::isConnected() {
	return _connected;
}

void TCPClient::joinOnConnectionThread() {
	listenTrd->join();
}

void TCPClient::_listen() {
	std::cerr << "\nTCPClient::listen Start";//todo:Use Logger
	int r = uv_run(&this->loop, UV_RUN_DEFAULT);
	std::cerr << "\nTCPClient::listen Finished with " << r;
}

//STATIC
void TCPClient::on_connect(uv_connect_t *req, int status) {
	TCPClient *c = (TCPClient *) req->data;
	uvEXTO(status, "New connection error", c->getNameAndType());
	c->_connected = true;
	fprintf(stderr, "Connected.\n");
	uv_read_start((uv_stream_t *) &c->server, TCPClient::alloc_buffer, TCPClient::on_client_read);
}

void TCPClient::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	*buf = uv_buf_init((char *) malloc(suggested_size), suggested_size);
}

void TCPClient::on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf) {
	TCPClient *c = (TCPClient *) _client->data;
	if (nread == -4095) { //EOF
		/*if (c->buff.size() > 0) {
			if (c->_onMessage != NULL)
				c->_onMessage(c->buff);
			c->buff = "";
		}*/
		c->_connected = false;
		if (c->_onDisconnect != NULL)
			c->_onDisconnect();
	}
	else if (nread < 0) { //Error
		//todo:uv_close((uv_handle_t *) client, NULL); & free?
		uvEXTO(nread, "TCP read failed", c->getNameAndType());
	} else if (nread == 0) { //UNK!
		fprintf(stderr, "TCPClient: read 0!");
	} else {
		c->buff = string(buf->base, nread);
		free(buf->base);
		if (c->_onMessage != NULL)
			c->_onMessage(c->buff);
	}
}

void TCPClient::send(std::string data) {
	uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));

	uv_buf_t *bufw = (uv_buf_t *) malloc(sizeof(uv_buf_t));
	bufw->base = (char *) malloc(data.size());
	bufw->len = data.size();
	memcpy(bufw->base, data.c_str(), data.size());

	write_req->data = (void *) bufw->base;

	uv_write(write_req, (uv_stream_t *) &this->server, bufw, 1, TCPClient::on_client_write);
}

void TCPClient::on_client_write(uv_write_t *req, int status) {
	if (status == -1) {
		fprintf(stderr, "error on_client_write");
		//todo:uv_close((uv_handle_t *) &tcps, NULL);
		return;
	}
	char *buffer = (char *) req->data;
	free(buffer);
	free(req);
}


}//utility
}//zeitoon