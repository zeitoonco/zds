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
	string x1 = "{\"b\":[1,2,3],\"c\":\"hello\"}";
	JStruct j(x1);
	cout<<j.size();
	for (size_t i=0;i<j.size();i++)
		cout<<j[i].size();
}

