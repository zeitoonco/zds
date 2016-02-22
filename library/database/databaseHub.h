#include "utility/exceptionex.hpp"
#include "sqlite3.h"
#include <vector>

namespace zeitoon {
namespace utility {

class databaseHub {
protected:
	sqlite3 *dbp;
	bool isConnected, onTransaction;

public:
	databaseHub() :
			dbp(NULL), isConnected(false), onTransaction(false) {
	}

	void connect(string fileName) {
		if (sqlite3_open(fileName.c_str(), &dbp)) {
			stringstream buff;
			buff << "Can't open database: " << sqlite3_errmsg(dbp);
			sqlite3_close(dbp);
			EXTconnectionError(buff.str());
		}
		isConnected = true;
	}

	void disconnect() {
		//todo: review transactions
		//if (onTransaction)
		//	rollbackTransaction();
		sqlite3_close(dbp);
		isConnected = false;
	}

	void query(string selectstmt, vector<vector<string> > &results, vector<string> *columns = NULL) {
		sqlite3_stmt* stmt;
		int res = sqlite3_prepare_v2(dbp, selectstmt.c_str(), -1, &stmt, NULL);
		if (res == SQLITE_OK) {
			int cols = sqlite3_column_count(stmt);
			if (columns != NULL)
				for (int i = 0; i < cols; i++)
					columns->push_back(string(sqlite3_column_name(stmt, i)));
			while (true) {
				res = sqlite3_step(stmt);
				if (res != SQLITE_ROW) {
					if (res == SQLITE_DONE)
						break;
					else {
						sqlite3_finalize(stmt);
						EXTDBError("Error in loading data.");
					}
				}
				vector<string> vrow;
				for (int i = 0; i < cols; i++)
					vrow.push_back((char*) sqlite3_column_text(stmt, i));
				results.push_back(vrow);
			}
			sqlite3_finalize(stmt);
		} else {
			EXTDBError("Error in loading statement.");
		}
	}

	void execute(string command) {
		char *zErrMsg = 0;
		int rc;
		rc = sqlite3_exec(dbp, command.c_str(), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			stringstream buff;
			buff << "SQL error: " << zErrMsg;
			sqlite3_free(zErrMsg);
			EXTDBError(buff.str());
		}
	}

	/*
	 ** This function is used to load the contents of a database file on disk
	 ** into the "main" database of open database connection pInMemory, or
	 ** to save the current contents of the database opened by pInMemory into
	 ** a database file on disk. pInMemory is probably an in-memory database,
	 ** but this function will also work fine if it is not.
	 **
	 ** Parameter zFilename points to a nul-terminated string containing the
	 ** name of the database file on disk to load from or save to. If parameter
	 ** isSave is non-zero, then the contents of the file zFilename are
	 ** overwritten with the contents of the database opened by pInMemory. If
	 ** parameter isSave is zero, then the contents of the database opened by
	 ** pInMemory are replaced by data loaded from the file zFilename.
	 **
	 ** If the operation is successful, SQLITE_OK is returned. Otherwise, if
	 ** an error occurs, an SQLite error code is returned.
	 */
	int loadOrSaveDb(const char *zFilename, int isSave) {
		int rc; /* Function return code */
		sqlite3 *pFile; /* Database connection opened on zFilename */
		sqlite3_backup *pBackup; /* Backup object used to copy data */
		sqlite3 *pTo; /* Database to copy to (pFile or pInMemory) */
		sqlite3 *pFrom; /* Database to copy from (pFile or pInMemory) */

		/* Open the database file identified by zFilename. Exit early if this fails
		 ** for any reason. */
		rc = sqlite3_open(zFilename, &pFile);
		if (rc == SQLITE_OK) {

			/* If this is a 'load' operation (isSave==0), then data is copied
			 ** from the database file just opened to database pInMemory.
			 ** Otherwise, if this is a 'save' operation (isSave==1), then data
			 ** is copied from pInMemory to pFile.  Set the variables pFrom and
			 ** pTo accordingly. */
			pFrom = (isSave ? this->dbp : pFile);
			pTo = (isSave ? pFile : this->dbp);

			/* Set up the backup procedure to copy from the "main" database of
			 ** connection pFile to the main database of connection pInMemory.
			 ** If something goes wrong, pBackup will be set to NULL and an error
			 ** code and  message left in connection pTo.
			 **
			 ** If the backup object is successfully created, call backup_step()
			 ** to copy data from pFile to pInMemory. Then call backup_finish()
			 ** to release resources associated with the pBackup object.  If an
			 ** error occurred, then  an error code and message will be left in
			 ** connection pTo. If no error occurred, then the error code belonging
			 ** to pTo is set to SQLITE_OK.
			 */
			pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
			if (pBackup) {
				(void) sqlite3_backup_step(pBackup, -1);
				(void) sqlite3_backup_finish(pBackup);
			}
			rc = sqlite3_errcode(pTo);
		}

		/* Close the database connection opened on database file zFilename
		 ** and return the result of this function. */
		(void) sqlite3_close(pFile);
		return rc;
	}

	void cleanup() {
		this->execute("VACUUM");
	}

	string getNameAndType() {
		return "databaseHub";
	}
};

}
}
