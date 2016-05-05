//
// Created by inf on 3/28/16.
//

#ifndef ZDS_UMCONFIG_HPP
#define ZDS_UMCONFIG_HPP

#include "utility/ConfigManager.hpp"

namespace zeitoon {
namespace usermanagement {


class UMConfig : public ConfigManager {
public:
	DTString serviceID = {"serviceID"};
	DTString serverIP = {"serverIP"};
	DTString serverPort = {"serverPort"};

	UMConfig() : ConfigManager("UserManagement", "UMConfig") {
		this->add(&serviceID);
		this->add(&serverIP);
		this->add(&serverPort);
	}

	~UMConfig() {
		this->save();
	}
};


}
}
extern zeitoon::usermanagement::UMConfig UMconfig;
#endif //ZDS_UMCONFIG_HPP
