/*
 * SettingMediator.hpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#ifndef MEDIATOR_SETTINGMEDIATOR_HPP_
#define MEDIATOR_SETTINGMEDIATOR_HPP_

#include "utility/exceptionex.hpp"

namespace zeitoon {
namespace utility {

class ServerMediator;

class SettingMediator {
private:
	ServerMediator *sm;

public:
	SettingMediator(ServerMediator *ism) :
			sm(ism) {
	}
	virtual ~SettingMediator() {
	}

	void add(string name, string defValue, bool isPublic = false, bool readOnly = false);
	string get(string name);
	void getAsync(string name);
	void remove(string name);
	void set(string name, string value);
};

} /* namespace utility */
} /* namespace zeitoon */

#endif /* MEDIATOR_SETTINGMEDIATOR_HPP_ */
