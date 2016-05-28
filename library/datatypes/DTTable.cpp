/*
 * DTTable.cpp
 *
 *  Created on: Feb 2, 2016
 *      Author: inf
 */

#include "datatypes/DTTable.hpp"
#include "utility/exceptionex.hpp"

using namespace zeitoon::utility;

namespace zeitoon {
namespace datatypes {

std::string ColumnDataType::toString(columnDataType type) {
	return ColumnDataType::enumArray[type];

}

ColumnDataType::columnDataType ColumnDataType::fromString(std::string str) {
	str = Strings::trim(str, "\"");
	for (int i = 0; i < ColumnDataType::columnDataType::__MAX; i++)
		if (!Strings::compare(str, ColumnDataType::enumArray[i], false))
			return (ColumnDataType::columnDataType) i;
	EXTcantParseStringO("Unknown constant passed to Enum :" + str, getNameAndType());
}

std::string ColumnDataType::enumArray[__MAX] = {"text", "integer", "float",
                                                "binary", "boolean","date", "unknown"};

}
}
