/*
 * FileSystem.hpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */

#ifndef FILESYSTEM_HPP_
#define FILESYSTEM_HPP_

#include <string>
#include <vector>
#include <boost/filesystem.hpp>

typedef std::vector<std::string> stringList;
using std::string;

namespace zeitoon {
namespace GUI {


class FileSystem {
public:
	static char pathSeprator;

	FileSystem(std::string baseDir);

	bool fileExist(std::string address);

	void fileRemove(std::string address);

	void fileCreate(std::string address, std::string content);

	void fileUpdate(std::string address, std::string content, bool create = true); //create= true
	std::string fileRead(std::string address);

	bool directoryExist(std::string address);

	void directoryCreate(std::string address);

	void directoryRemove(std::string address);

	stringList listFiles(std::string address, bool includeExtension = true);

	stringList listDirectory(std::string address);

	stringList list(std::string address, bool listDir, bool listFiles, bool includeExtension = true);

private:
	std::string baseDir = "";
};
}
}

#endif /* FILESYSTEM_HPP_ */
