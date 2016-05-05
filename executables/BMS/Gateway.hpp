//
// Created by ajl on 3/16/16.
//

#ifndef ZDS_GATEWAY_HPP
#define ZDS_GATEWAY_HPP

#include <datatypes/dtmultifieldtypes.hpp>
#include "Device.hpp"

namespace zeitoon {
namespace BMS {

#define PhysicalAddressDelim '/'
#define LogicalAddressDelim '>'

class Gateway : public datatypes::DTStruct {
protected:
	std::string address;
	Gateway *containerGateway;
	datatypes::DTSet<Gateway> subGateways;
	datatypes::DTSet<Device> subDevices;
public:
	Gateway(std::string iname, Gateway container, std::string iAddress) : DTStruct(iname, 0, 0, 0),
	                                                                      containerGateway(container),
	                                                                      address(iAddress) {

	}

	virtual ~Gateway() { }

	virtual std::string getAddress() { return address; }

	virtual std::string getFullPath() {
		return (containerGateway == NULL ? "" : containerGateway->getFullPath() + PhysicalAddressDelim) +
		       this->getName();
	}

	virtual bool supportSubNetworks() = 0;

	virtual void packetReceived(uint8_t *data, int len) = 0;

	virtual void sendPacket(uint8_t *data, int len) = 0;
};

class localGateway : public Gateway {
public:
	localGateway(std::string iname) : Gateway(iname, NULL, "") {

	}

	virtual ~localGateway() { }

	virtual bool isConnected() = 0;

	virtual void connect() = 0;

	virtual void disconnect() = 0;
};


}//BMS
}//zeitoon

#endif //ZDS_GATEWAY_HPP
