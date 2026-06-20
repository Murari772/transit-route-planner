#pragma once

#ifndef ASIO_STANDALONE
#define ASIO_STANDALONE
#endif

#include <asio/io_context.hpp>
#include <asio/post.hpp>
#include <asio/dispatch.hpp>
#include <asio/ip/address.hpp>

namespace asio {
class io_service : public io_context {
public:
    io_service() : io_context() {}
    explicit io_service(int concurrency_hint) : io_context(concurrency_hint) {}

    template<typename Handler>
    void post(Handler&& handler) {
        asio::post(static_cast<io_context&>(*this), std::forward<Handler>(handler));
    }

    template<typename Handler>
    void dispatch(Handler&& handler) {
        asio::dispatch(static_cast<io_context&>(*this), std::forward<Handler>(handler));
    }
};
} // namespace asio

#include "crow.h"
