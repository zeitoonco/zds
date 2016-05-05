//
// Created by ajl on 3/16/16.
//

#ifndef ZDS_BMSCORE_HPP
#define ZDS_BMSCORE_HPP

#include <datatypes/dtmultifieldtypes.hpp>
#include "Gateway.hpp"

namespace zeitoon {
namespace BMS {

class BMSCore : public datatypes::DTStruct {
	datatypes::DTSet<Gateway> localGateways;
public:
	BMSCore() : datatypes::DTStruct("BMSCore", 0, 0, 0), localGateways("localGateways") {
		this->add(localGateways);
	}


	void newGateway() {

	}

	void removeGateway() {

	}

	void editGateway() {

	}

	void moveGatewayToGateway() {

	}

	void newDevice() {

	}

	void removeDevice() {

	}

	void editDevice() {

	}

	void moveDeviceToGateway() {

	}

	void newZone() {

	}

	void removeZone() {

	}

	void editZone() {

	}

	void moveDeviceToZone() {

	}

	void moveGatewayToZone() {

	}

	void removeDeviceFromZone() {

	}

	void removeGatewayFromZone() {

	}

};

}//BMS
}//zeitoon

#endif //ZDS_BMSCORE_HPP
