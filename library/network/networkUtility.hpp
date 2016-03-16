//
// Created by ajl on 3/6/16.
//

#ifndef ZDS_NETWORKUTILITY_HPP
#define ZDS_NETWORKUTILITY_HPP

#include <uv.h>
#include <string>
#include <utility/exceptionex.hpp>

#define uvEXT(r, msg) if (r) EXTnetworkFailure(std::string(msg) + ".\n\t" + uv_err_name(r) + "[" +std::to_string(r) + "]: " + uv_strerror(r));
#define uvEXTO(r, msg, owner) if (r) EXTnetworkFailureO(std::string(msg) + ".\n\t" + uv_err_name(r) + "[" +std::to_string(r) + "]: " + uv_strerror(r), owner);

namespace zeitoon {
namespace utility {

class networkUtility {
public:
	static sockaddr *uv_resolveAddress(uv_loop_t *loop, std::string address, std::string service) {
		using zeitoon::utility::networkFailure;
		struct addrinfo hints;
		hints.ai_family = PF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = 0;

		uv_getaddrinfo_t resolver;
		int r = uv_getaddrinfo(loop, &resolver, NULL, address.c_str(), service.c_str(), &hints);
		uvEXTO(r, "getaddrinfo call failed", "networkUtility::uv_resolveAddress");
		sockaddr *temp = (sockaddr *) calloc(1, sizeof(sockaddr));
		(*temp) = *(resolver.addrinfo->ai_addr);
		free(resolver.addrinfo);
		return temp;// Do not forget to free temp
	}

};

}
}

#endif //ZDS_NETWORKUTILITY_HPP
