//
// Created by inf on 6/28/16.
//

#include "logger.hpp"
#include "utility.hpp"
#include <chrono>

zeitoon::utility::Logger logger;

namespace zeitoon {
namespace utility {

const std::string LogLevel::typeString[] = {"note", "debug", "trace", "warning", "error", "fatal"};

void Logger::log(string owner, string message, LogLevel::levels level) {
	std::string tempTime = this->currentTime();
	if (this->logDB)
		this->dbLogger(owner, message, level, tempTime);
	if (this->logTofile)
		this->fileLogger(owner, message, level, tempTime);
	if (terminalOut) {
		if (level < this->terminalLoglvl)
			return;
		if (owner.size() >= 16)
			owner += "   ";
		owner = zeitoon::utility::Strings::padRight(owner, 16);
		std::string tempLvl = Strings::padRight(LogLevel::typeString[level], 8);
		tempTime = zeitoon::utility::Strings::padRight(tempTime.substr(11, 50), 17);
		if (message.length()>121)
			message=message.substr(0,120);
		if (forceCerr || level >= LogLevel::warning)
			std::cerr <<"!>" + tempLvl + tempTime + owner + message + "\n";
		else
			std::cout <<"!>" + tempLvl + tempTime + owner + message + "\n";
	}
}

Logger::~Logger() {
	if (logTofile)
		_logFile.close();
	this->disableDB();
}

void Logger::enableDB(std::string dbName) {
	_dbh.connect(dbName);
	datatypes::DTTableString res = _dbh.query(
			"SELECT name FROM sqlite_master WHERE type='table' AND name='log';");
	if (res.rowCount() == 0) {
		_dbh.execute(
				"CREATE TABLE log ( ID INTEGER  PRIMARY KEY AUTOINCREMENT,  loglevel  INTEGER  NOT NULL DEFAULT ( 0 ), logTime TEXT NOT NULL DEFAULT('---') , owner  TEXT NOT NULL DEFAULT(''),  message   TEXT  NOT NULL DEFAULT(''));");
		_dbh.execute("CREATE TABLE loglevel ( ID   INTEGER PRIMARY KEY, name TEXT    NOT NULL );");
		_dbh.execute(
				"INSERT INTO [loglevel] ([ID], [name]) VALUES (0, 'note');INSERT INTO [loglevel] ([ID], [name]) VALUES (1, 'debug');INSERT INTO [loglevel] ([ID], [name]) VALUES (2, 'trace');INSERT INTO [loglevel] ([ID], [name]) VALUES (3, 'warning');INSERT INTO [loglevel] ([ID], [name]) VALUES (4, 'error');INSERT INTO [loglevel] ([ID], [name]) VALUES (5, 'fatal');");

	}
	this->startDBLoop();

}

void Logger::disableDB() {
	if (dbThread) {
		stopDBLoop();
		_dbh.disconnect();
	}
}

void Logger::clearDB() {
	_dbh.execute("DELETE FROM log; VACUUM;");
}

void Logger::enableFile(std::string filename) {
	_logFile.open(filename, std::ios::out);
	_logFile << "\n\n\tLog Enabled:   " << this->currentTime() << " ---------------\n";
	logTofile = true;
}

void Logger::disableFile() {
	_logFile.close();
	logTofile = false;
	_logFile << "\n\n\tLog Disabled:   " << this->currentTime() << " ---------------\n";
}

void Logger::clearFile() {
	_logFile.close();
	_logFile.trunc;
	_logFile << "\n\n\tLog cleared on:   " << this->currentTime() << " ---------------\n";

}

void Logger::enableTerminalOut(int input) {
	setTerminalLoglvl(input);
	enableTerminalOut();
}

void Logger::enableTerminalOut() {
	terminalOut = true;
}

void Logger::forceCerrOut() {
	this->forceCerr = true;
}

void Logger::forceCout() {
	this->forceCerr = false;
}

void Logger::setTerminalLoglvl(int input) {
	if (input < 0 or input >= zeitoon::utility::LogLevel::__Max)
		EXToutOfRange("Selected log level does not exist.");
	terminalLoglvl = input;
}

void Logger::disableTerminalOut() {
	terminalOut = false;
}

std::string Logger::currentTime() {
	const auto currentTime = std::chrono::system_clock::now();
	time_t time = std::chrono::system_clock::to_time_t(currentTime);
	auto currentTimeRounded = std::chrono::system_clock::from_time_t(time);
	if (currentTimeRounded > currentTime) {
		--time;
		currentTimeRounded -= std::chrono::seconds(1);
	}
	int milliseconds = std::chrono::duration_cast<std::chrono::duration<int, micro> >(
			currentTime - currentTimeRounded).count();
	struct tm *values;
	values = localtime(&time);
	char buffer[80];
	memset(buffer, 0, 80);
	std::strftime(buffer, 80, "%F %X", values);
	return std::string(buffer) + "." + Strings::padLeft(std::to_string(milliseconds), 6, '0');
}

void Logger::startDBLoop() {
	if (dbThread)
		return;
	this->dbLogThread = new thread(&Logger::dbLogLoop, this);
	dbThread = true;
	logDB = true;
	// dbLogThread->detach();

}

void Logger::stopDBLoop() {
	logDB = false;
	dbThread = false;
	if (dbLogThread->joinable())
		dbLogThread->join();


	delete dbLogThread;
}

void Logger::dbLogLoop() {
	while (true) {
		if (dbWriteQueue.size() > 0) {
			LogStruct &temp = dbWriteQueue.front();
			string ss;
			ss = "insert into log(loglevel,logTime,owner,message) values(" + std::to_string((int) temp.level) + ",'" +
			     temp.logTime + "','" + temp.owner + "','" + temp.message + "');";
			_dbh.execute(ss);
			dbWriteQueue.pop();
		}
		else
			std::this_thread::sleep_for(chrono::milliseconds(10));
		if (dbWriteQueue.size() == 0 && !dbThread)
			break;
	}
}

void Logger::dbLogger(string &owner, string &message, LogLevel::levels &level, std::string &_Time) {
	message = utility::Strings::replace(message, "'", "''");
	this->dbWriteQueue.push(zeitoon::utility::Logger::LogStruct(level, _Time, owner, message));
	if (!dbThread)
		this->startDBLoop();


}

void Logger::fileLogger(string owner, string &message, LogLevel::levels &level, std::string time) {

	owner = zeitoon::utility::Strings::padRight(owner, 32);
	string tempLvl = Strings::padRight(LogLevel::typeString[level], 8);
	time = Strings::padRight(time, 29);
	_logFile << tempLvl << time << owner << message << "\n";

}

}
}

