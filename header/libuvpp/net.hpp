#pragma once

#include <memory>
#include <utility>
#include <sstream>
#include <netinet/in.h>
#include <uv.h>

namespace libuvpp {
    typedef sockaddr_in ip4_addr;
    typedef sockaddr_in6 ip6_addr;

    inline ip4_addr to_ip4_addr(const std::string &ip, int port) {
        ip4_addr result;
        int res = 0;
        if ((res = uv_ip4_addr(ip.c_str(), port, &result)) != 0)
            exit(1);
        return result;
    }

    inline ip6_addr to_ip6_addr(const std::string &ip, int port) {
        ip6_addr result;
        int res = 0;
        if ((res = uv_ip6_addr(ip.c_str(), port, &result)) != 0)
            exit(1);
        return result;
    }

    inline bool from_ip4_addr(ip4_addr *src, std::string &ip, int &port) {
        char dest[16];
        if (uv_ip4_name(src, dest, 16) == 0) {
            ip = dest;
            port = static_cast<int>(ntohs(src->sin_port));
            return true;
        }
        return false;
    }

    inline bool from_ip6_addr(ip6_addr *src, std::string &ip, int &port) {
        char dest[46];
        if (uv_ip6_name(src, dest, 46) == 0) {
            ip = dest;
            port = static_cast<int>(ntohs(src->sin6_port));
            return true;
        }
        return false;
    }
}
#undef fs
