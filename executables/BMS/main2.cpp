//
// Created by inf on 3/15/16.
//
#include <iostream>
#include "protocolHDL.hpp"
#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"
//#include "control.hpp"
#include "network/UDPClient.hpp"

using namespace zeitoon::datatypes;


class HDLDevices : public DTStruct {
	typedef std::function<void(uint8_t *buff, int length)> sndPtr;
	///cons =  ODT, ODI, OSI
	// har function shoudl take OP-Code(and its params), TDI,TSI
	DTInteger<uint16_t> ODT;

	DTInteger<uint8_t> ODI;

	DTInteger<uint8_t> OSI;
	sndPtr send;
	MACAddress DMAC;


public:
	HDLDevices(uint16_t iODT, uint8_t iODI, uint8_t iOSI, std::string deviceName, sndPtr ptr) : DTStruct(deviceName, 0,
	                                                                                                     0, 0),
	                                                                                            ODT("ODT"), ODI("ODI"),
	                                                                                            OSI("OSI"), send(ptr),
	                                                                                            DMAC("MAC") {
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
		uint8_t tempArray[10];
		for (int i = 0; i < 8; i++) {
			tempArray[i] = temp.MAC[i];
		}
		tempArray[8] = temp.SubnetID;
		tempArray[9] = temp.DeviceID;

		myPack.SetContent(tempArray, 10);
		myPack.FixCRC();
		myPack.FixLength();
		myPack.FixHeader();
		for (int i = 0; i < 10; i++) {
			std::cout << tempArray[i] << " "
		}
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

	uint8_t *getMAC();


	void setMAC(uint8_t *mac);

	char *getRemarks();

	void setRemarks(char *r);

	void ping();
};

int main() {

	/*uint8_t data[3] = {01, 01, 33};
	Packet myPack;
	myPack.SetOriginalDeviceType(257);
	myPack.SetOriginalSubnetID(2);
	myPack.SetOriginalDeviceID(2);
	myPack.SetTargetSubnetID(1);
	myPack.SetTargetDeviceID(1);
	myPack.SetOperationCode(514);
	myPack.SetContent(data, 3);
	myPack.FixCRC();
	myPack.FixLength();
	myPack.FixHeader();
	uint8_t *temp = myPack.Generate();
	std::cout << myPack.toString() << endl;
	zeitoon::utility::UDPClient sendPacket;
	sendPacket.send("127.0.0.1", 15001, (char *) temp, myPack.length);*/

	HDLDevices HDL();

}