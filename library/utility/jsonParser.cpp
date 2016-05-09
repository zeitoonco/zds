/*
 * jsonParser.cpp
 *
 *  Created on: Aug 31, 2015
 *      Author: mahdi bakhshi
 */

#include <sstream>
#include "utility/jsonParser.hpp"
#include "utility/utility.hpp"
#include "utility/exceptionex.hpp"

using namespace zeitoon;
using namespace utility;

//-------------------------------jItem
string JItem::toString() const {
	stringstream str;
	if (name.length() > 0)
		str << "\"" << name << "\":";
	str << value->toString();
	return str.str();
}

void JItem::fromString(string str) {
	unsigned int pos;
	string val;
	str = Strings::trim(str);
	if (str[0] == '{') { //no name
		name = "";
		val = str;
	} else {
		pos = str.find(':', 0);
		if (pos == string::npos)
			EXTcantParseString("invalid JSON string. Cant find name of item.");
		int pos2 = str.find('"');
		int pos3 = str.find('"', pos2 + 1);
		name = str.substr(pos2 + 1, pos3 - pos2 - 1);
		val = str.substr(pos + 1, str.size());
	}
	value = JValue::praseString(val);
}

bool JItem::validateString(string str) {
	int NumStruct = 0, NumArray = 0;
	for (unsigned int i = 0; i < str.size(); i++) {
		if (str[i] == '{')
			NumStruct++;
		else if (str[i] == '}')
			NumStruct--;
		else if (str[i] == '[')
			NumArray++;
		else if (str[i] == ']')
			NumArray--;
		else if (str[i] == '"') {
			size_t pos = JSONUtility::getEndOfString(str, i + 1);
			if (pos != string::npos)
				i = pos;
			else
				EXTcantParseString(
						"cant find end of string:" + Strings::itoa(i));
		}
	}
	if (NumStruct != 0)
		EXTcantParseString("Bracket '{' or '}' missing.");
	if (NumArray != 0)
		EXTcantParseString("Bracket '[' or ']' missing.");
	/*try {
	 fromString(str);
	 } catch (exception& ex) {
	 return false;
	 }*/
	return true;
}

JValue &JItem::operator[](int i) const {
	if (value == NULL)
		EXTnullValue("Value is empty");
	if (value->getType() != JTypes::JTarray
	    && value->getType() != JTypes::JTstruct)
		EXTdataTypeMismatch(
				"index operator can only used on array and struct values");
	if (value->getType() == JTypes::JTarray) {
		return (*((JArray *) value))[i];
	} else { //if (value->getType() == JTypes::JTstruct) {
		return (*((JStruct *) value))[i];
	}
}

JValue &JItem::operator[](string name) const {
	if (value->getType() != JTypes::JTstruct)
		EXTdataTypeMismatch(
				"name index operator can only used on struct values");
	return (*((JStruct *) value))[name];
}

size_t JItem::size() {
	return value->size();
}

JItem::~JItem() {
	delete value;
}

string JItem::getValue() {
	return this->value->getValue();
}

//-------------------------------jValue
JValue *JValue::praseString(string str) {
	string trimed = Strings::trim(str);
	JValue *temp;
	if (trimed[0] == '{') {
		temp = new JStruct;
		temp->fromString(trimed);
		return temp;
	} else if (trimed[0] == '[') {
		temp = new JArray;
		temp->fromString(trimed);
		return temp;
	} else {
		temp = new JVariable(trimed);
		return temp;
	}
}

JValue &JValue::operator[](int i) const {
	if (getType() != JTypes::JTarray && getType() != JTypes::JTstruct)
		EXTdataTypeMismatch(
				"index operator can only used on array and struct values");
	if (getType() == JTypes::JTarray) {
		return (*((JArray *) this))[i];
	} else { //if (value->getType() == JTypes::JTstruct) {
		return (*((JStruct *) this))[i];
	}
}

JValue &JValue::operator[](string name) const {
	if (getType() != JTypes::JTstruct)
		EXTdataTypeMismatch(
				"named index operator can only used on struct values");
	return (*((JStruct *) this))[name];
}

//-------------------------------jVariable
bool JVariable::isNull() const {
	return (seq("NULL", value));
}

bool JVariable::isInt() const {
	if (value.length() == 0)
		return false;
	string str = Strings::trim(value, "\"");
	for (unsigned int i = 0; i < value.size(); i++)
		if ((str[i] < 48 || str[i] > 57)    //Numbers
		    && !((str[i] == 45 || str[i] == 43) && i == 0))    // '-','+'
			return false;
	if ((str[0] == 45 || str[0] == 43) && str.length() == 1)
		return false;
	return true;
}

bool JVariable::isFloat() const { //TODO: make sure that 'e','E','.' r only used once?
	if (value.length() == 0)
		return false;
	int cn, cd, ce;
	cn = ce = cd = 0;
	string str = Strings::trim(value, "\"");
	for (unsigned int i = 0; i < value.size(); i++)
		if (str[i] >= 48 && str[i] <= 57)    //Numbers
			cn += 1;
		else if (str[i] == 46)
			cd += 1;
		else if ((str[i] == 45 || str[i] == 43) && i == 0)    // '.','-','+'
			;//ok
		else
			return false;
	//&& str[i] != 101 && str[i] != 69)    //e & E
	if (cn<0 || cd>1)
		return false;
	return true;
}

bool JVariable::isBoolian() const {
	if (value.length() == 0)
		return false;
	string tval = Strings::toLower(Strings::trim(value, "\""));
	if (tval != "true" && tval != "false")
		return false;
	return true;
}

long long int JVariable::toInt() const {
	if (!isInt())
		EXTcantParseString("can't convert value to int");
	string tmp = Strings::trim(value, "\"");
	stringstream str(tmp);
	long long int val;
	str >> val;
	return val;
}

unsigned long long int JVariable::toUInt() const {
	if (!isInt())
		EXTcantParseString("can't convert value to int");
	string tmp = Strings::trim(value, "\"");
	stringstream str(tmp);
	unsigned long long int val;
	str >> val;
	return val;
}

long double JVariable::toFloat() const {
	if (!isFloat())
		EXTcantParseString("can't convert value to float");
	string tmp = Strings::trim(value, "\"");
	stringstream str(tmp);
	long double val;
	str >> val;
	return val;
}

bool JVariable::toBoolian() const {
	if (!isBoolian())
		EXTcantParseString("can't convert value to boolean");
	string tval = Strings::toLower(Strings::trim(value, "\""));
	if (tval == "true")
		return true;
	else if (tval != "false")
		return false;
	else
		EXTcantParseString("can't parse value to boolean");
}

string JVariable::getValue() const {
	return value;
}

void JVariable::setValue(bool val) {
	value = Strings::toString(val);
}

void JVariable::setValue(long long val) {
	value = Strings::toString(val);
}

void JVariable::setValue(unsigned long long int val) {
	value = Strings::toString(val);
}

void JVariable::setValue(long double val) {
	value = Strings::toString(val);
}

void JVariable::setValue(string val) {
	value = val;
}

void JVariable::fromString(string str) {
	str = Strings::trim(str);
	if (str[0] == '"') {
		str = str.substr(1, str.size() - 2);
		str = JSONUtility::decodeString(str);
	}
	value = str;
}

string JVariable::toString() const {
	if (this->isFloat() || this->isBoolian() || this->isNull())
		return Strings::toLower(value);
	else
		return "\"" + JSONUtility::encodeString(value) + "\"";
}

size_t JVariable::size() {
	if (isFloat() || isInt() || isBoolian())
		return 1;
	else
		return this->value.size();
}

//-------------------------------jArray
string JArray::toString() const {
	stringstream str;
	str << '[';
	for (vector<JValue *>::const_iterator i = fields.begin(); i < fields.end(); i++) {
		str << (*i)->toString();
		if (i + 1 != fields.end())
			str << ',';
	}
	str << ']';
	return str.str();
}

JArray::~JArray() {
	for (vector<JValue *>::iterator i = fields.begin(); i != fields.end(); i++)
		delete (*i);
}

void JArray::fromString(string str) {
	str = Strings::trim(str);
	if (str[0] != '[')
		EXTcantParseString("can't parse string as array");
	str = Strings::trim(str.substr(1, str.size() - 2));
	if (str.length() == 0)
		return;
	string parsed;
	size_t q = 0, j = 0;
	bool ready = false;
	for (size_t i = 0; i <= str.size(); i++) {
		if (str[i] == '"') {
			j = JSONUtility::getEndOfString(str, i);
		} else if (str[i] == '{') {
			j = JSONUtility::getEndOfStruct(str, i);
		} else if (str[i] == '[') {
			j = JSONUtility::getEndOfArray(str, i);
		} else if (str[i] == ',') {
			parsed = str.substr(q, i - q);
			ready = true;
			q = i + 1;
		}
		if (!ready) {
			if (j == string::npos) {
				EXTcantParseString("Syntax error on " + Strings::itoa(j));
			} else if (j != 0)
				i = j;
			j = 0;
		}
		if (i >= str.size()) {
			parsed = str.substr(q);
			ready = true;
		}
		if (ready) {
			JValue *temp;
			temp = JValue::praseString(parsed);
			fields.push_back(temp);
		}
		ready = false;
	}

}

void JArray::add(string value) {
	fields.push_back(JValue::praseString(value));
}

void JArray::addIgnored(string value) {
	fields.push_back((JValue *) new JIgnored(value));
}

void JArray::remove(JValue *value) {
	for (size_t i = 0; i < fields.size(); i++)
		if (value == fields[i]) {
			fields.erase(fields.begin() + i);
			return;
		}
}

void JArray::removeAt(size_t index) {
	if (index < 0 || index >= fields.size())
		EXToutOfRange("Invalid index.");
	fields.erase(fields.begin() + index);
}

size_t JArray::size() {
	return fields.size();
}

string JArray::getValue() const {
	return this->toString();
}

//-------------------------------jStruct

//todo : check new&delete in from strings,destructor

string JStruct::toString() const {
	stringstream str;
	str << '{';
	for (vector<JItem *>::const_iterator i = fields.begin(); i != fields.end(); i++) {
		str << (*i)->toString();
		if (i + 1 != fields.end())
			str << ',';
	}
	str << '}';
	return str.str();
}

void JStruct::fromString(string str) {//todo:too expensive, NEED rewrite
	size_t pos, rePos, j = 0;
	pos = str.find('{');
	rePos = JSONUtility::getEndOfStruct(str, pos);//rePos = str.rfind('}');
	if (pos == string::npos || rePos == string::npos) {
		EXTcantParseString("can't parse string as structure");
	}
	str = str.substr(pos + 1, rePos - pos - 1);
	string parsed, name, value;
	int q = 0;
	bool check = true, ready = false;
	for (size_t i = 0; i <= str.size(); i++) {
		if (str[i] == '"') {
			if (check) {
				q = i;
				check = false;
			}
			j = JSONUtility::getEndOfString(str, i);
		} else if (str[i] == '{') {
			j = JSONUtility::getEndOfStruct(str, i);
		} else if (str[i] == '[') {
			j = JSONUtility::getEndOfArray(str, i);
		} else if (str[i] == ',') {
			parsed = str.substr(q, i - q);
			ready = true;
			check = true;
			q = i + 1;
		}
		if (!ready) {
			if (j == string::npos) {
				EXTcantParseString("Syntax error on " + Strings::itoa(j));
			} else if (j != 0)
				i = j;
			j = 0;
		}
		if (i >= str.size()) {
			parsed = str.substr(q);
			ready = true;
		}
		if (ready) {
			size_t x = 0, y = 0;
			y = parsed.find('"', x + 1);
			if (y == string::npos) {
				EXTcantParseString(
						"Syntax error. Can't find character '\"' for name part of JSON field in '"
						+ parsed + "'");
			}
			x = parsed.find(':', y + 1);
			if (x == string::npos) {
				EXTcantParseString(
						"Syntax error. Can't find character ':' for JSON field in '"
						+ parsed + "'");
			}
			fields.push_back(new JItem(parsed));
		}
		ready = false;
	}
}

JValue &JStruct::operator[](int i) const {
	return *(fields[i]->value);
}

JValue &JStruct::operator[](string name) const {
	for (auto itm : fields) {
		if (Strings::compare(itm->name, name, false) == 0)
			return *(itm->value);
	}
	EXTinvalidName("cant find " + name);
}

JItem *JStruct::getField(string name) {
	for (auto itm : fields) {
		if (Strings::compare(itm->name, name, false) == 0)
			return itm;
	}
	return NULL;
}

bool JStruct::contains(string name) {
	for (auto itm : fields)
		if (Strings::compare(itm->name, name, false) == 0)
			return true;
	return false;
}

size_t JStruct::size() {
	return fields.size();
}

void JStruct::add(string name, string value) {
	fields.push_back(new JItem(name, value, this));
}

void JStruct::add(string name, JValue *value) {
	fields.push_back(new JItem(name, value, this));
}

void JStruct::addIgnored(string name, string value) {
	fields.push_back(new JItem(name, (JValue *) new JIgnored(value), this));
}

string JStruct::getValue() const {
	return this->toString();
}

JStruct::~JStruct() {
	for (vector<JItem *>::iterator i = fields.begin(); i != fields.end(); i++)
		delete (*i);
}
//-------------------------------jIgnored

string JIgnored::toString() const {
	return value;
}

void JIgnored::fromString(string str) {
	value = str;
}

size_t JIgnored::size() {
	return this->value.size();
}

string JIgnored::getValue() const {
	return this->value;
}


