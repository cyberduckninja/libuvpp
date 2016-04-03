#pragma once

#include "stream.hpp"
#include "net.hpp"
#include "loop.hpp"

namespace libuvpp {
    class tcp : public stream<uv_tcp_t> {
    public:
        tcp() : stream() {
            uv_tcp_init(uv_default_loop(), get());
        }

        tcp(loop &l) : stream() {
            uv_tcp_init(l.get(), get());
        }

        bool no_delay(bool enable) {
            return uv_tcp_nodelay(get(), enable ? 1 : 0) == 0;
        }

        bool keepalive(bool enable, unsigned int delay) {
            return uv_tcp_keepalive(get(), enable ? 1 : 0, delay) == 0;
        }

        bool simultanious_accepts(bool enable) {
            return uv_tcp_simultaneous_accepts(get(), enable ? 1 : 0) == 0;
        }

        // FIXME: refactor with getaddrinfo
        bool bind(const std::string &ip, int port) {
            ip4_addr addr = to_ip4_addr(ip, port);
            return uv_tcp_bind(
                    get(),
                    reinterpret_cast<sockaddr *>(&addr),
                    0
            ) == 0;
        }

        bool bind6(const std::string &ip, int port) {
            ip6_addr addr = to_ip6_addr(ip, port);
            return uv_tcp_bind(
                    get(),
                    reinterpret_cast<sockaddr *>(&addr),
                    0
            ) == 0;
        }

        bool connect(const std::string &ip, int port, CallbackWithResult callback) {
            callbacks::store(
                    get()->data,
                    internal::uv_cid_connect,
                    callback
            );
            ip4_addr addr = to_ip4_addr(ip, port);
            return uv_tcp_connect(
                    new uv_connect_t,
                    get(),
                    reinterpret_cast<const sockaddr *>(&addr),
                    [](uv_connect_t *req, int status) {
                        std::unique_ptr<uv_connect_t> reqHolder(req);
                        callbacks::invoke<decltype(callback)>(
                                req->handle->data,
                                internal::uv_cid_connect,
                                error(status)
                        );
                    }
            ) == 0;
        }

        bool connect6(const std::string &ip, int port, CallbackWithResult callback) {
            callbacks::store(
                    get()->data,
                    internal::uv_cid_connect6,
                    callback
            );
            ip6_addr addr = to_ip6_addr(ip, port);
            return uv_tcp_connect(
                    new uv_connect_t,
                    get(),
                    reinterpret_cast<const sockaddr *>(&addr),
                    [](uv_connect_t *req, int status) {
                        std::unique_ptr<uv_connect_t> reqHolder(req);
                        callbacks::invoke<decltype(callback)>(
                                req->handle->data,
                                internal::uv_cid_connect6,
                                error(status)
                        );
                    }
            ) == 0;
        }

        bool get_sock_name(bool &ip4, std::string &ip, int &port) {
            sockaddr_storage addr;
            int len = sizeof(addr);
            if (uv_tcp_getsockname(get(), reinterpret_cast<struct sockaddr *>(&addr), &len) == 0) {
                ip4 = (addr.ss_family == AF_INET);
                if (ip4)
                    return from_ip4_addr(reinterpret_cast<ip4_addr *>(&addr), ip, port);
                else
                    return from_ip6_addr(reinterpret_cast<ip6_addr *>(&addr), ip, port);
            }
            return false;
        }

        bool get_peer_name(bool &ip4, std::string &ip, int &port) {
            sockaddr_storage addr;
            int len = sizeof(addr);
            if (uv_tcp_getpeername(get(), reinterpret_cast<struct sockaddr *>(&addr), &len) == 0) {
                ip4 = (addr.ss_family == AF_INET);
                if (ip4)
                    return from_ip4_addr(reinterpret_cast<ip4_addr *>(&addr), ip, port);
                else
                    return from_ip6_addr(reinterpret_cast<ip6_addr *>(&addr), ip, port);
            }
            return false;
        }
    };
}
