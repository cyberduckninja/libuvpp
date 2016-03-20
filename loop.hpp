#pragma once

#include "error.hpp"

#include <memory>
#include <functional>

namespace libuvpp {

    class loop {
    public:
        loop(bool use_default = false)
                : default_loop(use_default),
                  m_uv_loop(use_default ? uv_default_loop() : new uv_loop_t, [this](uv_loop_t *loop) {
                      destroy(loop);
                  }) {
            if (!default_loop && uv_loop_init(m_uv_loop.get())) {
                exit(2);
            }
        }


        ~loop() {
            if (m_uv_loop.get()) {

                uv_loop_close(m_uv_loop.get());
            }
        }

        loop(const loop &) = delete;

        loop &operator=(const loop &) = delete;

        loop(loop &&other)
                : m_uv_loop(std::forward<decltype(other.m_uv_loop)>(other.m_uv_loop)) {

        }

        loop &operator=(loop &&other) {
            if (this != &other) {
                m_uv_loop = std::forward<decltype(other.m_uv_loop)>(other.m_uv_loop);
            }
            return *this;
        }


        uv_loop_t *get() {
            return m_uv_loop.get();
        }


        bool run() {
            return uv_run(m_uv_loop.get(), UV_RUN_DEFAULT) == 0;
        }


        bool run_once() {
            return uv_run(m_uv_loop.get(), UV_RUN_ONCE) == 0;
        }


        void update_time() {
            uv_update_time(m_uv_loop.get());
        }

        int64_t now() {
            return uv_now(m_uv_loop.get());
        }


        void stop() {
            uv_stop(m_uv_loop.get());
        }

    private:


        typedef std::function<void(uv_loop_t *)> Deleter;

        void destroy(uv_loop_t *loop) const {
            if (!default_loop) {
                delete loop;
            }
        }

        bool default_loop;
        std::unique_ptr<uv_loop_t, Deleter> m_uv_loop;
    };
}

