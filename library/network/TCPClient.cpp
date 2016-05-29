//
// Created by ajl on 3/4/16.
//

#include "TCPClient.hpp"
#include "networkUtility.hpp"
#include <inttypes.h>

namespace zeitoon {
namespace utility {

TCPClient::~TCPClient() {
	if (isConnected()) {
		this->disconnect();
	}
	freeThreadPool();
	uv_loop_close(&loop);
	delete listenTrd;
}

TCPClient::TCPClient() : addr(NULL), _connected(false), _buff(""), _lastPacketLen(0) {
	int r;
	r = uv_loop_init(&loop);
	uvEXT(r, "uv_loop_init failed")
	mainTimer.data = this;

	client.data = this;

	uv_timer_init(&loop, &mainTimer);
	uv_timer_start(&mainTimer, dataProcThreadMgrTimer, 0, 500);
	listenTrd = new std::thread(&TCPClient::runLoop, this);

	this->dataProcThreadMaker(4);
}

int ReconnectConfig::getNextInterval() {
	size_t size = this->timingSize();
	if (size < 1)
		EXTnetworkNoRetryTimeSet("NO Retry Intervals.");
	if (j < (timing[i + 1])->getValue()) {
		j++;
	} else if (j >= (timing[i + 1])->getValue()) {
		if (size > i + 2) {
			i += 2;
			j = 1;
		} else {
			if (timing[size - 1]->getValue() != 0) {
				resetInterval();
				EXTnetworkMaxRetryReached("RECONNECT FAILED.");
			}
		}
	}
	return timing[i]->getValue();
}
TCPClient::TCPClient(std::string ip, int port) : TCPClient(ip, std::to_string(port)) { }

TCPClient::TCPClient(std::string address, std::string service) : TCPClient() {
	addr = networkUtility::uv_resolveAddress(&this->loop, address, service);
}

void TCPClient::connect() {
	if (addr == NULL)
		EXTinvalidParameter("No valid address provided");

	int r = uv_tcp_init(&loop, &client);
	uvEXT(r, "uv_tcp_init failed")
	uv_connect_t *connect = (uv_connect_t *) malloc(sizeof(uv_connect_t));
	connect->data = this;
	r = uv_tcp_connect(connect, &client, addr, on_connect);
	uvEXT(r, "Connect failed(uv_tcp_connect)");

}

void TCPClient::connect(std::string ip, int port) {
	connect(ip, std::to_string(port));
}

void TCPClient::connect(std::string address, std::string service) {
	addr = networkUtility::uv_resolveAddress(&this->loop, address, service);
	connect();
}

void TCPClient::disconnect() {
	if (isConnected()) {
		uv_unref((uv_handle_t *) &this->mainTimer);//STOPPING THE MAIN LOOP CREATED EARLIER IN CONSTRUCTOR
		uv_close((uv_handle_t *) &this->client, NULL);
		listenTrd->join();
		this->_connected = false;
		if (this->_onDisconnect != NULL)
			this->_onDisconnect();
	}
}

void TCPClient::reconnect() {//uv_connect_t *connect = (uv_connect_t *) malloc(sizeof(uv_connect_t));

	uv_timer_t *Rtimer_req = (uv_timer_t *) malloc(sizeof(uv_timer_t));
	Rtimer_req->data = this;
	uv_timer_init(&this->loop, Rtimer_req);

	try {
		int _interval = reconnectOptions.getNextInterval();
		uv_timer_start(Rtimer_req, reconnTimerCB, (_interval * 1000), 0);
		std::cerr << "Reconnect in " << _interval << " seconds" << std::endl;

	} catch (networkMaxRetryReached *err) {
		std::cerr << "ERROR: " << err->what() << std::endl;

		uv_unref((uv_handle_t *) &mainTimer);//STOPPING THE MAIN KEEP ALIVE TIMER CREATED EARLIER IN CONSTRUCTOR
	} catch (networkNoRetryTimeSet *err) {
		std::cerr << "ERROR: " << err->what() << std::endl;
		this->setReconnectInterval("{\"timing\":[10,0]}");

	}

}

bool TCPClient::isConnected() {
	return _connected;
}

void TCPClient::joinOnConnectionThread() {
	listenTrd->join();
}

void TCPClient::runLoop() {
	try {
		std::cerr << "\nTCPClien EVENTS LOOP Start";//todo:Use Logger by ajl /// what is log needed for? // how to log ?
		int r = uv_run(&this->loop, UV_RUN_DEFAULT);
		uvEXT(r, "libuv events loop error: ");
		std::cerr << "\nTCPClient EVENTS LOOP Finished with " << r << std::endl;
	} catch (exceptionEx &ex) {
		cerr << "\nERROR ON TCPClient EVENTS LOOP: " << ex.what() << std::endl;
		uv_connect_t *connect = (uv_connect_t *) malloc(sizeof(uv_connect_t));
		uv_tcp_connect(connect, &client, addr, on_connect);
	}
}

void TCPClient::setReconnectInterval(std::string JSON) {
	reconnectOptions.setTiming(JSON);
}

void TCPClient::dataProcThreadMaker(int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		std::thread *temp = new std::thread(&TCPClient::dataProcessor, this);
		dataThreadPool.push_back(temp);
	}
}

void TCPClient::dataProcessor() {
	std::unique_lock<std::mutex> lck(mtx);
	lck.unlock();
	while (not stopDataProcess) {
		lck.lock();
		if (receivedDataQ.size() > 0) {
			std::string temp = this->receivedDataQ.front();
			this->receivedDataQ.pop();
			this->dataQ_Pops++;
			lck.unlock();
			this->_safeCaller(temp);
		} else {
			lck.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	std::cerr << "Thread No " << this_thread::get_id() << " terminated" << std::endl;
}

void TCPClient::freeThreadPool() {
	stopDataProcess = true;
	this_thread::sleep_for(chrono::milliseconds(500));
	auto length = dataThreadPool.size();
	for (auto i = 0; i < length; i++) {
		dataThreadPool[i]->detach();
		delete dataThreadPool[i];
	}
	dataThreadPool.clear();
}

//STATIC
void TCPClient::on_connect(uv_connect_t *req, int status) {
	TCPClient *c = (TCPClient *) req->data;
	free(req);
	if (status) {
		std::cerr << "ERROR: " << uv_err_name(status) << ": " << uv_strerror(status) << std::endl;
		if (c->reconnectOptions.timingSize() > 0) {
			uv_close((uv_handle_t *) &c->client, NULL);
			c->reconnect();
		} else {
			std::cerr << "No reconnect intervals specified." << std::endl;
			uv_unref((uv_handle_t *) &c->mainTimer);//STOPPING THE MAIN LOOP CREATED EARLIER IN CONSTRUCTOR
		}
	} else {
		c->_connected = true;
		cerr << c->client.flags << std::endl;
		if (c->_onConnect != NULL)
			c->_onConnect();
		fprintf(stderr, "Connected.\n");
		uv_read_start((uv_stream_t *) &c->client, TCPClient::alloc_buffer, TCPClient::on_client_read);
	}
	std::cerr << "EXIT On Connect reached" << std::endl;

}


void TCPClient::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	*buf = uv_buf_init((char *) malloc(suggested_size), (unsigned int) suggested_size);
}

void TCPClient::on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf) {
	//std::cerr << "Client read reached" << std::endl;
	//std::cerr << "NREAD: " << nread << std::endl;

	TCPClient *c = (TCPClient *) _client->data;
	if (nread == -4095) {
		free(buf->base);
		c->_connected = false;
		c->reconnect();
	}
	else if (nread < 0) { //Error
		//todo:uv_close((uv_handle_t *) client, NULL); & free?
		free(buf->base);
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
		//c->_buff = string(buf->base, (size_t) nread);
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

	write_req->data = (void *) bufw;
	uv_write(write_req, (uv_stream_t *) &this->client, bufw, 1, TCPClient::on_client_write);


}

void TCPClient::on_client_write(uv_write_t *req, int status) {
	uv_buf_t *buffer = (uv_buf_t *) req->data;
	if (status == -1) {
		fprintf(stderr, "error on_client_write");
		return;
	}
	free(buffer->base);
	free(req->data);
	free(req);
}

void TCPClient::dataProcThreadMgrTimer(uv_timer_t *handle) {
	TCPClient *c = (TCPClient *) handle->data;

	if (c->dataQ_Pops == 0 && c->lastDataQSize > 0) {
		c->dataProcThreadMaker(1);
		c->check2 = 0;
		//std::cerr << "NEW THREAD CREATED" << endl;
	} else if (c->dataQ_Pushes > c->dataQ_Pops) {
		if (c->check2 == 5) {
			c->dataProcThreadMaker(1);
			c->check2 = 0;
			//std::cerr << "NEW THREAD CREATED" << endl;
		} else {
			c->check2++;
		}
	} else if (c->dataQ_Pops > c->dataQ_Pushes && c->dataThreadPool.size() > 4) {//todo:: to be double checked by ajl
		c->dataThreadPool.erase(c->dataThreadPool.begin() + 4);
	} else {
		c->check2 = 0;
	}
	c->lastDataQSize = c->receivedDataQ.size();
	c->dataQ_Pushes = 0;
	c->dataQ_Pops = 0;
}

void TCPClient::reconnTimerCB(uv_timer_t *handle) {
	//std::cerr << "reconnTimerCB reached" << std::endl;
	TCPClient *c = (TCPClient *) handle->data;
	free(handle);
	c->connect();
}

}//utility
}//zeitoon