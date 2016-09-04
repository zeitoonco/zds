//
// Created by ajl on 3/4/16.
//

#include "TCPServer.hpp"
#include "networkUtility.hpp"
#include <cstring>
#include "utility/logger.hpp"

#define DEFAULT_BACKLOG 128

namespace zeitoon {
    namespace utility {


        TCPServer::TCPServer(int port) : clients(this), transmiter(this){
            if (port != -1)
                _port = port;
            int r;

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
            uv_timer_stop(&this->mainTimer);
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
            transmiter.startTransmission();
            /*this->mainTimer.data = this;
            uv_timer_init(&loop, &mainTimer);
            uv_timer_start(&this->mainTimer, dataProcThreadMgrTimer, 0, 300);*/
        }

        void TCPServer::_listen() {

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
            uv_close((uv_handle_t *) &(this->server), NULL);
            if (this->_onDisconnect != NULL)
                this->_onDisconnect();
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
            }
        }

        void TCPServer::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
            *buf = uv_buf_init((char *) malloc(suggested_size), suggested_size);
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
                            c->_parent->transmiter.rxBuff = std::string(buf->base + ci, *size);
                          //  c->_packetReceived();
                            c->_parent->transmiter.receivedDataQ.push(std::make_pair(c->_id, c->_parent->transmiter.rxBuff));
                            logger.log(c->_parent->getNameAndType(),"TCP-R. ID: " + std::to_string(c->_id) + "  " + c->_parent->transmiter.rxBuff, LogLevel::debug);
                            c->_parent->transmiter.rxBuff = "";
                            c->_parent->transmiter._lastPacketLen = 0;
                            ci += *size;
                        } else {//part of packet
                            c->_parent->transmiter._lastPacketLen = *size;
                            c->_parent->transmiter.rxBuff = std::string(buf->base + ci, (size_t) (nread - ci));
                            ci = nread; //fin.
                        }
                    } else if (ci == 0 && c->_parent->transmiter._lastPacketLen > 0) {  //Next part of last packet
                        size_t rem = (c->_parent->transmiter._lastPacketLen -  c->_parent->transmiter.rxBuff.size());
                        if ((rem) <= nread) { // packet complated
                            c->_parent->transmiter.rxBuff += std::string(buf->base, rem);
                            c->_parent->transmiter.receivedDataQ.push(std::make_pair(c->_id, c->_parent->transmiter.rxBuff));
                            logger.log(c->_parent->getNameAndType(),"TCP-R. ID: " + std::to_string(c->_id) + "  " + c->_parent->transmiter.rxBuff, LogLevel::debug);
                            // c->_packetReceived();
                            ci += rem;
                        } else {//another part
                            c->_parent->transmiter.rxBuff += std::string(buf->base, (size_t) (nread));
                            ci = nread; //fin.
                        }
                    }   //else? rubbish!

                }

            }
            free(buf->base);
        }

        void TCPServer::send(size_t clientId, std::string msg) {
            transmiter.pendingBuffs.push(std::make_pair(clientId,msg));
        }

        void TCPServer::clientCollection::client::send(std::string data) {
            if (!this->_isConnected)
                return;

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
            write_req->data = (void *) bufw;

            int r = uv_write(write_req, (uv_stream_t *) this->_client, bufw, 1, TCPServer::on_client_write);
            logger.log("TCPServer", "TCP-S. ID: " + std::to_string(this->_id) + " : " + data, LogLevel::debug);

            try {
                uvEXT(r, "Network uv_write failed");
            } catch (zeitoon::utility::exceptionEx err) {
                lError("SEND ERROR: " + std::string(err.what()));
            }
        }

        void TCPServer::on_client_write(uv_write_t *req, int status) {
            if (status != 0) {
                logger.log("TCPServer", "error on_client_write " + std::string(uv_err_name(status)) + "  " +
                                        std::string(uv_strerror(status)), LogLevel::error);
                //todo:uv_close((uv_handle_t *) &tcps, NULL);
                return;
            }
            uv_buf_t *buffer = (uv_buf_t *) req->data;
            free(buffer->base);
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





    }//zeitoon
}//utility