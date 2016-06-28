//
// Created by ajl on 3/4/16.
//

#ifndef NETTEST_TCPCLIENT_HPP
#define NETTEST_TCPCLIENT_HPP

#include <string>
#include <uv.h>
#include <functional>
#include <thread>
#include <iostream>
#include "datatypes/dtmultifieldtypes.hpp"
#include <queue>
#include <mutex>

namespace zeitoon {
    namespace utility {


        class ReconnectConfig : public zeitoon::datatypes::DTStruct {
            zeitoon::datatypes::DTSet<zeitoon::datatypes::DTInteger<>> timing = {"timing"};
            int i, j;
        public: //to be removed
            ~ReconnectConfig() {
                this->clear();
            }

            ReconnectConfig() : DTStruct("ReconnectConfig", 0, 0, 0) {
                this->add(&timing);
                this->resetInterval();
            }

            ReconnectConfig(std::string JSONData) : ReconnectConfig() {
                this->fromString(JSONData);
            }

            size_t timingSize() {
                return timing.length();
            }

            void setTiming(std::string JSON) {
                this->fromString(JSON);
            }

            void clearTiming() {
                this->timing.clear();
            }

            int getNextInterval();

            void resetInterval() {
                i = 0;
                j = 0;
            }

            std::string getNameAndType() {
                return "ReconnectConfig ";
            }
        };


        class TCPClient {
        public:
            typedef std::function<void(std::string)> onMessageDLG;
            typedef std::function<void(void)> onConnectDLG;

            class DataTransmiter {
                TCPClient *parentClass;
                std::mutex mtx;

            public:

                bool send_is_busy = false; // remove in case of multithreading
                int dataQ_Pops = 0, dataQ_Pushes = 0, lastDataQSize = 0, check2 = 0;
                bool __stopDataProcess = true;
                bool __stopSendProcess = true;

                uv_buf_t *bufw;
                uv_timer_t receiveTimer;
                uv_timer_t sendTimer;
                std::queue<std::string> pendingBuffs;
                std::queue<std::string> receivedDataQ;
                std::vector<std::thread *> dataThreadPool;

                DataTransmiter(zeitoon::utility::TCPClient *iparentClass) : parentClass(iparentClass) {

                }

                void startTransmissionProcess() {
                    this->startReceiveProcess();
                    this->startSendProcess();
                }

                void startReceiveProcess() {
                    bufw = NULL;
                    receiveTimer.data = this;
                    uv_timer_init(&parentClass->loop, &receiveTimer);
                    //this->dataProcThreadMaker(4);//swap if error
                    __stopDataProcess = false;
                    uv_timer_start(&receiveTimer, dataProcThreadMgrTimer, 0, 500);
                }

                void stopReceiveProcess() {
                    uv_unref((uv_handle_t *) &this->receiveTimer);
                    while (this->receivedDataQ.size() > 0)//wait for dataProcessor to finish off
                        std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    __stopDataProcess = true;//stop fetching from the receivedDataQ
                    freeThreadPool();
                }

                void stopSendProcess() {
                    uv_unref(
                            (uv_handle_t *) &this->sendTimer);//STOPPING THE MAIN LOOP CREATED EARLIER IN CONSTRUCTOR

                }

                void startSendProcess() {
                    sendTimer.data = this;
                    uv_timer_init(&parentClass->loop, &sendTimer);
                    uv_timer_start(&sendTimer, sendProcessor, 0, 150);
                }

                void stopTransmissionProcess() {
                    this->stopSendProcess();
                    this->stopReceiveProcess();
                }

                std::string getNameAndType() {
                    return "TCPClient::dataTransmiter";
                }

            private:
                void dataProcThreadMaker(int numberOfThreads = 4);

                void freeThreadPool();

                static void sendProcessor(uv_timer_t *handle);

                static void dataProcThreadMgrTimer(uv_timer_t *handle);

                void dataProcessor();


            };

            ~TCPClient();

            TCPClient();

            static void keepAliveTimerCB(uv_timer_t *handle) {
              /*  TCPClient *c = (TCPClient *) handle->data;
                std::cerr << "KEEP ALIVE EVENT\nConnected: " << c->isConnected() << "\nReceiver:  " <<
                (not c->dataTransmiter.__stopDataProcess) << "\nReceiveQ size: " <<
                c->dataTransmiter.receivedDataQ.size() << "\n";
*/

            }


            TCPClient(std::string ip, int port);

            TCPClient(std::string address, std::string service);

            void connect();

            void connect(std::string ip, int port);

            void connect(std::string address, std::string service);

            void disconnect();

            bool isConnected();

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

            void setReconnectInterval(std::string JSON);


        private:
            DataTransmiter dataTransmiter;


/*	std::thread sendThrd;
	void sendTimerThread();*/

//too cunstructor null shavad, to calback bad az delet null. dar new check shavad agar null = khata
//todo: create  a struct for thread variables.


            ReconnectConfig reconnectOptions;
            uv_timer_t mainTimer;
            uv_loop_t loop;
            uv_tcp_t client;
            sockaddr *addr;
            std::thread *listenTrd = NULL;
            onMessageDLG _onMessage;
            onConnectDLG _onConnect;
            onConnectDLG _onDisconnect;
            std::string _buff;
            size_t _lastPacketLen = 0;
            bool _connected;

            void runLoop();

            void reconnect();


            static void reconnTimerCB(uv_timer_t *handle);

            static void on_connect(uv_connect_t *req, int status);

            static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);

            static void on_client_read(uv_stream_t *_client, ssize_t nread, const uv_buf_t *buf);

            static void on_client_write(uv_write_t *req, int status);

            void _safeCaller(std::string data) {
                try {
                    this->_onMessage(data);
                } catch (exceptionEx &ex) {
                    cerr << "TCPS.Error.OnReceive: " << ex.what() << endl;
                } catch (exception &ex) {
                    cerr << "TCPS.sysError.OnReceive: " << ex.what() << endl;
                } catch (...) {
                    cerr << "TCPS.uncaughtError.OnReceive: " << endl;
                }
            }

            void _packetReceived() {
                /* fixme:: this way, in case of NULL _onmsg-> this function would just empty the string, witch would
                 * just clear the buffer and received data would be lost eventually.
                 **/
                if (this->_onMessage != NULL) {
                    dataTransmiter.receivedDataQ.push(this->_buff);
                    this->dataTransmiter.dataQ_Pushes++;
                }
                this->_buff = "";
                this->_lastPacketLen = 0;
            }
//	static void close_cb()
        };

    }//utility
}//zeitoon

#endif //NETTEST_TCPCLIENT_HPP


