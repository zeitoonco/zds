/** ***   WARNING   ***
  * This file has been generated using CodeGenerator.
  *   any manual modification will be erased in next cmake build.
  * Generated on : 2016-05-29 18:50:04.228406
  *
  * By ajl.
  */

#ifndef ZEITOON_UTILITY_EXCEPTIONS_HPP
#define ZEITOON_UTILITY_EXCEPTIONS_HPP

#include <utility/exceptionex.hpp>

namespace zeitoon {
namespace utility {

class invalidParameter : public exceptionExWithInnerEx {
public:
	invalidParameter(std::string imessage) :
			invalidParameter(imessage, "", "", "", -1) {
	}

	invalidParameter(std::string imessage, exceptionEx &innerException) :
			invalidParameter(imessage, "", "", "", -1, innerException) {
	}

	invalidParameter(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	invalidParameter(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Input parameter is invalid";
	}

	virtual std::string getName() {
		return "invalidParameter";
	}

};

class namesMismatch : public exceptionExWithInnerEx {
public:
	namesMismatch(std::string imessage) :
			namesMismatch(imessage, "", "", "", -1) {
	}

	namesMismatch(std::string imessage, exceptionEx &innerException) :
			namesMismatch(imessage, "", "", "", -1, innerException) {
	}

	namesMismatch(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	namesMismatch(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Variable names doesn't match";
	}

	virtual std::string getName() {
		return "namesMismatch";
	}

};

class dataTypeMismatch : public exceptionExWithInnerEx {
public:
	dataTypeMismatch(std::string imessage) :
			dataTypeMismatch(imessage, "", "", "", -1) {
	}

	dataTypeMismatch(std::string imessage, exceptionEx &innerException) :
			dataTypeMismatch(imessage, "", "", "", -1, innerException) {
	}

	dataTypeMismatch(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	dataTypeMismatch(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Can not convert between DataTypes";
	}

	virtual std::string getName() {
		return "dataTypeMismatch";
	}

};

class cantParseString : public exceptionExWithInnerEx {
public:
	cantParseString(std::string imessage) :
			cantParseString(imessage, "", "", "", -1) {
	}

	cantParseString(std::string imessage, exceptionEx &innerException) :
			cantParseString(imessage, "", "", "", -1, innerException) {
	}

	cantParseString(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	cantParseString(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Unreadable string format";
	}

	virtual std::string getName() {
		return "cantParseString";
	}

};

class outOfRange : public exceptionExWithInnerEx {
public:
	outOfRange(std::string imessage) :
			outOfRange(imessage, "", "", "", -1) {
	}

	outOfRange(std::string imessage, exceptionEx &innerException) :
			outOfRange(imessage, "", "", "", -1, innerException) {
	}

	outOfRange(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	outOfRange(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Value is out of range";
	}

	virtual std::string getName() {
		return "outOfRange";
	}

};

class notImplemented : public exceptionExWithInnerEx {
public:
	notImplemented(std::string imessage) :
			notImplemented(imessage, "", "", "", -1) {
	}

	notImplemented(std::string imessage, exceptionEx &innerException) :
			notImplemented(imessage, "", "", "", -1, innerException) {
	}

	notImplemented(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	notImplemented(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Not Implemented";
	}

	virtual std::string getName() {
		return "notImplemented";
	}

};

class invalidName : public exceptionExWithInnerEx {
public:
	invalidName(std::string imessage) :
			invalidName(imessage, "", "", "", -1) {
	}

	invalidName(std::string imessage, exceptionEx &innerException) :
			invalidName(imessage, "", "", "", -1, innerException) {
	}

	invalidName(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	invalidName(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Invalid Name";
	}

	virtual std::string getName() {
		return "invalidName";
	}

};

class obsolete : public exceptionExWithInnerEx {
public:
	obsolete(std::string imessage) :
			obsolete(imessage, "", "", "", -1) {
	}

	obsolete(std::string imessage, exceptionEx &innerException) :
			obsolete(imessage, "", "", "", -1, innerException) {
	}

	obsolete(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	obsolete(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Obsolete";
	}

	virtual std::string getName() {
		return "obsolete";
	}

};

class connectionError : public exceptionExWithInnerEx {
public:
	connectionError(std::string imessage) :
			connectionError(imessage, "", "", "", -1) {
	}

	connectionError(std::string imessage, exceptionEx &innerException) :
			connectionError(imessage, "", "", "", -1, innerException) {
	}

	connectionError(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	connectionError(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "connection failed";
	}

	virtual std::string getName() {
		return "connectionError";
	}

};

class DBError : public exceptionExWithInnerEx {
public:
	DBError(std::string imessage) :
			DBError(imessage, "", "", "", -1) {
	}

	DBError(std::string imessage, exceptionEx &innerException) :
			DBError(imessage, "", "", "", -1, innerException) {
	}

	DBError(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	DBError(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Database Error";
	}

	virtual std::string getName() {
		return "DBError";
	}

};

class duplicateName : public exceptionExWithInnerEx {
public:
	duplicateName(std::string imessage) :
			duplicateName(imessage, "", "", "", -1) {
	}

	duplicateName(std::string imessage, exceptionEx &innerException) :
			duplicateName(imessage, "", "", "", -1, innerException) {
	}

	duplicateName(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	duplicateName(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Duplicate Name";
	}

	virtual std::string getName() {
		return "duplicateName";
	}

};

class nullValue : public exceptionExWithInnerEx {
public:
	nullValue(std::string imessage) :
			nullValue(imessage, "", "", "", -1) {
	}

	nullValue(std::string imessage, exceptionEx &innerException) :
			nullValue(imessage, "", "", "", -1, innerException) {
	}

	nullValue(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	nullValue(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Null value received";
	}

	virtual std::string getName() {
		return "nullValue";
	}

};

class networkFailure : public exceptionExWithInnerEx {
public:
	networkFailure(std::string imessage) :
			networkFailure(imessage, "", "", "", -1) {
	}

	networkFailure(std::string imessage, exceptionEx &innerException) :
			networkFailure(imessage, "", "", "", -1, innerException) {
	}

	networkFailure(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	networkFailure(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Network Failure";
	}

	virtual std::string getName() {
		return "networkFailure";
	}

};

class accessDenied : public exceptionExWithInnerEx {
public:
	accessDenied(std::string imessage) :
			accessDenied(imessage, "", "", "", -1) {
	}

	accessDenied(std::string imessage, exceptionEx &innerException) :
			accessDenied(imessage, "", "", "", -1, innerException) {
	}

	accessDenied(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	accessDenied(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Access Denied";
	}

	virtual std::string getName() {
		return "accessDenied";
	}

};

class resourceNotAvailable : public exceptionExWithInnerEx {
public:
	resourceNotAvailable(std::string imessage) :
			resourceNotAvailable(imessage, "", "", "", -1) {
	}

	resourceNotAvailable(std::string imessage, exceptionEx &innerException) :
			resourceNotAvailable(imessage, "", "", "", -1, innerException) {
	}

	resourceNotAvailable(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	resourceNotAvailable(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Resource is not available";
	}

	virtual std::string getName() {
		return "resourceNotAvailable";
	}

};

class networkMaxRetryReached : public exceptionExWithInnerEx {
public:
	networkMaxRetryReached(std::string imessage) :
			networkMaxRetryReached(imessage, "", "", "", -1) {
	}

	networkMaxRetryReached(std::string imessage, exceptionEx &innerException) :
			networkMaxRetryReached(imessage, "", "", "", -1, innerException) {
	}

	networkMaxRetryReached(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	networkMaxRetryReached(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Maximum number of tries reached";
	}

	virtual std::string getName() {
		return "networkMaxRetryReached";
	}

};

class networkNoRetryTimeSet : public exceptionExWithInnerEx {
public:
	networkNoRetryTimeSet(std::string imessage) :
			networkNoRetryTimeSet(imessage, "", "", "", -1) {
	}

	networkNoRetryTimeSet(std::string imessage, exceptionEx &innerException) :
			networkNoRetryTimeSet(imessage, "", "", "", -1, innerException) {
	}

	networkNoRetryTimeSet(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	networkNoRetryTimeSet(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Reconnect interval has not been set";
	}

	virtual std::string getName() {
		return "networkNoRetryTimeSet";
	}

};

class filesystemFailure : public exceptionExWithInnerEx {
public:
	filesystemFailure(std::string imessage) :
			filesystemFailure(imessage, "", "", "", -1) {
	}

	filesystemFailure(std::string imessage, exceptionEx &innerException) :
			filesystemFailure(imessage, "", "", "", -1, innerException) {
	}

	filesystemFailure(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	filesystemFailure(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Filesystem Error";
	}

	virtual std::string getName() {
		return "filesystemFailure";
	}

};


} // zeitoon
} // utility


//Macro here -----------------------------------------------------------------

//invalidParameter
#define EXTinvalidParameter(msg) throw zeitoon::utility::invalidParameter(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTinvalidParameterO(msg, owner) throw zeitoon::utility::invalidParameter(msg,owner,__FILE__,__func__,__LINE__)
#define EXTinvalidParameterI(msg, inner) throw zeitoon::utility::invalidParameter(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTinvalidParameterIO(msg, owner, inner) throw zeitoon::utility::invalidParameter(msg,owner,__FILE__,__func__,__LINE__,inner)

//namesMismatch
#define EXTnamesMismatch(msg) throw zeitoon::utility::namesMismatch(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTnamesMismatchO(msg, owner) throw zeitoon::utility::namesMismatch(msg,owner,__FILE__,__func__,__LINE__)
#define EXTnamesMismatchI(msg, inner) throw zeitoon::utility::namesMismatch(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTnamesMismatchIO(msg, owner, inner) throw zeitoon::utility::namesMismatch(msg,owner,__FILE__,__func__,__LINE__,inner)

//dataTypeMismatch
#define EXTdataTypeMismatch(msg) throw zeitoon::utility::dataTypeMismatch(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTdataTypeMismatchO(msg, owner) throw zeitoon::utility::dataTypeMismatch(msg,owner,__FILE__,__func__,__LINE__)
#define EXTdataTypeMismatchI(msg, inner) throw zeitoon::utility::dataTypeMismatch(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTdataTypeMismatchIO(msg, owner, inner) throw zeitoon::utility::dataTypeMismatch(msg,owner,__FILE__,__func__,__LINE__,inner)

//cantParseString
#define EXTcantParseString(msg) throw zeitoon::utility::cantParseString(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTcantParseStringO(msg, owner) throw zeitoon::utility::cantParseString(msg,owner,__FILE__,__func__,__LINE__)
#define EXTcantParseStringI(msg, inner) throw zeitoon::utility::cantParseString(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTcantParseStringIO(msg, owner, inner) throw zeitoon::utility::cantParseString(msg,owner,__FILE__,__func__,__LINE__,inner)

//outOfRange
#define EXToutOfRange(msg) throw zeitoon::utility::outOfRange(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXToutOfRangeO(msg, owner) throw zeitoon::utility::outOfRange(msg,owner,__FILE__,__func__,__LINE__)
#define EXToutOfRangeI(msg, inner) throw zeitoon::utility::outOfRange(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXToutOfRangeIO(msg, owner, inner) throw zeitoon::utility::outOfRange(msg,owner,__FILE__,__func__,__LINE__,inner)

//notImplemented
#define EXTnotImplemented(msg) throw zeitoon::utility::notImplemented(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTnotImplementedO(msg, owner) throw zeitoon::utility::notImplemented(msg,owner,__FILE__,__func__,__LINE__)
#define EXTnotImplementedI(msg, inner) throw zeitoon::utility::notImplemented(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTnotImplementedIO(msg, owner, inner) throw zeitoon::utility::notImplemented(msg,owner,__FILE__,__func__,__LINE__,inner)

//invalidName
#define EXTinvalidName(msg) throw zeitoon::utility::invalidName(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTinvalidNameO(msg, owner) throw zeitoon::utility::invalidName(msg,owner,__FILE__,__func__,__LINE__)
#define EXTinvalidNameI(msg, inner) throw zeitoon::utility::invalidName(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTinvalidNameIO(msg, owner, inner) throw zeitoon::utility::invalidName(msg,owner,__FILE__,__func__,__LINE__,inner)

//obsolete
#define EXTobsolete(msg) throw zeitoon::utility::obsolete(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTobsoleteO(msg, owner) throw zeitoon::utility::obsolete(msg,owner,__FILE__,__func__,__LINE__)
#define EXTobsoleteI(msg, inner) throw zeitoon::utility::obsolete(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTobsoleteIO(msg, owner, inner) throw zeitoon::utility::obsolete(msg,owner,__FILE__,__func__,__LINE__,inner)

//connectionError
#define EXTconnectionError(msg) throw zeitoon::utility::connectionError(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTconnectionErrorO(msg, owner) throw zeitoon::utility::connectionError(msg,owner,__FILE__,__func__,__LINE__)
#define EXTconnectionErrorI(msg, inner) throw zeitoon::utility::connectionError(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTconnectionErrorIO(msg, owner, inner) throw zeitoon::utility::connectionError(msg,owner,__FILE__,__func__,__LINE__,inner)

//DBError
#define EXTDBError(msg) throw zeitoon::utility::DBError(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTDBErrorO(msg, owner) throw zeitoon::utility::DBError(msg,owner,__FILE__,__func__,__LINE__)
#define EXTDBErrorI(msg, inner) throw zeitoon::utility::DBError(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTDBErrorIO(msg, owner, inner) throw zeitoon::utility::DBError(msg,owner,__FILE__,__func__,__LINE__,inner)

//duplicateName
#define EXTduplicateName(msg) throw zeitoon::utility::duplicateName(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTduplicateNameO(msg, owner) throw zeitoon::utility::duplicateName(msg,owner,__FILE__,__func__,__LINE__)
#define EXTduplicateNameI(msg, inner) throw zeitoon::utility::duplicateName(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTduplicateNameIO(msg, owner, inner) throw zeitoon::utility::duplicateName(msg,owner,__FILE__,__func__,__LINE__,inner)

//nullValue
#define EXTnullValue(msg) throw zeitoon::utility::nullValue(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTnullValueO(msg, owner) throw zeitoon::utility::nullValue(msg,owner,__FILE__,__func__,__LINE__)
#define EXTnullValueI(msg, inner) throw zeitoon::utility::nullValue(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTnullValueIO(msg, owner, inner) throw zeitoon::utility::nullValue(msg,owner,__FILE__,__func__,__LINE__,inner)

//networkFailure
#define EXTnetworkFailure(msg) throw zeitoon::utility::networkFailure(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTnetworkFailureO(msg, owner) throw zeitoon::utility::networkFailure(msg,owner,__FILE__,__func__,__LINE__)
#define EXTnetworkFailureI(msg, inner) throw zeitoon::utility::networkFailure(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTnetworkFailureIO(msg, owner, inner) throw zeitoon::utility::networkFailure(msg,owner,__FILE__,__func__,__LINE__,inner)

//accessDenied
#define EXTaccessDenied(msg) throw zeitoon::utility::accessDenied(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTaccessDeniedO(msg, owner) throw zeitoon::utility::accessDenied(msg,owner,__FILE__,__func__,__LINE__)
#define EXTaccessDeniedI(msg, inner) throw zeitoon::utility::accessDenied(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTaccessDeniedIO(msg, owner, inner) throw zeitoon::utility::accessDenied(msg,owner,__FILE__,__func__,__LINE__,inner)

//resourceNotAvailable
#define EXTresourceNotAvailable(msg) throw zeitoon::utility::resourceNotAvailable(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTresourceNotAvailableO(msg, owner) throw zeitoon::utility::resourceNotAvailable(msg,owner,__FILE__,__func__,__LINE__)
#define EXTresourceNotAvailableI(msg, inner) throw zeitoon::utility::resourceNotAvailable(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTresourceNotAvailableIO(msg, owner, inner) throw zeitoon::utility::resourceNotAvailable(msg,owner,__FILE__,__func__,__LINE__,inner)

//networkMaxRetryReached
#define EXTnetworkMaxRetryReached(msg) throw zeitoon::utility::networkMaxRetryReached(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTnetworkMaxRetryReachedO(msg, owner) throw zeitoon::utility::networkMaxRetryReached(msg,owner,__FILE__,__func__,__LINE__)
#define EXTnetworkMaxRetryReachedI(msg, inner) throw zeitoon::utility::networkMaxRetryReached(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTnetworkMaxRetryReachedIO(msg, owner, inner) throw zeitoon::utility::networkMaxRetryReached(msg,owner,__FILE__,__func__,__LINE__,inner)

//networkNoRetryTimeSet
#define EXTnetworkNoRetryTimeSet(msg) throw zeitoon::utility::networkNoRetryTimeSet(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTnetworkNoRetryTimeSetO(msg, owner) throw zeitoon::utility::networkNoRetryTimeSet(msg,owner,__FILE__,__func__,__LINE__)
#define EXTnetworkNoRetryTimeSetI(msg, inner) throw zeitoon::utility::networkNoRetryTimeSet(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTnetworkNoRetryTimeSetIO(msg, owner, inner) throw zeitoon::utility::networkNoRetryTimeSet(msg,owner,__FILE__,__func__,__LINE__,inner)

//filesystemFailure
#define EXTfilesystemFailure(msg) throw zeitoon::utility::filesystemFailure(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTfilesystemFailureO(msg, owner) throw zeitoon::utility::filesystemFailure(msg,owner,__FILE__,__func__,__LINE__)
#define EXTfilesystemFailureI(msg, inner) throw zeitoon::utility::filesystemFailure(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTfilesystemFailureIO(msg, owner, inner) throw zeitoon::utility::filesystemFailure(msg,owner,__FILE__,__func__,__LINE__,inner)



#endif //ZEITOON_UTILITY_EXCEPTIONS_HPP
