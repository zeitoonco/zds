/*
 * DTTable.hpp
 *
 *  Created on: Jan 27, 2016
 *      Author: inf
 */

#ifndef DTTABLE_HPP_
#define DTTABLE_HPP_

#include <datatypes/dtbase.hpp>
#include "utility/exceptionex.hpp"

namespace zeitoon {
namespace datatypes {

class ColumnDataType {
	static std::string getNameAndType() {
		return "ColumnDataType";
	}

public:


	enum columnDataType {
		TEXT, INTEGER, FLOAT, BINARY, BOOLEAN, UNKNOWN, __MAX
	};
	static std::string enumArray[__MAX];

	static std::string toString(columnDataType);

	static columnDataType fromString(std::string);
};

class DTTable : public DTBase {
public:


	DTTable(std::string name) : DTBase(name) {

	}

	DTTable(std::string name, std::string text) : DTBase(name, text) {

	}

	virtual ~DTTable() {

	}

	virtual std::string getNameAndType() {
		return "DTTable";
	}

	virtual size_t rowCount() const= 0;

	virtual size_t columnCount() const= 0;

	virtual std::string columnName(int colNumber) const= 0;

	virtual ColumnDataType::columnDataType columnDataType(int colNumber)const = 0;

	virtual size_t columnDataSize(int colNumber) const= 0;

	virtual std::string fieldValue(int rowNumber, int colNumber) const= 0;

	virtual bool fieldIsNull(int rowNumber, int colNumber) const= 0;

	virtual std::string toString() const= 0;

	virtual void fromString(std::string data) = 0;

	virtual DTBase &operator=(std::string) = 0;

	virtual DTBase &operator=(DTBase &dtvar) = 0;

	virtual bool operator==(DTBase &dtvar) = 0;

	virtual bool operator!=(DTBase &dtvar) = 0;

};

}
}

#endif /* DTTABLE_HPP_ */
