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
#include <utility/exceptions.hpp>

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

	virtual string toString() const = 0;

	virtual void fromString(string str) = 0;

	static JValue *praseString(string str);

	friend std::ostream &operator<<(std::ostream &stream, JValue &jv) {
		stream << jv.toString();
		return stream;
	}

	virtual JTypes getType() const = 0;

	JValue &operator[](int i) const;

	JValue &operator[](string name) const;

	virtual size_t size() = 0;

	virtual string getValue() const = 0;

	inline string getNameAndType() const {
		return "JValue";
	}

	inline bool operator==(const string &rhs) {
		return (getValue() == rhs);
	}

	inline bool operator!=(const string &rhs) {
		return !(getValue() == rhs);
	}
};

class JItem {
public:
	string name;
	JValue *value;
	JStruct *parent;

	JItem() :
			name(""), value(NULL), parent(NULL) {
	}

	JItem(string json, JStruct *owner = NULL) :
			value(NULL), parent(owner) {
		this->fromString(json);
	}

	JItem(string iname, JValue *ivalue, JStruct *owner = NULL) :
			name(iname), value(ivalue), parent(owner) {
	}

	JItem(string iname, string ivalue, JStruct *owner = NULL) :
			JItem(iname, JValue::praseString(ivalue), owner) {
	}

	~JItem();

	string toString() const;

	void fromString(string str);

	friend std::ostream &operator<<(std::ostream &stream, JItem &jv) {
		stream << jv.toString();
		return stream;
	}

	bool validateString(string str);

	JValue &operator[](int i) const;

	JValue &operator[](string name) const;

	size_t size();

	JTypes type() {
		return value->getType();
	}

	string getValue();

	inline string getNameAndType() const {
		return "JItem[" + name + "]";
	}

	inline bool operator==(const string &rhs) {
		return (getValue() == rhs);
	}

	inline bool operator!=(const string &rhs) {
		return !(getValue() == rhs);
	}
};

class JStruct : public JValue {
public:
	vector<JItem *> fields;

	JStruct() {
	}

	JStruct(string json) {
		this->fromString(json);
	}

	virtual ~JStruct();

	void add(string name, string value);

	void add(string name, JValue *value);

	void addIgnored(string name, string value);

	string toString() const;

	void fromString(string str);

	friend std::ostream &operator<<(std::ostream &stream, JStruct &jv) {
		stream << jv.toString();
		return stream;
	}

	JValue &operator[](int i) const;

	JValue &operator[](string name) const;

	JItem *getField(string name);

	bool contains(string name);

	size_t size();

	JTypes getType() const {
		return JTypes::JTstruct;
	}

	string getValue() const;

	inline string getNameAndType() const {
		return "JStruct";
	}

	inline bool operator==(const string &rhs) {
		return (getValue() == rhs);
	}

	inline bool operator!=(const string &rhs) {
		return !(getValue() == rhs);
	}
};

class JArray : public JValue {
public:
	vector<JValue *> fields;

	JArray() {
	}

	JArray(string json) {
		this->fromString(json);
	}

	JArray(vector<string> list) {
		for (size_t i = 0; i < list.size(); i++)
			add(list[i]);
	}

	virtual ~JArray();

	void add(string value);

	void addIgnored(string value);

	void remove(JValue *value);

	void removeAt(size_t index);

	size_t size();

	string toString() const;

	void fromString(string str);

	friend std::ostream &operator<<(std::ostream &stream, JArray &jv) {
		stream << jv.toString();
		return stream;
	}

	JValue &operator[](int i) const {
		return *(fields[i]);
	}

	JTypes getType() const {
		return JTypes::JTarray;
	}

	string getValue() const;

	inline string getNameAndType() const {
		return "JArray";
	}

	inline bool operator==(const string &rhs) {
		return (getValue() == rhs);
	}

	inline bool operator!=(const string &rhs) {
		return !(getValue() == rhs);
	}
};

class JVariable : public JValue {
protected:
	string value;
	bool isString;
public:
	JVariable(string ivalue) {
		fromString(ivalue);
	}

	bool isNull() const;

	bool isInt() const;

	bool isFloat() const;

	bool isBoolian() const;

	long long int toInt() const;

	unsigned long long int toUInt() const;

	long double toFloat() const;

	bool toBoolian() const;

	string getValue() const;

	void setNULL();

	void setValue(string val);

	void setValue(long long int val);

	void setValue(unsigned long long int val);

	void setValue(long double val);

	void setValue(bool val);

	string toString() const;

	void fromString(string str);

	friend std::ostream &operator<<(std::ostream &stream, JVariable &jv) {
		stream << jv.toString();
		return stream;
	}

	size_t size();

	JTypes getType() const {
		return JTypes::JTvariable;
	}

	inline string getNameAndType() const {
		return "JVariable";
	}

	inline bool operator==(const string &rhs) {
		return (getValue() == rhs);
	}

	inline bool operator!=(const string &rhs) {
		return !(getValue() == rhs);
	}
};

class JIgnored : public JValue {
public:
	string value;

	JIgnored(string ivalue) {
		fromString(ivalue);
	}

	string toString() const;

	void fromString(string str);

	friend std::ostream &operator<<(std::ostream &stream, JIgnored &jv) {
		stream << jv.toString();
		return stream;
	}

	size_t size();

	JTypes getType() const {
		return JTypes::JTignored;
	}

	string getValue() const;

	inline string getNameAndType() const {
		return "JIgnored";
	}

	inline bool operator==(const string &rhs) {
		return (getValue() == rhs);
	}

	inline bool operator!=(const string &rhs) {
		return !(getValue() == rhs);
	}
};

}
}

#endif /* JSONPARSER_HPP */
