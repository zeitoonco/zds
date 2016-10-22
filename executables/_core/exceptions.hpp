/** ***   WARNING   ***
  * This file has been generated using CodeGenerator.
  *   any manual modification will be erased in next cmake build.
  *
  * By ajl.
  */

#ifndef ZEITOON__CORE_EXCEPTIONS_HPP
#define ZEITOON__CORE_EXCEPTIONS_HPP

#include <utility/exceptionex.hpp>

namespace zeitoon {
namespace _core {

class exceptionRedirect : public exceptionExWithInnerEx {
public:
	exceptionRedirect(std::string imessage) :
			exceptionRedirect(imessage, "", "", "", -1) {
	}

	exceptionRedirect(std::string imessage, exceptionEx &innerException) :
			exceptionRedirect(imessage, "", "", "", -1, innerException) {
	}

	exceptionRedirect(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	exceptionRedirect(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Exception received from service";
	}

	virtual std::string getName() {
		return "exceptionRedirect";
	}

};

class CMDSyncTimeOut : public exceptionExWithInnerEx {
public:
	CMDSyncTimeOut(std::string imessage) :
			CMDSyncTimeOut(imessage, "", "", "", -1) {
	}

	CMDSyncTimeOut(std::string imessage, exceptionEx &innerException) :
			CMDSyncTimeOut(imessage, "", "", "", -1, innerException) {
	}

	CMDSyncTimeOut(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	CMDSyncTimeOut(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "Sync Command timed out - more than 3 milliseconds without no responce. ";
	}

	virtual std::string getName() {
		return "CMDSyncTimeOut";
	}

};

class PingFailure : public exceptionExWithInnerEx {
public:
	PingFailure(std::string imessage) :
			PingFailure(imessage, "", "", "", -1) {
	}

	PingFailure(std::string imessage, exceptionEx &innerException) :
			PingFailure(imessage, "", "", "", -1, innerException) {
	}

	PingFailure(std::string imessage, std::string owner, std::string file, std::string function, int line,
	                 exceptionEx &innerException) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line, innerException) {
	}

	PingFailure(std::string imessage, std::string owner, std::string file, std::string function, int line) :
			exceptionExWithInnerEx(getDefTitle(), imessage, owner, file, function, line) {
	}

	virtual std::string getDefTitle() {
		return "PING FAILED";
	}

	virtual std::string getName() {
		return "PingFailure";
	}

};


} // zeitoon
} // _core


//Macro here -----------------------------------------------------------------

//exceptionRedirect
#define EXTexceptionRedirect(msg) throw zeitoon::_core::exceptionRedirect(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTexceptionRedirectO(msg, owner) throw zeitoon::_core::exceptionRedirect(msg,owner,__FILE__,__func__,__LINE__)
#define EXTexceptionRedirectI(msg, inner) throw zeitoon::_core::exceptionRedirect(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTexceptionRedirectIO(msg, owner, inner) throw zeitoon::_core::exceptionRedirect(msg,owner,__FILE__,__func__,__LINE__,inner)

//CMDSyncTimeOut
#define EXTCMDSyncTimeOut(msg) throw zeitoon::_core::CMDSyncTimeOut(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTCMDSyncTimeOutO(msg, owner) throw zeitoon::_core::CMDSyncTimeOut(msg,owner,__FILE__,__func__,__LINE__)
#define EXTCMDSyncTimeOutI(msg, inner) throw zeitoon::_core::CMDSyncTimeOut(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTCMDSyncTimeOutIO(msg, owner, inner) throw zeitoon::_core::CMDSyncTimeOut(msg,owner,__FILE__,__func__,__LINE__,inner)

//PingFailure
#define EXTPingFailure(msg) throw zeitoon::_core::PingFailure(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTPingFailureO(msg, owner) throw zeitoon::_core::PingFailure(msg,owner,__FILE__,__func__,__LINE__)
#define EXTPingFailureI(msg, inner) throw zeitoon::_core::PingFailure(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTPingFailureIO(msg, owner, inner) throw zeitoon::_core::PingFailure(msg,owner,__FILE__,__func__,__LINE__,inner)



#endif //ZEITOON__CORE_EXCEPTIONS_HPP
