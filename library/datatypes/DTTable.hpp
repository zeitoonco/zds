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
	static std::string getNameAndType(){
		return "ColumnDataType";
	}
public:


	enum columndataTYPE {
		TEXT, INTEGER, FLOAT, BINARY, BOOLEAN, __MAX
	};
	static std::string enumArray[__MAX];
	static std::string toString(columndataTYPE);
	static 	columndataTYPE fromString(std::string);
};

class DTTable: public DTBase {
public:


	DTTable(std::string name) :
			DTBase(name) {

	}

	DTTable(std::string text, SerializationType type) :
			DTBase(text, SerializationType::JSON) {

	}

	virtual ~DTTable() {

	}

	virtual std::string getNameAndType() {
		return "DTTable";
	}

	virtual int rowCount()=0;

	virtual int columnCount()=0;

	virtual std::string columnName(int colNumber)=0;

	virtual ColumnDataType::columndataTYPE columnDataType(int colNumber)=0;

	virtual int columnDataSize(int colNumber)=0;

	virtual std::string fieldValue(int rowNumber, int colNumber)=0;

	virtual bool fieldIsNull(int rowNumber, int colNumber)=0;

	virtual std::string toString(SerializationType type = JSON)=0;

	virtual void fromString(std::string data, SerializationType type = SerializationType::JSON)=0;

	virtual DTBase& operator =(std::string)=0;

	virtual DTBase& operator =(DTBase& dtvar)=0;

	virtual bool operator ==(DTBase& dtvar)=0;

	virtual bool operator !=(DTBase& dtvar)=0;

};

}
}

#endif /* DTTABLE_HPP_ */
