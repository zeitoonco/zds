//
// Created by ajl on 3/4/16.
//

#include "TCPClient.hpp"
#include "networkUtility.hpp"
#include <inttypes.h>

namespace zeitoon {
namespace utility {

TCPClient::TCPClient() : addr(NULL), _connected(false), _buff(""), _lastPacketLen(0) {
	int r;
	r = uv_loop_init(&loop);
	uvEXT(r, "uv_loop_init failed")
	r = uv_tcp_init(&loop, &client);
	uvEXT(r, "uv_tcp_init failed")
	client.data = this;
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
	int r = uv_tcp_connect(connect, &client, addr, on_connect);
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
	if ((this->client.flags & 3) == 0)//fixme:i don't like this condition!
		uv_close((uv_handle_t *) &this->client, NULL);
	//todo:check more about disconnecting/ed ( check handl flags? see in uv_close:1 )
	this->_connected = false;
	if (this->_onDisconnect != NULL)
		this->_onDisconnect();
}

bool TCPClient::isConnected() {
	return _connected;
}

void TCPClient::joinOnConnectionThread() {
	listenTrd->join();
}

void TCPClient::_listen() {
	try {
		std::cerr << "\nTCPClient::listen Start";//todo:Use Logger
		int r = uv_run(&this->loop, UV_RUN_DEFAULT);
		std::cerr << "\nTCPClient::listen Finished with " << r;
	} catch (exceptionEx *ex) {
		cerr << "\nERROR : " << ex->what();
	}
}

//STATIC
void TCPClient::on_connect(uv_connect_t *req, int status) {
	TCPClient *c = (TCPClient *) req->data;
	uvEXTO(status, "New connection error", c->getNameAndType());
	c->_connected = true;
	if (c->_onConnect != NULL)
		c->_onConnect();
	fprintf(stderr, "Connected.\n");
	uv_read_start((uv_stream_t *) &c->client, TCPClient::alloc_buffer, TCPClient::on_client_read);
}

void TCPClient::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	*buf = uv_buf_init((char *) malloc(suggested_size), (unsigned int) suggested_size);
}

void TCPClient::on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf) {
	TCPClient *c = (TCPClient *) _client->data;
	if (nread == -4095) { //EOF
		/*c->_connected = false;
		if (c->_onDisconnect != NULL)
			c->_onDisconnect();*/
		c->disconnect();
	}
	else if (nread < 0) { //Error
		//todo:uv_close((uv_handle_t *) client, NULL); & free?
		c->disconnect();
		uvEXTO(nread, "TCP read failed", c->getNameAndType());
	} else if (nread == 0) { //UNK!
		fprintf(stderr, "TCPClient: read 0!");
	} else {
		ssize_t ci = 0;
		//char *cp = buf->base;
		while (ci < nread) {
			if ((nread - ci) > 6 && buf->base[ci + 0] == 12 && buf->base[ci + 1] == 26) {  //New packet
				uint32_t *size = (uint32_t *) (buf->base + ci + 2);
				ci += 6;
				if ((ci + *size) <= (nread)) {//its whole
					c->_buff = std::string(buf->base + ci, *size);
					c->_packetReceived();
					ci += *size;
				} else {//part of packet
					c->_lastPacketLen = *size;
					c->_buff = std::string(buf->base + ci, (size_t) (nread - ci));
					ci = nread; //fin.
				}
			} else if (ci == 0 && c->_lastPacketLen > 0) {  //Next part of last packet
				size_t rem = (c->_lastPacketLen - c->_buff.size());
				if ((rem) <= nread) { // packet complated
					c->_buff += std::string(buf->base, rem);
					c->_packetReceived();
					ci += rem;
				} else {//another part
					c->_buff += std::string(buf->base, (size_t) (nread));
					ci = nread; //fin.
				}
			}   //else? rubbish!
		}
		c->_buff = string(buf->base, (size_t) nread);
		free(buf->base);
	}
}

void TCPClient::send(std::string data) {
	if (!this->_connected)
		return;
	uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));

	uv_buf_t *bufw = (uv_buf_t *) malloc(sizeof(uv_buf_t));
	uint8_t *buff = (uint8_t *) malloc(data.size() + 6);
	uint32_t size = (uint32_t) data.size();
	bufw->base = (char *) buff;
	bufw->len = data.size() + 6;
	memcpy(buff + 6, data.c_str(), data.size());
	buff[0] = 12;
	buff[1] = 26;
	memcpy(buff + 2, (void *) (&size), 4);

	write_req->data = (void *) bufw->base;
	uv_write(write_req, (uv_stream_t *) &this->client, bufw, 1, TCPClient::on_client_write);
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