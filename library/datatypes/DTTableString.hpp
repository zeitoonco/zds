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

namespace zeitoon {
namespace datatypes {

class DTTableString : public DTTable {
	std::string getNameAndType();

public:
	/** JStruct jsonData
	 *  baraye tabdil va negahdaari e result e haasel az yek query ke PGresult * result
	 */
	JStruct jsonData;

	~DTTableString();

	DTTableString(std::string name);

	DTTableString(std::string receivedData, std::string name);

	size_t rowCount();

	size_t columnCount();

	std::string columnName(int fieldNumber);

	ColumnDataType::columnDataType columnDataType(int columnNumber);

	size_t columnDataSize(int columnNumber);

	std::string fieldValue(int tupleNumber, int columnNumber);

	bool fieldIsNull(int tupleNumber, int columnNumber);

	size_t fieldSize(int tupleNumber, int columnNumber);

	std::string toString(); //no need to implement//todo:#NAVIDI wtf?
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
