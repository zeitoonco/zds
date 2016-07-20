//
// Created by inf on 3/27/16.
//

#ifndef ZDS_CONFIGMANAGER_HPP
#define ZDS_CONFIGMANAGER_HPP

#include "datatypes/dtmultifieldtypes.hpp"
#include "utility/FileSystem.hpp"
#include <utility/exceptions.hpp>
#include <iostream>

#define baseDir "/etc/zds/"

using namespace zeitoon::datatypes;


namespace zeitoon {
namespace utility {

class ConfigManager : protected DTStruct {
	FileSystem fs;
	std::string fileName;
	DTStruct::iterator iter;
	int lastStaticConfigIndex = -1;
	//std::vector<std::string> dynamicFields;
public:
	~ConfigManager() {

	}

	void removeConfigFile() {
		fs.fileRemove(fileName);
		if (fs.fileExist(fileName))
			EXTfilesystemFailure("Config file remove failed");
	}

	ConfigManager(std::string moduleName, std::string ifileName) :
			DTStruct(moduleName + "Config", 0, 0, 0), fs(baseDir + moduleName) {
		this->fileName = ifileName;


	}

	void load() { //fixme:breaks when file is empty (first run, somehow file was created but was empty)
		if (lastStaticConfigIndex == -1)
			lastStaticConfigIndex = this->length();
		if (fs.fileExist(fileName))
			this->fromString(fs.fileRead(fileName), true);
	}

	void save() {
		fs.fileUpdate(fileName, this->toString(), true);
	}

	void addConfig(std::string config, std::string defaultValue) {
		/**TODO: Bug Found -> Dynamic fields will add the same value with " a new pair of double quotation"
		 * EX: ->>  FIRST ATTEMPT TO ADD "Tessio" and its value:
		 *          {"serviceID":"gg458569412gg","Tessio":"Dumbo"}
		 *          SECOND ATTEMPT:
		 *          {"serviceID":"gg458569412gg","Tessio":""Dumbo""}
		 *          THIRD:
		 *          {"serviceID":"gg458569412gg","Tessio":"""Dumbo"""}
		**/
		for (int i = 0; i < this->length(); i++)
			if (this->operator[](i)->getName() == config)
				EXTinvalidName("config name already exists");
		DTString *temp = new DTString(config, defaultValue);
		this->add(temp, true);

	}

	void removeConfig(std::string config) {
		for (int i = this->lastStaticConfigIndex; i < this->length(); i++) {
			if (this->operator[](i)->getName() == config) {
				this->removeAt(i);
				return;
			}
		}
		EXTinvalidName("Invalid config name");
	}

	void set(std::string config, std::string value) {
		for (int i = this->lastStaticConfigIndex; i < this->length(); i++) {
			if (this->operator[](i)->getName() == config) {
				((DTString *) this->operator[](i))->setValue(value);
				return;
			}
		}
		EXTinvalidName("Invalid config name");
	}

	std::string get(std::string config) {
		for (int i = 0; i < this->length(); i++) {
			if (this->operator[](i)->getName() == config) {
				return ((DTString *) this->operator[](i))->getValue();
			}
		}
		EXTinvalidName("Invalid config name");
	}
    bool exists(std::string configName){
        for (int i= 0; i < this->length(); i++){
            if (this->contains(configName))
                return true;
        }
        return false;
    }

	std::string getNameAndType() {
		return "ConfigManager";
	}
};

}
}

#endif //ZDS_CONFIGMANAGER_HPP
