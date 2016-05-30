/** ***   WARNING   ***
  * This file has been generated using CodeGenerator.
  *   any manual modification will be erased in next cmake build.
  * Generated on : 2016-05-29 21:51:13.482669
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


} // zeitoon
} // _core


//Macro here -----------------------------------------------------------------

//exceptionRedirect
#define EXTexceptionRedirect(msg) throw zeitoon::_core::exceptionRedirect(msg,this->getNameAndType(),__FILE__,__func__,__LINE__)
#define EXTexceptionRedirectO(msg, owner) throw zeitoon::_core::exceptionRedirect(msg,owner,__FILE__,__func__,__LINE__)
#define EXTexceptionRedirectI(msg, inner) throw zeitoon::_core::exceptionRedirect(msg,this->getNameAndType(),__FILE__,__func__,__LINE__,inner)
#define EXTexceptionRedirectIO(msg, owner, inner) throw zeitoon::_core::exceptionRedirect(msg,owner,__FILE__,__func__,__LINE__,inner)



#endif //ZEITOON__CORE_EXCEPTIONS_HPP
