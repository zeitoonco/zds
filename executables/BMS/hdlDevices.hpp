//
// Created by inf on 3/17/16.
//

#ifndef ZDS_HDLDEVICES_HPP
#define ZDS_HDLDEVICES_HPP


#include <iostream>
#include "protocolHDL.hpp"
#include "datatypes/dtmultifieldtypes.hpp"
#include "network/UDPClient.hpp"

using namespace zeitoon::datatypes;

class HDLDevices : public DTStruct {

	typedef std::function<void(uint8_t *buff, int length)> sndPtr;
	DTInteger<uint16_t> ODT;
	DTInteger<uint8_t> ODI;
	DTInteger<uint8_t> OSI;
	sndPtr send;
	MACAddress DMAC;

public:
	HDLDevices(uint16_t iODT, uint8_t iODI, uint8_t iOSI, std::string deviceName, sndPtr ptr) :
			DTStruct(deviceName, 0, 0, 0), ODT("ODT"), ODI("ODI"), OSI("OSI"), send(ptr), DMAC("MAC") {
		ODT.operator=(iODT);
		ODI.operator=(iODI);
		OSI.operator=(iOSI);
		add(&ODT);
		add(&ODI);
		add(&OSI);
		add(&DMAC);
	}


	uint8_t getSubnetID() {
		return OSI.getValue();
	}

	void setSubnetID(uint8_t id) {
		OSI.operator=(id);
		Packet myPack;
		myPack.SetOriginalDeviceType(serverDevicetype);
		myPack.SetOriginalSubnetID(serverDeviceID);
		myPack.SetOriginalDeviceID(serverSubnetID);
		myPack.SetTargetSubnetID(OSI.getValue());
		myPack.SetTargetDeviceID(ODI.getValue());
		myPack.SetOperationCode(0xF005);

		PCSetDeviceID temp;
		temp.setMac(DMAC);
		temp.DeviceID = ODI.getValue();
		temp.SubnetID = id;

		myPack.SetContent((uint8_t *) &temp, 10);
		myPack.FixLength();
		myPack.FixHeader();
		myPack.FixCRC();
		send(myPack.Generate(), myPack.length);
		//TODO: Fire Event



	}

	uint8_t getDeviceID() {
		return ODI.getValue();
	}

	void setDeviceID(uint8_t id) {
		ODI.setValue(id);
	}

	uint16_t getDeviceType() {
		return ODT.getValue();
	}

	uint8_t *getMAC() {

		Packet myPack;
		myPack.SetTargetSubnetID(OSI.getValue());
		myPack.SetTargetDeviceID(ODI.getValue());
		myPack.SetOperationCode(0xF003);
		myPack.FixCRC();
		myPack.FixLength();
		myPack.FixHeader();
		send(myPack.Generate(), myPack.length);
		//TODO: Fire Event//getfullpath + Operation Name ; response

	}

	void setMAC(uint8_t *mac) {
		Packet myPack;
		myPack.SetTargetSubnetID(OSI.getValue());
		myPack.SetTargetDeviceID(ODI.getValue());
		myPack.SetOperationCode(0xF001);
		myPack.SetContent(mac, 8);
		myPack.FixLength();
		myPack.FixHeader();
		myPack.FixCRC();
		send(myPack.Generate(), myPack.length);
		//TODO: Fire Event//getfullpath + Operation Name ; response
	}

	char *getRemarks() {
		Packet myPack;
		myPack.SetTargetSubnetID(OSI.getValue());
		myPack.SetTargetDeviceID(ODI.getValue());
		myPack.SetOperationCode(0x000E);
		myPack.FixLength();
		myPack.FixHeader();
		myPack.FixCRC();
		send(myPack.Generate(), myPack.length);
		//TODO: Fire Event//getfullpath + Operation Name ; response
	}

	void setRemarks(char *remark) {
		Packet myPack;
		myPack.SetTargetSubnetID(OSI.getValue());
		myPack.SetTargetDeviceID(ODI.getValue());
		myPack.SetOperationCode(0x0010);
		myPack.SetContent((uint8_t *) remark, 20);
		myPack.FixLength();
		myPack.FixHeader();
		myPack.FixCRC();
		send(myPack.Generate(), myPack.length);
		//TODO: Fire Event//getfullpath + Operation Name ; response
	}

	void ping() {
		getRemarks();
	}

	void packetReceived(uint8_t *rPack, int length) {
		//TODO: replace all couts with a proper event
		Packet recPack(rPack, length);

		uint8_t *temp = recPack.GetContent();
		switch (recPack.GetOperationCode()) {
			case (0xF004)://Read MAC Addr
				DMAC.setValue(*((uint64_t *) recPack.GetContent()));
				std::cout << this->getName() << ":  Fetched MAC address" << std::endl;
				break;
			case (0x000F):
				std::cout << this->getName() << ": Fetched device remarks" << std::endl;
				break;
			case (0x0011):
				if (*temp == 0xF5) {
					std::cout << this->getName() << ":  Failed to set remarks" << std::endl;
				}
				if (*temp == 0xF8) {
					std::cout << this->getName() << ":  Successfully set remarks" << std::endl;
				}
				break;
			case (0xF002):
				if (*temp == 0xF5) {
					std::cout << this->getName() << ":  Failed to modifiy subnetID/deviceID/MAC addr" << std::endl;
				}
				if (*temp == 0xF8) {
					std::cout << this->getName() << ":  Successfully modified subnetID/deviceID/MAC addr" << std::endl;
				}
			default:
				return;


		}
	}
};

#endif //ZDS_HDLDEVICES_HPP
