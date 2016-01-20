/*
 * Jsonparser.h
 *
 *  Created on: Aug 29, 2015
 *      Author: mahdi bakhshi
 */

#ifndef JSONPARSER_HPP
#define JSONPARSER_HPP

#include <iostream>
#include <vector>
#include <string.h>
#include "utility/exceptionex.hpp"

using namespace std;

namespace zeitoon {
namespace utility {

class JStruct;

enum JTypes {
	JTstruct, JTarray, JTvariable, JTignored
};

class JValue {
public:
	virtual ~JValue() {
	}
	virtual string toString() = 0;
	virtual void fromString(string str) = 0;
	static JValue* praseString(string str);
	friend std::ostream& operator<<(std::ostream& stream, JValue& jv) {
		stream << jv.toString();
		return stream;
	}
	virtual JTypes getType()=0;
	JValue& operator [](int i);
	JValue& operator [](string name);
	virtual size_t size()=0;

	inline string getNameAndType() {
		return "JValue";
	}
};

class JItem {
public:
	string name;
	JValue* value;
	JStruct* parent;

	JItem() :
			name(""), value(NULL), parent(NULL) {
	}
	JItem(string json, JStruct* owner = NULL) :
			value(NULL), parent(owner) {
		this->fromString(json);
	}
	JItem(string iname, JValue* ivalue, JStruct* owner = NULL) :
			name(iname), value(ivalue), parent(owner) {
	}
	JItem(string iname, string ivalue, JStruct* owner = NULL) :
			JItem(iname, JValue::praseString(ivalue), owner) {
	}

	~JItem();

	string toString();
	void fromString(string str);
	friend std::ostream& operator<<(std::ostream& stream, JItem& jv) {
		stream << jv.toString();
		return stream;
	}
	bool validateString(string str);
	JValue& operator [](int i);
	JValue& operator [](string name);
	size_t size();
	JTypes type() {
		return value->getType();
	}

	inline string getNameAndType() {
		return "JItem[" + name + "]";
	}
};

class JStruct: public JValue {
public:
	vector<JItem*> fields;

	JStruct() {
	}
	JStruct(string json) {
		this->fromString(json);
	}
	virtual ~JStruct();

	void add(string name, string value);
	void addIgnored(string name, string value);
	string toString();
	void fromString(string str);
	friend std::ostream& operator<<(std::ostream& stream, JStruct& jv) {
		stream << jv.toString();
		return stream;
	}
	JValue& operator [](int i);
	JValue& operator [](string name);
	size_t size();
	JTypes getType() {
		return JTypes::JTstruct;
	}

	inline string getNameAndType() {
		return "JStruct";
	}
};

class JArray: public JValue {
public:
	vector<JValue*> fields;

	JArray() {
	}
	JArray(string json) {
		this->fromString(json);
	}
	virtual ~JArray();

	void add(string value);
	void addIgnored(string value);
	void remove(JValue* value);
	void removeAt(int index);
	size_t size();
	string toString();
	void fromString(string str);
	friend std::ostream& operator<<(std::ostream& stream, JArray& jv) {
		stream << jv.toString();
		return stream;
	}
	JValue& operator [](int i) {
		return *(fields[i]);
	}
	JTypes getType() {
		return JTypes::JTarray;
	}

	inline string getNameAndType() {
		return "JArray";
	}
};

class JVariable: public JValue {
protected:
	string value;

public:
	JVariable(string ivalue) {
		fromString(ivalue);
	}
	bool isInt();
	bool isFloat();
	bool isBoolian();
	long long int toInt();
	unsigned long long int toUInt();
	long double toFloat();
	bool toBoolian();
	string getValue();
	void setValue(string val);
	void setValue(long long int val);
	void setValue(unsigned long long int val);
	void setValue(long double val);
	void setValue(bool val);
	string toString();
	void fromString(string str);
	friend std::ostream& operator<<(std::ostream& stream, JVariable& jv) {
		stream << jv.toString();
		return stream;
	}
	size_t size();
	JTypes getType() {
		return JTypes::JTvariable;
	}

	inline string getNameAndType() {
		return "JVariable";
	}

};

class JIgnored: public JValue {
public:
	string value;

	JIgnored(string ivalue) {
		fromString(ivalue);
	}
	string toString();
	void fromString(string str);
	friend std::ostream& operator<<(std::ostream& stream, JIgnored& jv) {
		stream << jv.toString();
		return stream;
	}
	size_t size();
	JTypes getType() {
		return JTypes::JTignored;
	}

	inline string getNameAndType() {
		return "JIgnored";
	}
};

}
}

#endif /* JSONPARSER_HPP */
