//
// Created by ajl on 3/4/16.
//

#include "TCPServer.hpp"
#include "networkUtility.hpp"
#include <cstring>
#include "utility/logger.hpp"
#include "TCPClient.hpp"

#define DEFAULT_BACKLOG 128
namespace zeitoon {
namespace utility {
std::vector<std::thread::id> transnmissionThreadList;
std::atomic<int> rxThreadCounter;
int rxCounter = 0;
bool reduceRXthread = false;
std::thread *threadManager;
bool __stopDataProcess = false;
std::mutex rxMtx, txMtx, ggg;
std::string rxBuff;
int rxDataQ_Pops = 0, rxDataQ_Pushes = 0, rxLastDataQSize = 0;
//SEND
int txDataQ_Pops = 0, txDataQ_Pushes = 0, txlastDataQSize = 0, txThreadCounter = 0;
short txCounter = 0;
std::atomic<bool> txReady;
std::atomic<bool> txRemoveThread;
std::atomic<bool> received;
size_t _lastPacketLen;

std::queue<std::pair<zeitoon::utility::TCPServer::clientCollection::client *, std::string>> pendingBuffs;
std::queue<std::pair<size_t, std::string>> receivedDataQ;
std::condition_variable readNotification;
std::condition_variable txNotification;


TCPServer::TCPServer(int port) : clients(this) {

	if (port != -1)
		_port = port;
	int r;
	__stopDataProcess = true;
	rxThreadCounter = 0;
	r = uv_loop_init(&loop);
	uvEXT(r, "uv_loop_init failed")
	r = uv_tcp_init(&loop, &server);
	uvEXT(r, "uv_tcp_init failed")
	flushTimer.data = this;//remove no need for CB to have access
	uv_timer_init(&loop, &flushTimer);
	uv_timer_start(&flushTimer, &keepAliveTimerCB, 500, 5000);

}

TCPServer::~TCPServer() {
	//free something!
	uv_timer_stop(&this->flushTimer);
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
	std::thread([this] {
		txThreadCounter=0;
		rxThreadCounter=0;
		while (true) {
			std::cout << "TX: " << txThreadCounter << "  PENDING: " << pendingBuffs.size() << "  BUFFER: " <<
			receivedDataQ.size() << std::endl;
			std::cout << "RX: " << txThreadCounter << "   TTC:" << transnmissionThreadList.size() <<
			"     Counter: " + std::to_string(rxThreadCounter) << std::endl;

			std::this_thread::sleep_for(std::chrono::seconds(5));
		}
	}).detach();
}

void TCPServer::_listen() {
	this->startDataTransmission();
	if (this->_onConnect != NULL)
		this->_onConnect();
	int r = uv_run(&this->loop, UV_RUN_DEFAULT);
	if (this->_onDisconnect != NULL)
		this->_onDisconnect();
	lNote("TCP loop finished");
}

void TCPServer::_safeCaller(size_t id, std::string data) {

	try {
		this->_onMessage(id, data);
	} catch (exceptionEx &ex) {
		lError("TCPS.Error.OnReceive: " + std::string(ex.what()));
	} catch (exception &ex) {
		lError("TCPS.Error.OnReceive: " + std::string(ex.what()));
	} catch (...) {
		lError("TCPS.Error.OnReceive: UNKNOWN");
	}
}

void TCPServer::stop() {
	stopDataTransmission();
	for (auto iter :this->clients.clients) {
		iter.second->stop();
		delete iter.second;
	}
	this->clients.clients.clear();
	uv_close((uv_handle_t *) &(this->server), NULL);
	if (this->_onDisconnect != NULL)
		this->_onDisconnect();
}

void TCPServer::startDataTransmission() {
	__stopDataProcess = false;
	threadManager = new std::thread([&] {
		while (not __stopDataProcess) {
			//std::cout<<"SafeCaller Still Hanging: "<<DummyCounter<<std::endl;
			this->txThreadMgr();
			this->rxThreadMgr();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	});
}

void TCPServer::stopDataTransmission() {

	__stopDataProcess = true;
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
/*	for (auto it:transnmissionThreadList)
		delete it;*/
	transnmissionThreadList.clear();
	threadManager->join();
	delete threadManager;
}

void TCPServer::on_new_connection(uv_stream_t *server, int status) {
	TCPServer *parent = (TCPServer *) server->data;
	uv_tcp_t *client = (uv_tcp_t *) malloc(sizeof(uv_tcp_t));
	uv_tcp_init(&parent->loop, client);
	zeitoon::utility::TCPServer::clientCollection::client *c = parent->clients.add(client);
	if (status < 0) {
		logger.log(c->getNameAndType(), "New connection error, " + std::string(uv_strerror(status)),
		           LogLevel::error);
		return;
	}

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
		logger.log(c->getNameAndType(), "New connection error, " + std::string(uv_strerror(r)),
		           LogLevel::error);
		uv_close((uv_handle_t *) client, NULL);
		free(client);//newly added
	}
}

void TCPServer::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
	*buf = uv_buf_init((char *) malloc(suggested_size), suggested_size);
}

void TCPServer::rxThreadMaker(int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		std::thread temp = std::thread(&zeitoon::utility::TCPServer::rxThread, this);
		rxThreadCounter++;
		transnmissionThreadList.push_back(temp.get_id());
		temp.detach();
	}
}

void TCPServer::rxThreadMgr() {

	if (rxDataQ_Pops == 0 && rxLastDataQSize > 0) {
		std::cout << "Creating RX pops0, rxLdQs>0:   " << rxThreadCounter << std::endl;
		rxThreadMaker(1);
		rxCounter = 0;
	} else if (rxDataQ_Pushes > rxDataQ_Pops) {
		std::cout << "Creating RX pops < pushes:  " << rxThreadCounter << std::endl;
		rxThreadMaker(1);
		rxCounter = 0;
	} else if (((rxDataQ_Pops > rxDataQ_Pushes) or
	            (receivedDataQ.size() == 0)) and
	           rxCounter <= 30) {//todo  this section needs too be ewviewd
		rxCounter++;
	} else if ((rxCounter >= 30) and (rxThreadCounter > 1)) {
		if (not reduceRXthread) {
			reduceRXthread = true;
			readNotification.notify_one();
		}
		rxCounter = 0;
	}

	rxLastDataQSize = receivedDataQ.size();
	rxDataQ_Pushes = 0;
	rxDataQ_Pops = 0;
}


void TCPServer::rxThread() {
	while (not __stopDataProcess) {
		std::unique_lock<std::mutex> LOCKK(ggg);
		while (not received) {
			readNotification.wait(LOCKK);
			if (reduceRXthread) {
				reduceRXthread = false;
				rxThreadCounter--;
				LOCKK.unlock();
				for (std::vector<std::thread::id>::iterator iterator = transnmissionThreadList.begin();
				     iterator != transnmissionThreadList.end(); iterator++)
					if ((*iterator) == this_thread::get_id()) {
						transnmissionThreadList.erase(iterator);
						lDebug("Removing thread.. Number of Rx Acive threads:" + std::to_string(rxThreadCounter));
						return;
					}
				rxThreadCounter++;
				continue;
			}
		}


		//	lDebug("rafter while loop RX");

		if (receivedDataQ.size() == 0) {
			received = false;
			LOCKK.unlock();
			continue;
		}

		auto temp = receivedDataQ.front();
		receivedDataQ.pop();
		//lDebug("before the safe caller");
		LOCKK.unlock();
		this->_safeCaller(temp.first, temp.second);
		rxDataQ_Pops++;


	}

}


void TCPServer::_packetReceived(size_t &clientID) {
	/* fixme:: this way, in case of NULL _onmsg-> this function would just empty the string, witch would
				   * just clear the buffer and received data would be lost eventually.
				   **/

	lDebug("TCP-R. ID: " + std::to_string(clientID) + "  " + rxBuff);


	std::unique_lock<std::mutex> LOKK(rxMtx);
	receivedDataQ.push(std::make_pair(clientID, rxBuff));
	rxDataQ_Pushes++;
	received = true;
	readNotification.notify_one();
	rxBuff = "";
	_lastPacketLen = 0;
	LOKK.unlock();

	//lDebug("Exiting _packetReceived");

}

void TCPServer::on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf) {
	zeitoon::utility::TCPServer::clientCollection::client *c = (clientCollection::client *) _client->data;
	if (nread == -4095) { //EOF

		c->stop();
	}
	else if (nread < 0) { //Error
		//todo:uv_close((uv_handle_t *) client, NULL); & free?
		c->stop();
		if (nread == -104) {
			logger.log("TCPServer, on_client_read",
			           "Nread = -104, breaking out of the function. debug needed." +
			           std::string(uv_err_name(10)),
			           LogLevel::error);
			return;
		}
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
					rxBuff = std::string(buf->base + ci, *size);
					//  c->_packetReceived();
					//receivedDataQ.push(std::make_pair(c->_id, rxBuff));
					/*		logger.log(c->_parent->getNameAndType(),
									   "TCP-R. ID: " + std::to_string(c->_id) + "  " + rxBuff,
									   LogLevel::debug);*/
					c->_parent->_packetReceived(c->_id);
					ci += *size;
				} else {//part of packet
					_lastPacketLen = *size;
					rxBuff = std::string(buf->base + ci, (size_t) (nread - ci));
					ci = nread; //fin.
				}
			} else if (ci == 0 && _lastPacketLen > 0) {  //Next part of last packet
				size_t rem = (_lastPacketLen - rxBuff.size());
				if ((rem) <= nread) { // packet complated
					rxBuff += std::string(buf->base, rem);
					c->_parent->_packetReceived(c->_id);
//receivedDataQ.push(std::make_pair(c->_id, rxBuff));
					/*logger.log(c->_parent->getNameAndType(),
					           "TCP-R. ID: " + std::to_string(c->_id) + "  " + rxBuff,
					           LogLevel::debug);*/
					// c->_packetReceived();
					ci += rem;
				} else {//another part
					rxBuff += std::string(buf->base, (size_t) (nread));
					ci = nread; //fin.
				}
			}   //else? rubbish!

		}

	}
	free(buf->base);
}

void TCPServer::send(size_t clientId, std::string msg) {
	std::unique_lock<std::mutex> LOKK(txMtx);
	txReady = false;
	pendingBuffs.push(std::make_pair(clients[clientId], msg));
	txDataQ_Pushes++;
	LOKK.unlock();
	txReady = true;
	txNotification.notify_one();

	//pendingBuffs.push(std::make_pair(clientId, msg));
}

void  TCPServer::txThreadMaker(int numberOfThreads) {
	for (int i = 0; i < numberOfThreads; i++) {
		std::thread temp = std::thread(&zeitoon::utility::TCPServer::txThread, this);
		txThreadCounter++;
		transnmissionThreadList.push_back(temp.get_id());
		temp.detach();
	}
}

void TCPServer::txThreadMgr() {
	if (txDataQ_Pops == 0 && txlastDataQSize > 0 or txThreadCounter < 1) {
		txThreadMaker(1);
		txCounter = 0;
	} else if (txDataQ_Pushes > txDataQ_Pops) {

		txThreadMaker(1);
		txCounter = 0;
	} else if ((txDataQ_Pops > txDataQ_Pushes or
	            pendingBuffs.size() == 0)) {//todo  this section needs too be ewviewd
		txCounter++;
	} else if (txCounter >= 15) {
		if ((not txRemoveThread) and (txThreadCounter > 1)) {
			txRemoveThread = true;
			txNotification.notify_one();
		}
		txCounter = 0;
	}

	txlastDataQSize = pendingBuffs.size();
	txDataQ_Pops = 0;
	txDataQ_Pushes = 0;
}

void TCPServer::txThread() {
	while (not __stopDataProcess) {

		std::unique_lock<std::mutex> LOCKK(txMtx);

		while (not txReady) {
			txNotification.wait(LOCKK);
			if (txRemoveThread) {
				txRemoveThread = false;
				txThreadCounter--;
				LOCKK.unlock();

				for (std::vector<std::thread::id>::iterator iterator = transnmissionThreadList.begin();
				     iterator != transnmissionThreadList.end(); iterator++)
					if ((*iterator) == this_thread::get_id()) {
						transnmissionThreadList.erase(iterator);
						lDebug("Removing thread.. Number of Tx Acive threads:" + std::to_string(txThreadCounter));
						return;
					}
				txThreadCounter++;
				continue;
			}
		}
		if (pendingBuffs.size() == 0) {
			txReady = false;
			LOCKK.unlock();
			continue;
		}

		auto temp = pendingBuffs.front();
		pendingBuffs.pop();
		txDataQ_Pops++;
		while (not uv_is_writable((uv_stream_t *) temp.first->_client))
			this_thread::sleep_for(chrono::nanoseconds(5));
		if (temp.first->_isConnected)
			//EXTconnectionError("No connection for client");
			temp.first->send(temp.second);
		LOCKK.unlock();


	}
}

void TCPServer::clientCollection::client::send(std::string data) {
	try {
		uv_write_t *write_req = (uv_write_t *) malloc(sizeof(uv_write_t));

		uint8_t *buff = (uint8_t *) malloc(data.size() + 6);

		if (data.size() == 0)
			EXTunknownException("SEND DATA SIZE 0");

		uint32_t size = (uint32_t) data.size();

		memcpy(buff + 6, data.c_str(), data.size());
		buff[0] = 12;
		buff[1] = 26;
		memcpy(buff + 2, (void *) (&size), 4);


		uv_buf_t bufw = uv_buf_init((char *) buff, sizeof(buff));

		bufw.base = (char *) buff;
		bufw.len = data.size() + 6;

		write_req->data = (void *) this;

		int r = uv_write(write_req, (uv_stream_t *) this->_client, &bufw, 1,
		                 [&](uv_write_t *req, int status) {


			                 if (status != 0) {
				                 if (status == -9) {
					                 //temp->_ptr->stop();//newly added
				                 }
				                 logger.log("TCPServer",
				                            "error on_client_write " + std::string(uv_err_name(status)) + "  " +
				                            std::string(uv_strerror(status)), LogLevel::error);
				                 //todo:uv_close((uv_handle_t *) &tcps, NULL);
			                 }
			                 free(req);
		                 });

		logger.log("TCPServer", "TCP-S [" + std::to_string(this->_id) + "]: " + data, LogLevel::debug);

		if (r != 0) {

			EXTnetworkFailureO(
					"Network uv_write failed" + std::string(uv_err_name(r)) + "[" + std::to_string(r) +
					"]: " +
					uv_strerror(r),
					this->getNameAndType());
		}


	} catch (exceptionEx
	         ex) {
		lError("SendErr: " + ex.toString());
	} catch (...) {
		lError("SendErr: Unknown error...");
	}
}

void TCPServer::on_client_write(uv_write_t *req, int status) {
//Used a lambda for send CB check the line with uv_write for more detail of CB
	//this section is not up to date

	if (status != 0) {
		if (status == -9) {
		}
		logger.log("TCPServer", "error on_client_write " + std::string(uv_err_name(status)) + "  " +
		                        std::string(uv_strerror(status)), LogLevel::error);
		//todo:uv_close((uv_handle_t *) &tcps, NULL);
	}


	free(req);
}

void TCPServer::clientCollection::client::stop() {
	this->_isConnected = false;
	if ((this->_client->flags & 3) == 0)//fixme:i don't like this condition!
		uv_close((uv_handle_t *) this->_client, NULL);

	if (this->_parent->_onClientDisconnect != NULL)
		this->_parent->_onClientDisconnect(this->_id);

	_parent->clients.remove(this);

}

void TCPServer::joinOnListenThread() {
	if (listenTrd != NULL)
		listenTrd->join();
}


}



//zeitoon
}//utility