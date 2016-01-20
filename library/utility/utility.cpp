/*
 * utility.cpp
 *
 *  Created on: Sep 4, 2015
 *      Author: ajl
 */

#include "utility/utility.hpp"

namespace zeitoon {
namespace utility {

/**
 * listi as caracter haaye khaas ke dar encodeString va decodeString be kar miravand.
 */
const char JSONUtility::specialChars[specialCharsLen][2] = { { '\\', '\\' },  { '\b', 'b' },
		{ '\f', 'f' }, { '\n', 'n' }, { '\r', 'r' }, { '\t', 't' } };//{ '"', '"' }, { '/', '/' },

/**
 * character haye default ke dar tavaabe ye trim() be kaar miravand.
 */
const char* Strings::defIgnrChrs = " \r\n\t";


const char* MessageTypes::typeString[__MAX] = { "fire", "call", "callback" };

}
}
