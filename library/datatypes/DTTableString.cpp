/*
 * DTTableString.cpp
 *
 *  Created on: Feb 2, 2016
 *      Author: inf
 */

#include "datatypes/DTTableString.hpp"
#include "utility/exceptionex.hpp"
#include "utility/jsonParser.hpp"
#include "datatypes/dtmultifieldtypes.hpp"
#include "datatypes/dtsingletypes.hpp"

using namespace zeitoon::datatypes;
using namespace zeitoon::utility;

DTTableString::DTTableString(std::string receivedData, std::string name) :
		DTTable(name) {
	jsonData=std::make_shared<JStruct>();
	jsonData->fromString(receivedData);
}

DTTableString::DTTableString(std::string name) :
		DTTableString("{\"columns\":[],\"rows\":[]}", name) {
	//jsonData->fromString("{\"columns\":[],\"rows\":[]}");
}

/*DTTableString::DTTableString(const DTTableString &tbl) : DTTable(tbl.getName()) {
	this->fromString(tbl.toString());
}*/

DTTableString::~DTTableString() {

}

size_t DTTableString::rowCount() const {
	return (*jsonData)["rows"].size();

}

size_t DTTableString::columnCount() const {
	return ((*jsonData)["columns"].size());
}

std::string DTTableString::columnName(int fieldNumber) const { //field numbers start from 0
	return (*jsonData)["columns"][fieldNumber]["name"].getValue();
}

ColumnDataType::columnDataType DTTableString::columnDataType(int columnNumber) const {
	return ColumnDataType::fromString(
			(*jsonData)["columns"][columnNumber]["datatype"].getValue());
}

size_t DTTableString::columnDataSize(int columnNumber) const {
//returns -1 for variable size fields.
	return (size_t) std::atol(
			((*jsonData)["columns"][columnNumber]["size"].getValue()).c_str());
}

std::string DTTableString::fieldValue(int rowNumber, int colNumber) const {
	return (*jsonData)["rows"][rowNumber][colNumber].getValue();
}

bool DTTableString::fieldIsNull(int tupleNumber, int columnNumber) const {
	return this->fieldValue(tupleNumber, columnNumber) == "NULL";
}

size_t DTTableString::fieldSize(int tupleNumber, int columnNumber) const {
	if (this->columnDataType(columnNumber) == ColumnDataType::TEXT) {
		return (*jsonData)["rows"][tupleNumber][columnNumber].size();
	} else {
		return this->columnDataSize(columnNumber);
	}
}

std::string DTTableString::toString(int indent,
                                    std::string indentContent) const {//TODO: args changed--CHECK TOSTRING for bugs
	return jsonData->toString();//todo: um, what r those args for?!
}

void DTTableString::fromString(std::string data) {
	jsonData->fromString(data);
}

DTBase &DTTableString::operator=(std::string input) {
	jsonData->fromString(input);
	return *this;
}

DTBase &DTTableString::operator=(DTBase &dtvar) {
	auto tempDTS = dynamic_cast<DTString *>(&dtvar);
	if (tempDTS) {
		jsonData->fromString(tempDTS->getValue());
		return *this;;
	} else {
		auto tempDTT = dynamic_cast<DTTableString *>(&dtvar);
		if (tempDTT) {
			jsonData = tempDTT->jsonData;
			return *this;
		} else {
			auto tempDTTable = dynamic_cast<DTTable *>(&dtvar);
			if (tempDTTable) {
				this->fromString(tempDTTable->toString());
				return *this;
			}
		}
	}
	EXTdataTypeMismatch("Invalid Data input");
}

bool DTTableString::operator==(DTBase &dtvar) {
	auto tempDTS = dynamic_cast<DTString *>(&dtvar);
	if (tempDTS) {
		return (JStruct(tempDTS->getValue()) == jsonData->toString());
	} else {
		auto tempDTTable = dynamic_cast<DTTable *>(&dtvar);
		if (tempDTTable) {
			return (jsonData->toString() == tempDTTable->toString());
		}
	}
	return false;
}

bool DTTableString::operator!=(DTBase &dtvar) {
	return !this->operator==(dtvar);
}

std::string DTTableString::getNameAndType() {
	return "DTTableString";

}

void DTTableString::columnRemove(string name) {
	//Call Clear if there's only one row , and the command is to remove that particular row ;
	JArray &cols = (JArray &) (*jsonData)["columns"];
	int index = -1;
	for (unsigned int colIter = 0; colIter < cols.size(); colIter++) {
		if (cols[colIter]["name"] == name) {
			index = colIter;
			break;
		}
	}
	if (index == -1)
		EXTinvalidName("Invalid given column name.");
	columnRemoveAt((size_t) index);
}

void DTTableString::columnRemoveAt(size_t index) {
	//Calls Clear if there's only one column , and the command is to remove that particular col ;
	JArray &cols = (JArray &) (*jsonData)["columns"];
	JArray &tupples = (JArray &) (*jsonData)["rows"];
	try {
		cols.removeAt(index);
		if (cols.size() < 1) {
			this->clear();
		}
	} catch (exceptionEx *errorInfo) {
		EXToutOfRangeI("invalid column index", errorInfo);
	}

	for (unsigned int rowIter = 0; rowIter < tupples.size(); rowIter++) {
		JArray &tuppleTemp = (JArray &) tupples[rowIter];
		tuppleTemp.removeAt(index);
	}

}

void DTTableString::columnAdd(string name, ColumnDataType::columnDataType dtype, int size) {
	JArray &cols = (JArray &) (*jsonData)["columns"];
	JArray &tupples = (JArray &) (*jsonData)["rows"];
	JStruct colStruct;
	colStruct.add("name", name);
	colStruct.add("datatype", ColumnDataType::toString(dtype));
	colStruct.add("size", std::to_string(size));
	cols.add(colStruct.toString());
	for (unsigned int iter = 0; iter < tupples.size(); iter++) {
		JArray &tuppleTemp = (JArray &) tupples[iter];
		tuppleTemp.add("NULL");
	}
}

void DTTableString::rowAdd(std::vector<std::string> newTupple) {
	JArray &tupples = (JArray &) (*jsonData)["rows"];
	JArray &cols = (JArray &) (*jsonData)["columns"];
	if (newTupple.size() != cols.fields.size())
		EXTexceptionEx("Number of new row parameters does not match the number of columns");
	tupples.add(JArray(newTupple).getValue());
}

void DTTableString::rowRemove(size_t indx) {
	JArray &tupples = (JArray &) (*jsonData)["rows"];
	tupples.removeAt(indx);
}

void DTTableString::clearRows() {
	JArray &tupples = (JArray &) (*jsonData)["rows"];
	for (size_t iter = (tupples.size() - 1); iter > -1; iter--) {
		tupples.removeAt(iter);
	}
}

void DTTableString::clear() {
	JArray &cols = (JArray &) (*jsonData)["columns"];
	for (size_t iter = (cols.size() - 1); iter > -1; iter--) {
		this->columnRemoveAt(iter);
	}
	this->clearRows();
}
