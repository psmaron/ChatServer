#include "LoggingConfig.h"

#include <chrono>
#include <locale>
#include <unistd.h>

#include <log4cpp/Category.hh>
#include <log4cpp/Configurator.hh>
#include <log4cpp/NDC.hh>
#include <log4cpp/PropertyConfigurator.hh>

namespace conf {

static log4cpp::Category &logger = log4cpp::Category::getRoot();

static bool debugLevelEnabled = false;

std::string createTimeZoneString() {
    constexpr int lengthOfTimezoneString = 6; // output from strftime: '-0400'
    std::time_t time = std::time(nullptr);
    std::string timezone(lengthOfTimezoneString, ' ');
    if (std::strftime(timezone.data(), timezone.length(), "%z", std::localtime(&time))) {
        timezone.insert(3, ":");
    } else {
        LOG4CPP_WARN_S(logger) << "Cannot create time zone string. Using +00:00 time zone";
        return std::string("+00:00");
    }
    return timezone;
}

void configureLogging(const std::string &loggerConfigFilePath, bool isDebugModeEnabled) {
    try {
        log4cpp::PropertyConfigurator::configure(loggerConfigFilePath);
    } catch (log4cpp::ConfigureFailure &e) {
        std::cerr << "Cannot configure logger, error: " << e.what() << std::endl;
        std::exit(1);
    }

    auto pid = getpid();
    auto timezoneWithProgramNameAndPid = createTimeZoneString();
    timezoneWithProgramNameAndPid += "asioServer[" + std::to_string(pid) + ']';
    log4cpp::NDC().push(timezoneWithProgramNameAndPid);
    setDebugLevelLoggingTo(isDebugModeEnabled);
    LOG4CPP_INFO_S(logger) << "Configured logging using " << loggerConfigFilePath;
}

void setDebugLevelLoggingTo(bool newValue) {
    debugLevelEnabled = newValue;
    log4cpp::Category::setRootPriority(debugLevelEnabled ? log4cpp::Priority::DEBUG : log4cpp::Priority::INFO);
    if (newValue) {
        LOG4CPP_DEBUG_S(logger) << "Debug output ENABLED";
    } else {
        LOG4CPP_INFO_S(logger) << "Debug output DISABLED";
    }
}

bool isDebugLevelLoggingEnabled() {
    return debugLevelEnabled;
}

} // namespace conf