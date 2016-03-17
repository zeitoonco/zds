#include "hdlDevices.hpp"
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
//HDL DEVICES TEST
	HDLDevices HDL('2', '1', '2', "HDLDUMMY", NULL);
	HDL.setSubnetID('3');

}