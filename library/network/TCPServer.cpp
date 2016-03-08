//
// Created by ajl on 3/4/16.
//

#include "TCPServer.hpp"
#include "networkUtility.hpp"

#define DEFAULT_BACKLOG 128

namespace zeitoon {
namespace utility {


TCPServer::TCPServer(int port) : clients(this) {
	if (port != -1)
		_port = port;
	int r;
	r = uv_loop_init(&loop);
	uvEXT(r, "uv_loop_init failed")
	r = uv_tcp_init(&loop, &server);
	uvEXT(r, "uv_tcp_init failed")
}

TCPServer::~TCPServer() {
	//free something!
}

void TCPServer::listen(int port) {
	int r;
	if (port != -1)
		_port = port;
	if (_port == -1)
		EXTinvalidParameter("No valid port number provided");
	struct sockaddr_in addr;
	r = uv_ip4_addr("0.0.0.0", _port, &addr);
	uvEXT(r, "uv_ip4_addr call failed(invalid port number?)");
	r = uv_tcp_bind(&this->server, (const struct sockaddr *) &addr, 0);
	uvEXT(r, "binding failed(uv_tcp_bind)");
	this->server.data = (void *) this;
	r = uv_listen((uv_stream_t *) &server, DEFAULT_BACKLOG, TCPServer::on_new_connection);
	uvEXT(r, "listen failed(uv_listen)");
	listenTrd = new std::thread(&TCPServer::_listen, this);
}

void TCPServer::_listen() {
	std::cerr << "\nTCPServer::listen Start";//todo:Use Logger
	int r = uv_run(&this->loop, UV_RUN_DEFAULT);
	std::cerr << "\nTCPServer::listen Finished with " << r;
}

void TCPServer::stop() {
	//todo:STOP!
}

void TCPServer::on_new_connection(uv_stream_t *server, int status) {
	if (status < 0) {
		fprintf(stderr, "New connection error %s\n", uv_strerror(status));
		// error!
		return;
	}
	TCPServer *parent = (TCPServer *) server->data;
	uv_tcp_t *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));
	uv_tcp_init(&parent->loop, client);
	clientCollection::client *c = parent->clients.add(client);
	//todo: free this cc::client & uv_client, see uv_tcp_init docs
	client->data = (void *) c;
	int r = uv_accept(server, (uv_stream_t *) client);
	if (r == 0) {
		c->_isConnected = true;
		uv_read_start((uv_stream_t *) client, TCPServer::alloc_buffer, TCPServer::on_client_read);
	}
	else {
		fprintf(stderr, "New connection failed: %s\n", uv_strerror(r));
		uv_close((uv_handle_t *) client, NULL);
	}
}

void TCPServer::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	*buf = uv_buf_init((char *) malloc(suggested_size), suggested_size);
}

void TCPServer::on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf) {
	clientCollection::client *c = (clientCollection::client *) _client->data;
	if (nread == -4095) { //EOF
		/*if (c->buff.size() > 0) {
			if (c->_parent->_onMessage != NULL)
				c->_parent->_onMessage(c->_id, c->buff);
			c->buff = "";
		}*/
		c->_isConnected = false;
		if (c->_parent->_onClientDisconnect != NULL)
			c->_parent->_onClientDisconnect(c->_id);
	}
	else if (nread < 0) { //Error
		//todo:uv_close((uv_handle_t *) client, NULL); & free?
		uvEXTO(nread, "TCP read failed", c->getNameAndType());
	} else if (nread == 0) { //UNK!
		fprintf(stderr, "TCPServer: read 0!");
	} else {
		c->buff = string(buf->base, nread);
		free(buf->base);
		if (c->_parent->_onMessage != NULL)
			c->_parent->_onMessage(c->_id, c->buff);
	}
}

void TCPServer::send(size_t clientId, std::string msg) {
	clients[clientId]->send(msg);
}

void TCPServer::clientCollection::client::send(std::string data) {
	uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));

	uv_buf_t *bufw = (uv_buf_t *) malloc(sizeof(uv_buf_t));
	bufw->base = (char *) malloc(data.size());
	bufw->len = data.size();
	memcpy(bufw->base, data.c_str(), data.size());

	write_req->data = (void *) bufw->base;
	//write_req->type=UV_TCP;
	uv_write(write_req, (uv_stream_t *) this->_client, bufw, 1, TCPServer::on_client_write);
}

void TCPServer::on_client_write(uv_write_t *req, int status) {
	if (status == -1) {
		fprintf(stderr, "error on_client_write");
		//todo:uv_close((uv_handle_t *) &tcps, NULL);
		return;
	}
	char *buffer = (char *) req->data;
	free(buffer);
	free(req);
}

void TCPServer::clientCollection::client::stop() {
	uv_close((uv_handle_t *) this, NULL);
}


void TCPServer::joinOnListenThread() {
	if (listenTrd != NULL)
		listenTrd->join();
}

}//zeitoon
}//utility