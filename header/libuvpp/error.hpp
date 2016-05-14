#pragma once

#include <assert.h>
#include <stdexcept>
#include <string>
#include <uv.h>

namespace libuvpp {
    class error {
    public:
        error(int c) : m_error(c) {
        }

    public:
        explicit operator bool() const {
            return m_error != 0;
        }

        const char *str() const {
            return uv_strerror(m_error);
        }

    private:
        int m_error;
    };
}

