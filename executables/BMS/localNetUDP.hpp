//
// Created by ajl on 3/16/16.
//

#ifndef ZDS_LOCALNETUDP_HPP
#define ZDS_LOCALNETUDP_HPP

#include "Gateway.hpp"
#include <network/UDPClient.hpp>

#define HDLUDPPort 6000

namespace zeitoon {
namespace BMS {


class localNetUDP : public localGateway {
	using zeitoon::utility::UDPClient;
	UDPClient udp;

public:
	localNetUDP(int portNo) : localGateway("localNetUDP"), udp(portNo) {
		udp.registerOnMessageBinary(
				bind(&localNetUDP::packetReceived, this, placeholders::_1, placeholders::_2, placeholders::_3));
	}

	bool supportSubNetworks() {
		return true;
	}

	void packetReceived(uint8_t *data, int len) {
		EXTnotImplemented("HEY! you shouldn't be here!");
	}

	void packetReceived(uint8_t *data, int len, std::string from) {
		bool done = false;
		for (size_t i = 0; i < subGateways.length() && !done; i++) {
			if (seq(subGateways[i]->getAddress(), from)) {
				done = true;
				subGateways[i]->packetReceived(data, len);
			}
		}
		for (size_t i = 0; i < subDevices.length() && !done; i++) {
			if (seq(subDevices[i]->getAddress(), from)) {
				done = true;
				subDevices[i]->packetReceived(data, len);
			}
		}
		if (!done) {
			std::stringstream temp;
			temp << std::hex << std::uppercase;
			for (int i = 0; i < len; i++) {
				if (data[i] < 16) { temp << 0; }
				temp << (int) (data[i]) << " ";
			}
			temp << std::dec << std::nouppercase;
			cerr << "localNetUDP: unregistered device/gateway>Packet received: len" << len << "\nBIN: " << temp.str()
			<< endl;
		}
	}

	void sendPacket(uint8_t *data, int len, std::string to) {
		udp.send(to, HDLUDPPort, (char *) data, len);
	}

	bool isConnected() {
		return udp.isConnected();
	}

	void connect() {
		udp.listen();
	}

	void disconnect() {
		udp.disconnect();
	}
};


}
}
#endif //ZDS_LOCALNETUDP_HPP
