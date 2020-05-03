#include <boost/asio/io_service.hpp>

#include <log4cpp/Category.hh>

#include "conf/CmdLineArgsParser.hpp"
#include "conf/LoggingConfig.h"
#include "core/Server.h"
#include "core/SignalHandler.h"
#include "db/DatabaseProxy.hpp"
#include "utils/Logging.hpp"

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("main"));

// TODO: try reorganize try-catch mess

void runEventLoop(boost::asio::io_service &io_service) {
    for (;;) {
        try {
            io_service.run();
            break;
        } catch (const std::exception &e) {
            LOG4CPP_ERROR_S(logger) << "Unexpected exception thrown: " << e.what() << ", server continues to work";
        } catch (...) {
            LOG4CPP_ERROR_S(logger) << "Unexpected exception thrown, server continues to work";
        }
    }
}

int main(int argc, char **argv) {
    try {
        auto [port, loggerConfigFilePath, isDebugModeEnabled] = conf::parseCommandLineArguments(argc, argv);
        conf::configureLogging(loggerConfigFilePath, isDebugModeEnabled);
        boost::asio::io_service io_service;
        core::connectSignals(io_service);
        auto databaseProxy = std::make_shared<db::DatabaseProxy>(io_service);
        core::Server::initialize(io_service, databaseProxy, port);
        runEventLoop(io_service);
    } catch (const core::Error &e) {
        LOG4CPP_ERROR_S(logger) << e.what() << ", exiting with " << utils::log(e);
        return static_cast<core::Error>(e).getErrorCode();
    } catch (const std::exception &e) {
        LOG4CPP_ERROR_S(logger) << e.what() << ", exiting (no error code).";
        return -1;
    }
    return 0;
}
