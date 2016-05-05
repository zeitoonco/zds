/*
 * DTTableString.hpp
 *
 *  Created on: Feb 2, 2016
 *      Author: inf
 */

#ifndef DTTABLESTRING_HPP_
#define DTTABLESTRING_HPP_

#include "datatypes/DTTable.hpp"
#include "utility/jsonParser.hpp"
#include <memory>

namespace zeitoon {
namespace datatypes {

class DTTableString : public DTTable {
	std::string getNameAndType();

public:
	/** JStruct jsonData
	 *  baraye tabdil va negahdaari e result e haasel az yek query ke PGresult * result
	 */
	std::shared_ptr<JStruct> jsonData;

	~DTTableString();

	DTTableString(std::string name);

	//DTTableString(const DTTableString &tbl);

	DTTableString(std::string receivedData, std::string name);

	size_t rowCount() const;

	size_t columnCount() const;

	std::string columnName(int fieldNumber) const;

	ColumnDataType::columnDataType columnDataType(int columnNumber) const;

	size_t columnDataSize(int columnNumber) const;

	std::string fieldValue(int rowNumber, int colNumber) const;

	bool fieldIsNull(int tupleNumber, int columnNumber) const;

	size_t fieldSize(int tupleNumber, int columnNumber) const;

	std::string toString(int indent = -1, std::string indentContent = defaultIndentContent) const;

	void fromString(std::string data);

	DTBase &operator=(std::string);

	DTBase &operator=(DTBase &dtvar);

	bool operator==(DTBase &dtvar);

	bool operator!=(DTBase &dtvar);

	void columnRemove(string name);

	void columnRemoveAt(size_t index);

	void columnAdd(string name, ColumnDataType::columnDataType dtype, int size);

	void rowAdd(std::vector<std::string>);

	void rowRemove(size_t indx);

	void clearRows();

	void clear();
};
}
}

#endif /* DTTABLESTRING_HPP_ */
