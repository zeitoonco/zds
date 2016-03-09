//
// Created by inf on 3/7/16.
//

#ifndef GUIDTSTRUCTS_HPP
#define GUIDTSTRUCTS_HPP

#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"
#include "datatypes/DTTableString.hpp"

using namespace zeitoon::datatypes;
namespace zeitoon {
namespace GUI {

class DSListOfLists : public DTStruct {
public:
	DTSet<DTString> ListOfLists = {"ListOfLists"};

	static std::string getStructName() {
		return "DSListOfLists";
	}

	static int getStructVersion() {
		return 1;
	}

	DSListOfLists() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}

	DSListOfLists(std::string data) :
			DSListOfLists() {
		this->fromString(data);
	}

	DSListOfLists(std::vector<DTString> iListOfList) :
			DSListOfLists() {
		ListOfLists.addRange(iListOfList);
	}
};

class DSRegList : public DTStruct {
public:
	DTString name = {"name"};
	DTString content = {"content"};

	static std::string getStructName() {
		return "DSRegList";
	}

	static int getStructVersion() {
		return 1;
	}

	DSRegList() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}

	DSRegList(std::string data) :
			DSRegList() {
		this->fromString(data);
	}

	DSRegList(std::string iname, std::string icontent) :
			DSRegList() {
		name = iname;
		content = icontent;
		this->list.push_back(&name);
		this->list.push_back(&content);
	}
};

class DSTemplateInfo : public DTStruct {
public:
	DTString templateName = {"templateName"};
	DTString fileName = {"fileName"};

	static std::string getStructName() {
		return "DSTemplateInfo";
	}

	static int getStructVersion() {
		return 1;
	}

	DSTemplateInfo() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}

	DSTemplateInfo(std::string data) :
			DSTemplateInfo() {
		this->fromString(data);
	}

	DSTemplateInfo(std::string itemplateName, std::string ifileName) :
			DSTemplateInfo() {
		templateName = itemplateName;
		fileName = ifileName;
		this->list.push_back(&templateName);
		this->list.push_back(&fileName);
	}
};

class DSRegTemplate : public DTStruct {
public:
	DTString templateName = {"templateName"};
	DTString fileName = {"fileName"};
	DTString content = {"content"};

	static std::string getStructName() {
		return "DSRegTemplate";
	}

	static int getStructVersion() {
		return 1;
	}

	DSRegTemplate() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}

	DSRegTemplate(std::string data) :
			DSRegTemplate() {
		this->fromString(data);
	}

	DSRegTemplate(std::string itemplateName, std::string ifileName, std::string icontent) :
			DSRegTemplate() {
		templateName = itemplateName;
		fileName = ifileName;
		content = icontent;
		this->list.push_back(&templateName);
		this->list.push_back(&fileName);
		this->list.push_back(&content);
	}
};

class DSTemplateFilesList : public DTStruct {
public:
	DTSet<DTString> TemplateFilesList = {"TemplateFilesList"};

	static std::string getStructName() {
		return "DSTemplateFilesList";
	}

	static int getStructVersion() {
		return 1;
	}

	DSTemplateFilesList() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}

	DSTemplateFilesList(std::string data) :
			DSTemplateFilesList() {
		this->fromString(data);
	}

	DSTemplateFilesList(std::vector<DTString> iTemplateFilesList) :
			DSTemplateFilesList() {
		TemplateFilesList.addRange(iTemplateFilesList);
	}
};

class DSTemplatesList : public DTStruct {
public:
	DTSet<DTString> TemplatesList = {"TemplatesList"};

	static std::string getStructName() {
		return "DSTemplatesList";
	}

	static int getStructVersion() {
		return 1;
	}

	DSTemplatesList() :
			DTStruct(this->getStructName(), this->getStructVersion(), 1, 1) {
	}

	DSTemplatesList(std::string data) :
			DSTemplatesList() {
		this->fromString(data);
	}

	DSTemplatesList(std::vector<DTString> iTemplatesList) :
			DSTemplatesList() {
		TemplatesList.addRange(iTemplatesList);
	}
};


}
}
#endif //ZDS_DTSTRUCTS_HPP
