//
// Created by ajl on 3/4/16.
//

#include "TCPServer.hpp"
#include "networkUtility.hpp"
#include <cstring>

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
	this->dataProcThreadMaker(4);

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
	this->mainTimer.data = this;
	uv_timer_init(&loop, &mainTimer);
	//this_thread::sleep_for(std::chrono::milliseconds(5000));
	uv_timer_start(&this->mainTimer, dataProcThreadMgrTimer, 0, 300);
}

void TCPServer::_listen() {
	try {
		std::cerr << "\nTCPServer::listen Start";//todo:Use Logger by ajl /// what is log needed for? // how to log ?
		if (this->_onConnect != NULL)
			this->_onConnect();
		int r = uv_run(&this->loop, UV_RUN_DEFAULT);
		if (this->_onDisconnect != NULL)
			this->_onDisconnect();
		std::cerr << "\nTCPServer::listen Finished with " << r;
	} catch (exceptionEx &ex) {
		cerr << "Uncought ERROR: " << ex.what();
	} /*catch (exception &ex){
		cerr << "Uncought ERROR: " << ex.what();
	}*/
}

void TCPServer::stop() {
	uv_close((uv_handle_t *) &(this->server), NULL);
	if (this->_onDisconnect != NULL)
		this->_onDisconnect();
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
		if (c->_parent->_onClientConnect != NULL)
			c->_parent->_onClientConnect(c->_id);
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
	//cerr << "\nR" << nread << ";";
	clientCollection::client *c = (clientCollection::client *) _client->data;
	if (nread == -4095) { //EOF
		c->stop();
	}
	else if (nread < 0) { //Error
		//todo:uv_close((uv_handle_t *) client, NULL); & free?
		c->stop();
		uvEXTO(nread, "TCP read failed", c->getNameAndType());
	} else if (nread == 0) { //UNK!
		fprintf(stderr, "TCPServer: read 0!");
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
				//cerr << "NR" << nread << "," << c->_lastPacketLen << "," << rem << endl;
				if ((rem) <= nread) { // packet complated
					c->_buff += std::string(buf->base, rem);
					c->_packetReceived();
					ci += rem;
				} else {//another part
					c->_buff += std::string(buf->base, (size_t) (nread));
					ci = nread; //fin.
				}
				//c->_buff = string(buf->base, (size_t) nread);
			}   //else? rubbish!

		}

	}
	free(buf->base);

}

void TCPServer::send(size_t clientId, std::string msg) {
	clients[clientId]->send(msg);
}

void TCPServer::clientCollection::client::send(std::string data) {
	if (!this->_isConnected)
		return;
	cerr << "\nSEND " << this->_id << ":" << data;
	uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));

	uv_buf_t *bufw = (uv_buf_t *) malloc(sizeof(uv_buf_t));
	uint8_t *buff = (uint8_t *) malloc(data.size() + 6);
	uint32_t size = data.size();
	bufw->base = (char *) buff;
	bufw->len = data.size() + 6;
	memcpy(buff + 6, data.c_str(), data.size());
	buff[0] = 12;
	buff[1] = 26;
	memcpy(buff + 2, (void *) (&size), 4);


	write_req->data = (void *) bufw->base;
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
	//uv_shutdown()
	this->_isConnected = false;
	if ((this->_client->flags & 3) == 0)//fixme:i don't like this condition!
		uv_close((uv_handle_t *) this->_client, NULL);

	if (this->_parent->_onClientDisconnect != NULL)
		this->_parent->_onClientDisconnect(this->_id);
}

void TCPServer::joinOnListenThread() {
	if (listenTrd != NULL)
		listenTrd->join();
}

void TCPServer::dataProcThreadMgrTimer(uv_timer_t *handle) {
	TCPServer *c = (TCPServer *) handle->data;
	uv_timer_t sc = c->mainTimer;
	if (c->dataQ_Pops == 0 && c->lastDataQSize > 0) {
		c->dataProcThreadMaker(1);
		c->check2 = 0;
		//std::cerr << "NEW THREAD No Pops since last lab" << endl;
	} else if (c->dataQ_Pushes > c->dataQ_Pops) {
		if (c->check2 == 5) {
			c->dataProcThreadMaker(1);
			c->check2 = 0;
			//std::cerr << "NEW THREAD Pushez > Popz" << endl;
		} else if (c->dataQ_Pops > c->dataQ_Pushes && c->dataThreadPool.size() > 4) {
			c->dataThreadPool.erase(c->dataThreadPool.begin() + 4);
		} else {
			c->check2++;
		}
	} else {
		c->check2 = 0;
	}
	c->lastDataQSize = c->receivedDataQ.size();
	c->dataQ_Pushes = 0;
	c->dataQ_Pops = 0;
}

void TCPServer::dataProcThreadMaker(int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		std::thread *temp = new std::thread(&TCPServer::dataProcessor, this);
		dataThreadPool.push_back(temp);
	}
}

void TCPServer::freeThreadPool() {
	stopDataProcess = true;
	this_thread::sleep_for(chrono::milliseconds(500));
	for (auto i = 0; i < dataThreadPool.size(); i++) {
		dataThreadPool[i]->detach();
		delete dataThreadPool[i];
	}
}

void TCPServer::dataProcessor() {
	std::unique_lock<std::mutex> lck(mtx);
	lck.unlock();
	while (not stopDataProcess) {
		lck.lock();
		if (receivedDataQ.size() > 0) {
			receivedData temp = this->receivedDataQ.front();
			//std::cerr << "data Proc: " << temp.data << endl;
			this->receivedDataQ.pop();
			this->dataQ_Pops++;
			lck.unlock();
			_safeCaller(temp.clientID, temp.data);
			//std::cerr << "data Proc done: " << temp.data << endl;
		} else {
			lck.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

	}
}

}//zeitoon
}//utility