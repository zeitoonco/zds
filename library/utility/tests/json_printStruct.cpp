/*
 * mian.cpp
 *
 *  Created on: Jan 10, 2016
 *      Author: ajl
 */
#include <iostream>
#include "utility/jsonParser.hpp"

using namespace zeitoon::utility;

void printJS(JValue *js, int tab=0) {
	if (js->getType() == JTstruct) {
		JStruct *q = (JStruct*) js;
		cout << string(tab, ' ') << "+JStruct [size:" << q->size() << "]\n";
		for (size_t i = 0; i < q->size(); i++) {
			cout << string(tab + 2, ' ') << "-Item[" << i << ":"
					<< q->fields[i]->name << "]:\n";
			printJS(&(q->operator [](i)), tab + 6);
		}
	} else if (js->getType() == JTarray) {
		JArray *q = (JArray*) js;
		cout << string(tab, ' ') << "+JArray [size:" << q->size() << "]\n";
		for (size_t i = 0; i < q->size(); i++) {
			cout << string(tab + 2, ' ') << "-Item[" << i << "]:\n";
			printJS(&(q->operator [](i)), tab + 6);
		}
	} else if (js->getType() == JTvariable) {
		JVariable *q = (JVariable*) js;
		cout << string(tab, ' ') << "$JVariable [size:" << q->size() << "]: "
				<< q->toString() << "\n";
	}
}

int main() {
	JStruct j;
	j.add("name", "ajl\na");
	j.add("anarray", "[1,2,3,4]");
	j.add("jsinjs", new JVariable("{\"aa\":\"aa\",\"bb\":22}"));

	printJS(&j);
	cout<<endl<<j;
}

