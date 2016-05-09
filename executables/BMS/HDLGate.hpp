//
// Created by ajl on 3/17/16.
//

#ifndef ZDS_HDLGATE_HPP
#define ZDS_HDLGATE_HPP

#include "Gateway.hpp"
#include "protocolHDL.hpp"

namespace zeitoon {
namespace BMS {

//Only accepts a UDP net as parent
class HDLOneIP : public Gateway {
public:
	HDLOneIP(Gateway container, std::string iAddress) : Gateway("HDLOneIP", container, iAddress) { }

	virtual bool supportSubNetworks() { return false; }

	virtual void packetReceived(uint8_t *data, int len) {
		Packet pkt(data + 14, len - 14);
		string addr = to_string(pkt.GetOriginalSubnetID()) + ":" + to_string(pkt.GetOriginalDeviceID());
		for (int i = 0; i < subDevices.length(); i++) {
			if (seq(addr, subDevices[i]->getAddress())) {
				subDevices[i]->packetReceived(data + 14, len - 14);
				break;
			}
		}
		cerr << "HDLOneIP: unregistered device>Packet received: " << pkt.GetOperationCode() << " from "
		<< pkt.GetOriginalSubnetID() << ":" << pkt.GetOriginalDeviceID() << "\nBIN: " << pkt.toString() << endl;

	}

	virtual void sendPacket(uint8_t *data, int len) {
		uint8_t *buff = (uint8_t *) malloc(len + 14);//todo: need a DT IP class
		vector<std::string> ipp = utility::Strings::split(".");
		buff[0] = stoi(ipp[0]);
		buff[1] = stoi(ipp[1]);
		buff[2] = stoi(ipp[2]);
		buff[3] = stoi(ipp[3]);
		buff[4] = 0x48;//{H}
		buff[5] = 0x44;//{D}
		buff[6] = 0x4C;//{L}
		buff[7] = 0x4D;//{M}
		buff[8] = 0x49;//{I}
		buff[9] = 0x52;//{R}
		buff[10] = 0x41;//{A}
		buff[11] = 0x43;//{C}
		buff[12] = 0x4C;//{L}
		buff[13] = 0x45;//{E}
		memcpy(buff + 14, dara, len);
		containerGateway->sendPacket(buff, len + 14);
		free(buff);
	}
};

}
}
#endif //ZDS_HDLGATE_HPP
