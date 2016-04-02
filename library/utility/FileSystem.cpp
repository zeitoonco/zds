/*
 * FileSystem.cpp
 *
 *  Created on: Jan 6, 2016
 *      Author: inf
 */

#include "FileSystem.hpp"
#include "utility/utility.hpp"

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <fstream>
#include <iostream>
#include <streambuf>

using namespace boost::filesystem;

namespace zeitoon {
namespace utility {


char FileSystem::pathSeprator = boost::filesystem::path::preferred_separator;

FileSystem::FileSystem(std::string ibaseDir) {
	if (ibaseDir != "") {
		ibaseDir = Strings::rtrim(ibaseDir, "/\\") + pathSeprator;
		if (boost::filesystem::is_directory(ibaseDir) == false) {
			boost::filesystem::create_directories(ibaseDir); //creates a directory
		}
	}
	this->baseDir = ibaseDir;
}

bool FileSystem::fileExist(std::string address) {
	address = Strings::trim(address, "/\\");
	boost::filesystem::path pp = baseDir + address;
	return boost::filesystem::exists(pp);
}

void FileSystem::fileRemove(std::string address) {
	address = Strings::trim(address, "/\\");
	boost::filesystem::path pp = baseDir + address;

	if (boost::filesystem::remove(pp) == false) {
		//exception needed
		return;
	}
}

void FileSystem::fileCreate(std::string address, std::string content) {
	address = Strings::trim(address, "/\\");
	std::string path = baseDir + address;
	std::ofstream outfile;
	outfile.open(path, std::ofstream::out);
	outfile << content << std::endl;
	outfile.close();
}

void FileSystem::fileUpdate(std::string address, std::string content, bool create) {
	address = Strings::trim(address, "/\\");
//if file didnt exist && create = false == exception
	std::string path = baseDir + address;
	if (this->fileExist(address) == false) {
		if (create == false) {
			//exception
			return;
		} else {
			this->fileCreate(address, content);
		}
	} else {
		std::ofstream outfile;
		outfile.open(path, std::ofstream::out | std::ofstream::trunc);
		outfile << content << std::endl;
		outfile.close();
	}
}

bool FileSystem::directoryExist(std::string address) {
	address = Strings::trim(address, "/\\");
	boost::filesystem::path path = baseDir + address;
	return boost::filesystem::is_directory(path);
}

void FileSystem::directoryCreate(std::string address) {
	address = Strings::trim(address, "/\\");
	boost::filesystem::path path = baseDir + address;
	boost::filesystem::create_directories(path);
}

void FileSystem::directoryRemove(std::string address) {
	address = Strings::trim(address, "/\\");
	boost::filesystem::path path = baseDir + address;
	boost::filesystem::remove_all(path);
}

std::string FileSystem::fileRead(std::string address) {
	address = Strings::trim(address, "/\\");
	std::string path = baseDir + address;
	std::ifstream inFile(path);
	std::string tempBuff((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
	return tempBuff;
}

stringList FileSystem::list(std::string address, bool listDir, bool listFiles, bool includeExtension) {
	address = Strings::trim(address, "/\\");
	stringList lists;
	path p(baseDir + address);
	std::string fileName;
	for (auto entry : boost::make_iterator_range(directory_iterator(p), {})) {
		fileName = entry.path().filename().string();
		if (listFiles && entry.status().type() == file_type::regular_file) {

			if (!includeExtension && entry.path().extension().has_extension()) {
				fileName.erase(fileName.find(entry.path().extension().string()));
			}
			lists.push_back(fileName);
			continue;
		}
		if (listDir && entry.status().type() == file_type::directory_file) {
			lists.push_back(fileName);
		}
	}
	return lists;
}

stringList FileSystem::listFiles(std::string address, bool includeExtension) {
	return list(address, false, true, includeExtension);
}

stringList FileSystem::listDirectory(std::string address) {
	return list(address, true, false, false);
}
}
}