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

namespace zeitoon {
namespace utility {

/**
 * sotoohe mokhtalefe log
 */
enum class LogLevels {
	note = 0,        ///<baraye mavaredi ke ahamiyati nadarand, va dar amalkard system tasiri nadaran
	debug = 1,        ///<mavaredi ke debugger va programmer be an niyaz darad
	trace = 2,        ///<mavaredi ke ehtemal darad moshkel ijad konad
	warning = 3,    ///<mavarede khatarnaki ke be khata nemirasad vali momken ast badan dar system ijade khata konad
	error = 4,        ///<khatahayi ke dar system rokh midahad, vali system an ra handle mikonad
	fatal = 5        ///<khatahayi ke dar system rokh midahad, vali system nemitavanad an ra handl konad
};

/**
 * class makhsoose log gereftan dar system. in class ye database mahali ijad mikonad va tamami peygham haye daryafti ra dar an zakhira mikonad.
 */
class logger {
private:
	sqliteDatabaseHub dbh; //<database log ha

public:
	/**
	 * sazandaye class. in tabe addrese file database ra migirad va an ra baz mikonad. agar file vojood nadasht, database va table haye an ra misazad.
	 * @param logfile addrese file database
	 */
	logger(string logfile) {
		dbh.connect(logfile);
		//if database is created, create tables
		vector<vector<string> > results;
		datatypes::DTTableString res = dbh.query("SELECT name FROM sqlite_master WHERE type='table' AND name='log';");
		if (res.rowCount() == 0) {
			dbh.execute(
					"CREATE TABLE log ( ID INTEGER  PRIMARY KEY AUTOINCREMENT, timestamp DATETIME NOT NULL DEFAULT (  datetime('now', 'localtime') ), loglevel  INTEGER  NOT NULL DEFAULT ( 0 ), owner     TEXT NOT NULL DEFAULT(''),  message   TEXT  NOT NULL DEFAULT(''));");
			dbh.execute("CREATE TABLE loglevel ( ID   INTEGER PRIMARY KEY, name TEXT    NOT NULL );");
			dbh.execute(
					"INSERT INTO [loglevel] ([ID], [name]) VALUES (0, 'note');INSERT INTO [loglevel] ([ID], [name]) VALUES (1, 'debug');INSERT INTO [loglevel] ([ID], [name]) VALUES (2, 'trace');INSERT INTO [loglevel] ([ID], [name]) VALUES (3, 'warning');INSERT INTO [loglevel] ([ID], [name]) VALUES (4, 'error');INSERT INTO [loglevel] ([ID], [name]) VALUES (5, 'fatal');");
		}
	}

	/**
	 * mokhareb class. etesal be database ra mibandad.
	 */
	~logger() {
		dbh.disconnect();
	}

	/**
	 * yek peygham ra be onvane log daryaft karda va an ra dar database zakhira mikonad.
	 * @param owner   objecti ke in log ra ferestade. mitavan az tabe ::getNameAndType() ke dar aksare class ha tarif shode estefade kard.
	 * @param message peyghami ke dar morede log ast
	 * @param level   sathe log ba tavajeh be ahamiyate an baraye system va mored estefada an. @see LogLevels
	 */
	void log(string owner, string message, LogLevels level) {
		stringstream ss;
		ss << "insert into log(owner,message,loglevel) values('" << owner << "','" << message << "'," << (int) level
		<< ");";
		dbh.execute(ss.str());
	}

	void log(string owner, string message) {
		log(owner, message, LogLevels::note);
	}

	void log(string message) {
		log("", message, LogLevels::note);
	}

	void log(string message, LogLevels level) {
		log("", message, level);
	}

	/**
	 * tamami ye log haye zakhira shoda ra az database pak mikonad.
	 */
	void clearLogs() {
		dbh.execute("DELETE FROM log; VACUUM;");
	}
};

}
}
#endif /* LOGGER_HPP_ */
