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

	int rowCount();

	int columnCount();

	std::string columnName(int fieldNumber);

	ColumnDataType::columndataTYPE columnDataType(int columnNumber);

	int columnDataSize(int columnNumber);

	std::string fieldValue(int tupleNumber, int columnNumber);

	bool fieldIsNull(int tupleNumber, int columnNumber);

	int fieldSize(int tupleNumber, int columnNumber);

	std::string toString(SerializationType type = JSON); //no need to implement
	void fromString(std::string data, SerializationType type = SerializationType::JSON);

	DTBase &operator=(std::string);

	DTBase &operator=(DTBase &dtvar);

	bool operator==(DTBase &dtvar);

	bool operator!=(DTBase &dtvar);

	void columnRemove(string name);

	void columnRemoveAt(int indx);

	void columnAdd(string name, ColumnDataType::columndataTYPE dtype, int size);

	void rowAdd(std::vector<std::string>);

	void rowRemove(int indx);

	void clearRows();

	void clear();
};
}
}

#endif /* DTTABLESTRING_HPP_ */
