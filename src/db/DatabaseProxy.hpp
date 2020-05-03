#pragma once

#include <spawn/spawn.hpp>

#include <log4cpp/Category.hh>

#include <ozo/connection_info.h>
#include <ozo/connection_pool.h>
#include <ozo/execute.h>
#include <ozo/request.h>
#include <ozo/transaction.h>

#include <log4cpp/Category.hh>

#include "db/DatabaseErrors.h"
#include "db/Transaction.hpp"
#include "db/impl/DatabaseProxyImpl.hpp"
#include "db/impl/Types.h"

namespace db {

class DatabaseProxy {
public:
    explicit DatabaseProxy(boost::asio::io_service &io_service);

    DatabaseProxy(DatabaseProxy &) = delete;

    // TODO: rename the 'fake' param, it's not accurate, or try to remove it altogether
    template <bool fake = false> auto beginTransaction(const spawn::yield_context &yield) {
        return TransactionProxy{std::move(ozo::begin(connectionProvider, yield))};
    }

    template <typename Query, typename Timeout, typename Out> void blockingRequest(const Query query, Timeout timeout, Out out) {
        impl::BlockingRequest<decltype(connectionInfo), Query, Timeout, Out>::blockingRequest(connectionInfo, query, timeout,
                                                                                              out);
    }

    template <typename Query, typename Timeout, typename Out>
    void request(const Query query, Timeout timeout, Out out, const spawn::yield_context &yield) {
        impl::Request<Query, Timeout, Out>::request(connectionProvider, query, timeout, out, yield);
    }

    template <typename Query, typename Timeout>
    void execute(const Query query, Timeout timeout, const spawn::yield_context &yield) {
        impl::Execute<Query, Timeout>::execute(connectionProvider, query, timeout, yield);
    }

    impl::ConnectionPool createConnectionPool();

    static std::string getConnectionString();
    static std::string getEnvironmentVariableOrDefault(const std::string &variableName);

    inline static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("database"));
    inline static const std::map<std::string, std::string> defaultEnvironmentVariablesValues = {
        {"PGHOST", "localhost"}, {"PGDATABASE", "postgres"}, {"PGUSER", "postgres"}, {"PGPASSWORD", ""}, {"PGPORT", "5432"},
    };
    inline static const auto connectionInfo = ozo::connection_info(getConnectionString());
    impl::ConnectionPool connectionPool;
    impl::ConnectionPoolProvider connectionProvider;
};

} // namespace db
