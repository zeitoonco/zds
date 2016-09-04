//
// Created by ajl on 3/4/16.
//

#include "TCPClient.hpp"
#include "networkUtility.hpp"
#include <inttypes.h>
#include "utility/logger.hpp"

namespace zeitoon {
namespace utility {

TCPClient::~TCPClient() {
	if (isConnected()) {
		this->disconnect();
	}
	uv_timer_stop(&this->mainTimer);
	if (not uv_is_closing((uv_handle_t *) &loop))//check if the handle is already closed
		uv_loop_close(&loop);
}

TCPClient::TCPClient() : dataTransmiter(this), addr(NULL), _connected(false), _buff(""), _lastPacketLen(0) {
	int r;
	// = (uv_timer_t *) malloc(sizeof(uv_timer_t));
	r = uv_loop_init(&loop);
	this->Rtimer_req.data = this;
	uv_timer_init(&loop, &Rtimer_req);
	uvEXT(r, "uv_loop_init failed");
	mainTimer.data = this;//remove no need for CB to have access
	uv_timer_init(&loop, &mainTimer);
	uv_timer_start(&mainTimer, &keepAliveTimerCB, 500, 5000);


	listenTrd = std::thread(&TCPClient::runLoop, this);

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
	client.data = this;
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

	dataTransmiter.stopTransmissionProcess();
	uv_close((uv_handle_t *) &this->client, NULL);


	if (this->_onDisconnect != NULL)
		this->_onDisconnect();
}


void TCPClient::reconnect() {//uv_connect_t *connect = (uv_connect_t *) malloc(sizeof(uv_connect_t));
	try {
		int _interval = reconnectOptions.getNextInterval();
		uv_timer_start(&Rtimer_req, reconnTimerCB, (_interval * 1000), 0);
		lNote("Reconnect in " + std::to_string(_interval) + " seconds");
	} catch (networkMaxRetryReached &err) {
		EXTnetworkMaxRetryReachedI("TCP reconnect failed. max reconnect interval reached.", err);
	} catch (networkNoRetryTimeSet &err) {
		EXTnetworkNoRetryTimeSetI("TCP reconnect failed. no retry interval found", err);
	}

}

bool TCPClient::isConnected() {
	return _connected;
}

void TCPClient::joinOnConnectionThread() {
	listenTrd.join();
}

void TCPClient::runLoop() {
	try {
		int r = uv_run(&this->loop, UV_RUN_DEFAULT);
		uvEXT(r, "libuv events loop error: ");
		lNote("TCPClient loop finished. " + std::string(uv_err_name(r)) + " " + std::string(uv_strerror(r)));
		this->_connected = false;
	} catch (exceptionEx &ex) {
		disconnect();
		EXTnetworkFailureI("TCP LOOP FAILED", ex);
	}
}

void TCPClient::setReconnectInterval(std::string JSON) {
	reconnectOptions.setTiming(JSON);
}

std::string TCPClient::getReconnectInterval() {
	return reconnectOptions.getTiming();
}


std::string TCPClient::defaultReconnInterval() {
	return "[2,2,4,3,8,4,16,5,32,6,64,0]";
}


void TCPClient::Transmiter::dataProcThreadMaker(int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		std::thread *temp = new std::thread(&zeitoon::utility::TCPClient::Transmiter::rxProcessor, this);
		dataThreadPool.push_back(temp);
	}
}

void TCPClient::Transmiter::rxProcessor() {
	std::unique_lock<std::mutex> lck(mtx);
	lck.unlock();
	while (not __stopDataProcess) {
		lck.lock();
		if (receivedDataQ.size() > 0) {
			std::string temp = this->receivedDataQ.front();
			this->receivedDataQ.pop();
			this->dataQ_Pops++;
			lck.unlock();
			this->parentClass->_safeCaller(temp);
		} else {
			lck.unlock();
			std::this_thread::sleep_for(std::chrono::microseconds(TCPCLIENTSLEEPTIME));
		}
	}
}

void TCPClient::Transmiter::freeThreadPool() {//todo: needs a lock
	if (not __stopDataProcess)
		EXTnetworkFailure("Free TCP thread pool aborted. Data processor is still active");
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
		try {
			c->reconnect();
		} catch (zeitoon::utility::exceptionEx &err) {


			/* switch (status) {

				 case (-111): {

					 logger.log(c->getNameAndType(), "TCP Terminating.   nread: " + std::to_string(status),
								zeitoon::utility::LogLevel::error);
					 //  c->disconnect(); todo: review, breaks on stopSendProcess cuz its not started yet!
					 break;
				 }
				 default:
			*/         EXTnetworkFailureO("TCP onConnect failure. ERR: " + std::string(uv_strerror(status)) +
			                              "  MSG:  " + std::string(uv_err_name(status)), c->getNameAndType());
		}


	} else {
		c->dataTransmiter.startTransmissionProcess();
		c->_connected = true;
		c->reconnectOptions.resetInterval();
		if (c->_onConnect != NULL)
			c->_onConnect();
		// fprintf(stderr, "Connected.\n");
		uv_read_start((uv_stream_t *) &c->client, TCPClient::alloc_buffer, TCPClient::on_client_read);
	}
}


void TCPClient::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	*buf = uv_buf_init((char *) malloc(suggested_size), (unsigned int) suggested_size);
}

void TCPClient::on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf) {
	TCPClient *c = (TCPClient *) _client->data;
	if (nread == -4095) {
		free(buf->base);

		c->_connected = false;
		// uv_close((uv_handle_t *) _client, NULL);/////////////
		c->disconnect();
		c->reconnect();
	} else if (nread < 0) { //Error
		free(buf->base);
		c->disconnect();
		EXTnetworkFailureO("TCP onConnect failure. ERR: " + std::string(uv_strerror(nread)) +
		                   "  MSG:  " + std::string(uv_err_name(nread)), c->getNameAndType());
	} else if (nread == 0) { //UNK!
		logger.log(c->getNameAndType(), "TCPClient: read 0!", zeitoon::utility::LogLevel::warning);
	} else {
		ssize_t ci = 0;
		//char *cp = buf->base;
		while (ci < nread) {///what is this needed for?
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
			} else if (ci == 0 && c->_lastPacketLen > 0) {
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
		free(buf->base);
	}
}

void TCPClient::send(std::string data) {//todo:to be tested with valgrind for possible mem leaks
	if (not _connected)
		EXTnetworkFailure("SEND FAILED, NO CONNECTION");
	//FIXME: when disconnected, what happens to the buffer?? data would be transmited via newly established con*
	this->dataTransmiter.pendingBuffs.push(data);


}

void TCPClient::on_client_write(uv_write_t *req, int status) {
	if (status != 0) {
		//tempCL->dataTransmiter.send_is_busy = false;
		fprintf(stderr, "error on_client_write");
		return;
	}

	uv_buf_t *bufw= (uv_buf_t *) req->data;
	free(bufw->base);
	free(bufw);
	free(req);
}

void TCPClient::_packetReceived() {
	/* fixme:: this way, in case of NULL _onmsg-> this function would just empty the string, witch would
				   * just clear the buffer and received data would be lost eventually.
				   **/
	lDebug("TCP-R: " + this->_buff);

	if (this->_onMessage != NULL) {
		dataTransmiter.receivedDataQ.push(this->_buff);
		this->dataTransmiter.dataQ_Pushes++;
	}
	this->_buff = "";
	this->_lastPacketLen = 0;
}


void TCPClient::_safeCaller(std::string data) {
	try {
		this->_onMessage(data);
	} catch (exceptionEx &ex) {
		lError("TCPS.Error.OnReceive: " + std::string(ex.what()));
	} catch (exception &ex) {
		lError("TCPS.Error.OnReceive: " + std::string(ex.what()));
	} catch (...) {
		lError("TCPS.Error.OnReceive: UNKNOWN");
	}
}


void TCPClient::reconnTimerCB(uv_timer_t *handle) {
	TCPClient *c = (TCPClient *) handle->data;
	//free(handle);
	c->connect();
}

//---------------------------------------------------------------------------

void TCPClient::Transmiter::txProcessor() {
	try {
		while (!stopSendt) {
			if (/*this->send_is_busy ||*/ this->pendingBuffs.size() == 0) {
				std::this_thread::sleep_for(std::chrono::microseconds(TCPCLIENTSLEEPTIME));
				continue;
			}


			//this->send_is_busy = true;
			std::string data = this->pendingBuffs.front();
			this->pendingBuffs.pop();

			//uv_write_t *write_req = new uv_write_t[sizeof(uv_write_t)];
			uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));

			uv_buf_t *bufw = (uv_buf_t *) malloc(sizeof(uv_buf_t));
			//this->bufw = new uv_buf_t[sizeof(uv_buf_t)];

			uint8_t *buff = (uint8_t *) malloc(data.size() + 6);
			//uint8_t *buff = new uint8_t[data.size() + 6];


			uint32_t size = (uint32_t) data.size();
			bufw->base = (char *) buff;
			bufw->len = data.size() + 6;
			memcpy(buff + 6, data.c_str(), data.size());
			buff[0] = 12;
			buff[1] = 26;
			memcpy(buff + 2, (void *) (&size), 4);
			write_req->data = (void *) bufw;

			//uv_write(write_req, (uv_stream_t *) this->_client, bufw,
			int r = uv_write(write_req, (uv_stream_t *) &this->parentClass->client, bufw, 1, TCPClient::on_client_write);
			logger.log("TCPClient", "TCP-S: " + data, LogLevel::debug);

			if (r != 0) {
				//this->send_is_busy = false;
				EXTnetworkFailureO(
						"Network uv_write failed" + std::string(uv_err_name(r)) + "[" + std::to_string(r) +
						"]: " +
						uv_strerror(r),
						this->getNameAndType());
			}
		}
	} catch (exceptionEx ex) {
		lError("SendErr: " + ex.toString());
	} catch (...) {
		lError("SendErr: Unknown error...");
	}
}

void TCPClient::Transmiter::dataProcThreadMgrTimer(uv_timer_t *handle) {
	TCPClient::Transmiter *c = (TCPClient::Transmiter *) handle->data;

	if (c->dataQ_Pops == 0 && c->lastDataQSize > 0) {
		c->dataProcThreadMaker(1);
		c->check2 = 0;
	} else if (c->dataQ_Pushes > c->dataQ_Pops) {
		if (c->check2 == 5) {
			c->dataProcThreadMaker(1);
			c->check2 = 0;
		} else {
			c->check2++;
		}
	} else if (c->dataQ_Pops > c->dataQ_Pushes &&
	           c->dataThreadPool.size() > 4) {//todo  this section needs too be ewviewd

		c->dataThreadPool.erase(c->dataThreadPool.begin() + 4);
	} else {
		c->check2 = 0;
	}
	c->lastDataQSize = c->receivedDataQ.size();
	c->dataQ_Pushes = 0;
	c->dataQ_Pops = 0;
}


}//utility
}//zeitoon