#include "SignalHandler.h"

#include <functional>
#include <memory>

#include <boost/asio/signal_set.hpp>

#include <log4cpp/Category.hh>

#include "conf/LoggingConfig.h"

namespace core {

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("signalHandler"));

static const std::unordered_map<int, std::string_view> signalName = {
    {SIGINT, "SIGINT"}, {SIGTERM, "SIGTERM"}, {SIGHUP, "SIGHUP"}};

void signalsHandler(const boost::system::error_code &error, int signalNumber, std::shared_ptr<boost::asio::signal_set> signals) {
    if (error) {
        LOG4CPP_ERROR_S(logger) << "Error during handling signal: " << error.message();
    } else {
        switch (signalNumber) {
        case SIGINT:
            [[fallthrough]];
        case SIGTERM:
            LOG4CPP_INFO_S(logger) << "Received " << signalName.at(signalNumber) << ", shutting down";
            signals->get_io_context().stop();
            return;
        case SIGHUP:
            LOG4CPP_INFO_S(logger) << "Caught " << signalName.at(signalNumber) << " signal, changing logging level";
            conf::setDebugLevelLoggingTo(not conf::isDebugLevelLoggingEnabled());
            break;
        default:
            LOG4CPP_ERROR_S(logger) << "Received registered signal without implemented callback. Doing nothing";
        }
    }
    signals->async_wait(std::bind(signalsHandler, std::placeholders::_1, std::placeholders::_2, signals));
}

void connectSignals(boost::asio::io_service &io_service) {
    auto signals = std::make_shared<boost::asio::signal_set>(io_service, SIGTERM, SIGHUP, SIGINT);
    signals->async_wait(std::bind(signalsHandler, std::placeholders::_1, std::placeholders::_2, signals));
}

} // namespace core
