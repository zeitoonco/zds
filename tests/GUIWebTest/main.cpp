//
// Created by ajl on 5/9/16.
//
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility/utility.hpp>

using namespace std;

int main() {
	system(("x-www-browser " + zeitoon::utility::FileSystemUtility::getAppPath() + "/web/index.html").c_str());
}