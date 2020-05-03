#pragma once

#include <spawn/spawn.hpp>

#include <ozo/execute.h>
#include <ozo/request.h>
#include <ozo/transaction.h>

#include <log4cpp/Category.hh>

#include "Types.h"
#include "db/impl/ErrorHandler.hpp"

namespace db {

namespace impl {

template <typename ConnectionInfo, typename Query, typename Timeout, typename Out> class BlockingRequest {
public:
    static void blockingRequest(ConnectionInfo &connectionInfo, const Query query, Timeout timeout, Out out) {
        // TODO: reorganize logger and log message
        //        static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("database"));
        //        LOG4CPP_DEBUG_S(logger) << "Database blocking request: " << query.text();
        boost::asio::io_service io_service;
        spawn::spawn(io_service, [&](const spawn::yield_context &yield) {
            ozo::error_code ec;
            auto connection = ozo::request(connectionInfo[io_service], query, timeout, out, yield[ec]);
            handleError(ec, connection);
        });
        io_service.run();
    }
};

template <typename Query, typename Timeout, typename Out> class Request {
public:
    static void request(ConnectionPoolProvider &connectionProvider, const Query query, Timeout timeout, Out out,
                        const spawn::yield_context &yield) {
        // TODO: reorganize logger and log message
        //        static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("database"));
        //        LOG4CPP_DEBUG_S(logger) << "Database asynchronous request: " << query.text();
        ozo::error_code ec;
        auto connection = ozo::request(connectionProvider, query, timeout, out, yield[ec]);
        handleError(ec, connection);
    }
};

template <typename Query, typename Timeout> class Execute {
public:
    static void execute(ConnectionPoolProvider &connectionProvider, const Query query, Timeout timeout,
                        const spawn::yield_context &yield) {
        // TODO: reorganize logger and log message
        //        static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("database"));
        //        LOG4CPP_DEBUG_S(logger) << "Database asynchronous execute: " << query.text();
        ozo::error_code ec;
        auto connection = ozo::execute(connectionProvider, query, timeout, yield[ec]);
        handleError(ec, connection);
    }
};

} // namespace impl
} // namespace db