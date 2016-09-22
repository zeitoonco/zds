//
// Created by ajl on 3/4/16.
//

#include "TCPClient.hpp"
#include "networkUtility.hpp"
#include <inttypes.h>
#include "utility/logger.hpp"

bool send_is_busy = false;
std::condition_variable sendNotify;
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

void TCPClient::txThreadMgr() {
	std::cout << "TX THREADS: " << txThreadCounter << "\n\n\n\n" << std::endl;
	if (txDataQ_Pops == 0 && txlastDataQSize > 0) {
		txThreadMaker(1);
		txCounter = 0;
	} else if (txDataQ_Pushes > txDataQ_Pops) {

		txThreadMaker(1);
		txCounter = 0;
	} else if ((txDataQ_Pops > txDataQ_Pushes or
	            pendingBuffs.size() == 0) and txThreadCounter > 1 and
	           txCounter <= 3) {//todo  this section needs too be ewviewd
		txCounter++;
	} else if (txCounter >= 3) {
		if (not txRemoveThread) {
			txRemoveThread = true;
			txNotification.notify_one();
			txCounter = 0;
		}
	}

	txlastDataQSize = pendingBuffs.size();
	txDataQ_Pops = 0;
	txDataQ_Pushes = 0;

}


/*
void TCPClient::TX(uv_timer_t *handle) {
	TCPClient *c = (TCPClient *) handle->data;
	std::cout << "TX THREADS: " << c->txThreadCounter<<"\n\n\n\n" << std::endl;
	if (c->txDataQ_Pops == 0 && c->txlastDataQSize > 0) {
		c->txThreadMaker(1);
		c->txCounter = 0;
	} else if (c->txDataQ_Pushes > c->txDataQ_Pops) {

		c->txThreadMaker(1);
		c->txCounter = 0;
	} else if ((c->txDataQ_Pops > c->txDataQ_Pushes or
	            c->pendingBuffs.size() == 0) and c->txThreadCounter > 1 and
	           c->txCounter <= 3) {//todo  this section needs too be ewviewd
		c->txCounter++;
	} else if (c->txCounter >= 3) {
		if (not c->txRemoveThread) {
			c->txRemoveThread = true;
			c->txNotification.notify_one();
			c->txCounter = 0;
		}
	}

	c->txlastDataQSize = c->pendingBuffs.size();
	c->txDataQ_Pops = 0;
	c->txDataQ_Pushes = 0;

*//*	TCPClient *temp = (TCPClient *) hdl->data;

	if (temp->pendingBuffs.size() == 0)
		return;

	if (send_is_busy)
		return;

	send_is_busy = true;
	temp->txProcessor();*//*
}*/

TCPClient::TCPClient() : addr(NULL), _connected(false), _buff(""), _lastPacketLen(0) {
	threadCounter = 0;
	txReady = false;
	received = false;
	txReady = 0;
	txRemoveThread = false;
	int r;
	r = uv_loop_init(&loop);
	this->Rtimer_req.data = this;
	uv_timer_init(&loop, &Rtimer_req);
	uvEXT(r, "uv_loop_init failed");
	mainTimer.data = this;//remove no need for CB to have access
	uv_timer_init(&loop, &mainTimer);
	uv_timer_start(&mainTimer, &keepAliveTimerCB, 500, 5000);
/*
	uv_timer_init(&loop, &txTimer);
	txTimer.data = this;

	uv_timer_start(&txTimer, &TX, 500, 500);
*/

	/*uv_idle_init(&loop, &txTimer);
	txTimer.data = this;

	uv_idle_start(&txTimer, &TX);
	uv_idle_init(&loop, &txHandler);

	uv_idle_start(&txHandler, &waiter);*/
	/*uv_timer_init(&loop, &rxTimer);
	rxTimer.data = this;

	uv_timer_start(&rxTimer, &rxThreadMgr, 500, 500);
*/
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
//todo: if handle alive kil it
	/*uv_idle_stop(&this->txTimer);
	uv_idle_stop(&this->txHandler);
	uv_timer_stop(&this->rxTimer);*/


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


void TCPClient::rxProcessor() {


	//std::cerr << "RX" + std::to_string(xt++) + "\n";

	while (not __stopDataProcess) {
		std::unique_lock<std::mutex> LOCKK(rxMtx);
		while (not received) {
			this->readNotification.wait(LOCKK);
			if (this->reduceRXthread & not received) {
				reduceRXthread = false;
				threadCounter--;
				LOCKK.unlock();
				return;
			}
		}

		if (receivedDataQ.size() == 0) {
			received = false;
			continue;
		}
		std::string temp = this->receivedDataQ.front();
		this->receivedDataQ.pop();
		this->dataQ_Pops++;
		LOCKK.unlock();
		this->_safeCaller(temp);

	}
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
		//TODO: START TRANSMISSION HERE,, and end it when gets disconnected
		c->_connected = true;
		c->reconnectOptions.resetInterval();
		if (c->_onConnect != NULL)
			c->_onConnect();
		c->__stopDataProcess = false;
		std::thread *threadManager = new std::thread([&]{
			while (not c->__stopDataProcess) {
				c->txThreadMgr();
				c->rxThreadMgr();
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
		});
		threadManager->detach();
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
	std::unique_lock<std::mutex> LOKK(txMtx);
	txReady = false;
	this->pendingBuffs.push(data);
	this->txDataQ_Pushes++;
	txReady = true;
	LOKK.unlock();
	txNotification.notify_one();
}

void TCPClient::on_client_write(uv_write_t *req, int status) {

	if (status != 0) {
		fprintf(stderr, "error on_client_write");
		return;
	}
	send_is_busy = false;
	sendNotify.notify_one();

	uv_buf_t *bufw = (uv_buf_t *) req->data;
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
		/*while (not rxMtx.try_lock())
			std::this_thread::sleep_for(std::chrono::microseconds(1));*/
		/* std::unique_lock<std::mutex> lock(g_lock);
   g_done = true;
   std::notify_all_at_thread_exit(g_signal, std::move(lock));*/
		std::unique_lock<std::mutex> LOKK(rxMtx);
		receivedDataQ.push(this->_buff);
		this->dataQ_Pushes++;
		received = true;
		//LOKK.unlock();
		readNotification.notify_one();
		//std::notify_all_at_thread_exit(readNotification, std::move(LOKK));
		//rxMtx.unlock();
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

//---------------------------------------------  std::lock_guard<std::mutex> guard------------------------------


void TCPClient::txProcessor() {
	try {
		std::string data;
		while (not __stopDataProcess) {

			std::unique_lock<std::mutex> LOCKK(txMtx);

			while ((not txReady)/* and send_is_busy*/)
				this->txNotification.wait(LOCKK);

			data = this->pendingBuffs.front();
			this->pendingBuffs.pop();
			this->txDataQ_Pops++;
			if (pendingBuffs.size() == 0)
				txReady = false;
/*			if (this->txRemoveThread) {
				txRemoveThread = false;
				txThreadCounter--;
				txReady--;
				LOCKK.unlock();
				return;
			}*/








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
			while (send_is_busy)
				std::this_thread::sleep_for(std::chrono::nanoseconds(100));
			send_is_busy = false;
			int r = uv_write(write_req, (uv_stream_t *) &this->client, bufw, 1,
			                 TCPClient::on_client_write);

			logger.log("TCPClient", "TCP-S: " + data, LogLevel::debug);

			if (r != 0) {
				LOCKK.unlock();

				//this->send_is_busy = false;
				EXTnetworkFailureO(
						"Network uv_write failed" + std::string(uv_err_name(r)) + "[" + std::to_string(r) +
						"]: " +
						uv_strerror(r),
						this->getNameAndType());
			}
			LOCKK.unlock();


		}
	} catch (exceptionEx
	         ex) {
		lError("SendErr: " + ex.toString());
	} catch (...) {
		lError("SendErr: Unknown error...");
	}


}

void TCPClient::txThreadMaker(int threadsNumber) {
	for (int i = 0; i < threadsNumber; i++) {
		std::thread *temp = new std::thread(&zeitoon::utility::TCPClient::txProcessor, this);
		this->txThreadCounter++;
		temp->detach();
	}
}

void TCPClient::rxThradMaker(int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		std::thread *temp = new std::thread(&zeitoon::utility::TCPClient::rxProcessor, this);
		this->threadCounter++;
		temp->detach();
	}
}

void TCPClient::rxThreadMgr(/*uv_timer_t *handle*/) {
//TCPClient *c = (TCPClient *) handle->data;
	std::cout << "RX THREADS: " << threadCounter << "\n\n\n\n" << std::endl;
	if (dataQ_Pops == 0 && lastDataQSize > 0) {
		rxThradMaker(1);
		counter = 0;
	} else if (dataQ_Pushes > dataQ_Pops) {

		rxThradMaker(1);
		counter = 0;
	} else if ((dataQ_Pops > dataQ_Pushes or
	            receivedDataQ.size() == 0) and threadCounter > 1 and
	           counter <= 3) {//todo  this section needs too be ewviewd
		counter++;
	} else if (counter >= 3) {
		if (not reduceRXthread)
			reduceRXthread = true;
		counter = 0;
		readNotification.notify_one();
	}

	lastDataQSize = receivedDataQ.size();
	dataQ_Pushes = 0;
	dataQ_Pops = 0;
}


}//utility
}//zeitoon