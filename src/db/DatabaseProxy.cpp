#include "DatabaseProxy.hpp"

#include <thread>

namespace db {

DatabaseProxy::DatabaseProxy(boost::asio::io_service &io_service)
    : connectionPool(std::move(createConnectionPool())), connectionProvider(connectionPool, io_service) {
}

impl::ConnectionPool DatabaseProxy::createConnectionPool() {
    ozo::connection_pool_config connectionPoolConfig;
    connectionPoolConfig.capacity = 2 * std::thread::hardware_concurrency();
    connectionPoolConfig.queue_capacity = 124; // maximum number of queued requests waiting for available connection
    connectionPoolConfig.idle_timeout = std::chrono::seconds(60);
    connectionPoolConfig.lifespan = std::chrono::hours(24);

    return ozo::connection_pool(connectionInfo, connectionPoolConfig);
}

std::string DatabaseProxy::getConnectionString() {
    return "host=" + getEnvironmentVariableOrDefault("PGHOST") + " port=" + getEnvironmentVariableOrDefault("PGPORT") +
           " dbname=" + getEnvironmentVariableOrDefault("PGDATABASE") + " user=" + getEnvironmentVariableOrDefault("PGUSER") +
           " password=" + getEnvironmentVariableOrDefault("PGPASSWORD");
}

std::string DatabaseProxy::getEnvironmentVariableOrDefault(const std::string &variableName) {
    if (const auto &env = std::getenv(variableName.c_str())) {
        return env;
    }
    auto defaultValue = defaultEnvironmentVariablesValues.at(variableName);
    LOG4CPP_INFO_S(logger) << "Environment variable " << variableName << " is not set, the default value: " << defaultValue
                           << " will be used.";
    return defaultValue;
}

} // namespace db
