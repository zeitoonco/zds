/*
 * mian.cpp
 *
 *  Created on: Jan 10, 2016
 *      Author: ajl
 */
#include <iostream>
#include "utility/jsonParser.hpp"

using namespace zeitoon::utility;

int main() {
	JStruct j;
	j.add("name", "ajl");
	j.add("anarray", "[1,2,3,4]");
	j.add("jsinjs", new JVariable("{\"aa\":\"aa\",\"bb\":22}"));

	cout << "name is ajl:" << (j["name"] == "ajl")<<endl;
	cout << "name is Ajl:" << (j["name"] == "Ajl")<<endl;
	cout << "array:" << (j["anarray"] == "[1,2,3,4]")<<endl;
	cout << "array 2:" << (j["anarray"][2] == "3")<<endl;

}

