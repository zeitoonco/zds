#include <iostream>
//#include "datatypes/DTTablePostgres.hpp"
#include "datatypes/DTTableString.hpp"
#include <vector>
using namespace zeitoon;
using namespace zeitoon::datatypes;
int main() {
	try {
		DTTableString testdb("test");
		std::cerr<<"empty      : "<< testdb.toString()<<std::endl;
		testdb.columnAdd("id", ColumnDataType::INTEGER, 4);
		testdb.columnAdd("name", ColumnDataType::TEXT, 16);
		testdb.columnAdd("tel", ColumnDataType::TEXT, 16);
		std::cerr<<"Cols       : "<< testdb.toString()<<std::endl;
		std::vector<std::string> row { "11", "Apple", "13231-321" };
		testdb.rowAdd(row);
		testdb.rowAdd(row);
		testdb.rowAdd(row);
		std::cerr<<"full       : "<< testdb.toString()<<std::endl;
		testdb.rowRemove(2);
		std::cerr<<"rem 2      : "<< testdb.toString()<<std::endl;
		testdb.clearRows();
		std::cerr<<"clear rows : "<< testdb.toString()<<std::endl;
		testdb.clear();
		std::cerr<<"clear      : "<< testdb.toString()<<std::endl;
	} catch (exceptionEx &exc) {
		std::cerr << std::endl << exc->what();
	}
}

