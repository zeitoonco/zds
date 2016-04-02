//
// Created by inf on 3/29/16.
//

#ifndef ZDS_PGCONFIG_HPP
#define ZDS_PGCONFIG_HPP

#include "utility/ConfigManager.hpp"

namespace zeitoon {
namespace pgdatabase {

class PGConfig : public ConfigManager {
public:
	DTString serviceID = {"serviceID"};
	DTString adminUsrPGDB = {"adminUsrPGDB"};
	DTString adminPwPGDB = {"adminPwPGDB"};

	PGConfig() : ConfigManager("PGDatabase", "PGConfig") {
		this->add(&serviceID);
		this->add(&adminUsrPGDB);
		this->add(&adminPwPGDB);
	}

	~PGConfig() {
		this->save();
	}

};

}
}
extern zeitoon::pgdatabase::PGConfig PGconfiguration;
#endif //ZDS_PGCONFIG_HPP
