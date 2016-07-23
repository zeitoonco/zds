/*
 * logger.hpp
 *
 *  Created on: Nov 16, 2015
 *      Author: ajl
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <database/sqliteDatabaseHub.h>
#include "sstream"
#include <algorithm>
#include <fstream>
#include <queue>
#include <thread>

namespace zeitoon {
namespace utility {

/**
 * sotoohe mokhtalefe log
 */
class LogLevel {
public:
	enum levels {
		note = 0,         ///<baraye mavaredi ke ahamiyati nadarand, va dar amalkard system tasiri nadaran
		debug = 1,        ///<mavaredi ke debugger va programmer be an niyaz darad
		trace = 2,        ///<mavaredi ke ehtemal darad moshkel ijad konad
		warning = 3,      ///<mavarede khatarnaki ke be khata nemirasad vali momken ast badan dar system ijade khata konad
		error = 4,        ///<khatahayi ke dar system rokh midahad, vali system an ra handle mikonad
		fatal = 5,         ///<khatahayi ke dar system rokh midahad, vali system nemitavanad an ra handl konad
		__Max = 6
	};
	const static string typeString[];
};

/**
 * class makhsoose log gereftan dar system. in class ye database mahali ijad mikonad va tamami peygham haye daryafti ra dar an zakhira mikonad.
 */
class Logger {
	struct LogStruct {
		LogLevel::levels level;
		string logTime;
		string owner;
		string message;

		LogStruct(LogLevel::levels ilevel, string ilogTime, string iowner, string imessage) {
			level = ilevel;
			logTime = ilogTime;
			owner = iowner;
			message = imessage;
		}
	};

	sqliteDatabaseHub _dbh;            // Logs database instance
	std::queue<zeitoon::utility::Logger::LogStruct> dbWriteQueue;
	std::ofstream _logFile;            // Logs file handler
	bool logTofile = false;
	bool logDB = false;
	bool terminalOut = false;
	bool forceCerr = false;
	bool dbThread = false;
	int terminalLoglvl = 0;
	std::thread *dbLogThread = NULL;


	void dbLogLoop();

	void startDBLoop();

	void stopDBLoop();

	void dbLogger(string &owner, string &message, LogLevel::levels &level, std::string &_Time);

	void fileLogger(string owner, string &message, LogLevel::levels &level, std::string _Time);

public:
	~Logger();

	void log(string owner, string message, LogLevel::levels level);

	void enableDB(std::string dbName);

	void disableDB();

	void clearDB();

	void enableFile(std::string filename);

	void disableFile();

	void clearFile();//remove

	void enableTerminalOut();

	void enableTerminalOut(int input);

	void setTerminalLoglvl(int input);

	void forceCerrOut();

	void forceCout();

	void disableTerminalOut();

	std::string getNameAndType() {
		return "Logger";
	}
	void flush();


	std::string currentTime();

	bool isTerminalLog() {
		return terminalOut;
	}

};


}
}
extern zeitoon::utility::Logger logger;

#define l(msg, lvl) logger.log(this->getNameAndType(),msg,lvl)
#define lNote(msg) logger.log(this->getNameAndType(),msg, zeitoon::utility::LogLevel::note)
#define lDebug(msg) logger.log(this->getNameAndType(),msg, zeitoon::utility::LogLevel::debug)
#define lTrace(msg) logger.log(this->getNameAndType(),msg, zeitoon::utility::LogLevel::trace)
#define lWarnig(msg) logger.log(this->getNameAndType(),msg, zeitoon::utility::LogLevel::warning)
#define lError(msg) logger.log(this->getNameAndType(),msg, zeitoon::utility::LogLevel::error)
#define lFatal(msg) logger.log(this->getNameAndType(),msg, zeitoon::utility::LogLevel::fatal)

#endif /* LOGGER_HPP_ */
