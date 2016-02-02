/*
 * SettingMediator.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: ajl
 */

#include "SettingMediator.hpp"
#include "utility/utility.hpp"
#include "utility/jsonParser.hpp"
#include "ServerMediator.hpp"

namespace zeitoon {
namespace utility {

void SettingMediator::add(string name, string defValue, bool isPublic, bool readOnly) {
	sm->communication.runCommand("registersetting",
			"{\"name\" : \"" + name + "\" , \"defvalue\" : \"" + defValue + "\" , \"public\" : \"" + Strings::toString(isPublic) + "\" , \"readonly\" : \""
					+ Strings::toString(readOnly) + "\"}");
}
string SettingMediator::get(string name) {
	string respData = sm->communication.runCommandSync("getsetting", "{\"name\" : \"" + name + "\"}");
	JStruct js(respData);
	return js["value"].getValue();
}
void SettingMediator::getAsync(string name) {
	sm->communication.runCommand("getsetting", "{\"name\" : \"" + name + "\"}");
}
void SettingMediator::remove(string name) {
	sm->communication.runCommand("removesetting", "{\"name\" : \"" + name + "\"}");
}
void SettingMediator::set(string name, string value) {
	sm->communication.runCommand("setsetting", "{\"name\" : \"" + name + "\", \"value\" : \"" + value + "\"}");
}

} /* namespace utility */
} /* namespace zeitoon */
