//
// Created by inf on 5/14/16.
//

#ifndef ZDS_GUICONFIG_HPP
#define ZDS_GUICONFIG_HPP

#include "utility/ConfigManager.hpp"

namespace zeitoon {
namespace GUI {
class GUIConfig : public zeitoon::utility::ConfigManager {
public:
	DTString serviceID = {"serviceID"};
	DTString serverIP = {"serverIP"};
	DTString WSserverPort = {"WSserverPort"};
	DTString serverPort = {"serverPort"};

	GUIConfig() : ConfigManager("GUI", "GUIConfig") {
		this->add(&serviceID);
		this->add(&WSserverPort);
		this->add(&serverIP);
		this->add(&serverPort);
	}

	~GUIConfig() {
		this->save();
	}
};

}
}
extern zeitoon::GUI::GUIConfig GUIConfiguration;
#endif //ZDS_GUICONFIG_HPP
