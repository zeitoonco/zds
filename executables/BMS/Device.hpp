//
// Created by ajl on 3/16/16.
//

#ifndef ZDS_DEVICE_HPP
#define ZDS_DEVICE_HPP


namespace zeitoon {
namespace BMS {

class Gateway;

class Device : public datatypes::DTStruct {
protected:
	std::string address;
	Gateway *containerGateway;
public:
	Device(std::string iname, Gateway container, std::string iAddress) : DTStruct(iname, 0, 0, 0), address(iAddress),
	                                                                     containerGateway(container) { }

	virtual std::string getAddress() { return address; }

	virtual std::string getFullPath() {
		return (containerGateway == NULL ? "" : containerGateway->getFullPath() + PhysicalAddressDelim) +
		       this->getName();
	}

	virtual bool supportSubNetworks() = 0;

	virtual void packetReceived(uint8_t *data, int len) = 0;

	virtual void sendPacket(uint8_t *data, int len) = 0;

};

}
}
#endif //ZDS_DEVICE_HPP
