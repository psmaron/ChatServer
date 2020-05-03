#pragma once

#include <string>

namespace conf {

void configureLogging(const std::string &loggerConfigFilePath, bool debugMode);

void setDebugLevelLoggingTo(bool newValue);

bool isDebugLevelLoggingEnabled();

} // namespace conf