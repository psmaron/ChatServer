#pragma once

#include <iostream>
#include <tuple>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include <boost/algorithm/string.hpp>

#include "CmdLineHelpFunction.hpp"
#include "LoggingConfig.h"

namespace conf {

std::tuple<unsigned int, std::string, bool> parseCommandLineArguments(int argc, char **argv) {
    namespace po = boost::program_options;
    po::options_description desc("Usage: ChatServer [-h] [-d] [-p <port number>] [-c <config path>]");
    // clang-format off
    desc.add_options()
        ("help,h", "Displays help message")
        ("debug,d", "Sets logging level to DEBUG")
        ("port,p", po::value<unsigned int>()->default_value(defaultPort), "Starts server at given port number")
        ("config,c", po::value<std::string>()->default_value(defaultConfigPath),"Path for logger configuration file");
    // clang-format on

    po::variables_map variablesMap;

    parseArgs(argc, argv, desc, variablesMap);
    printHelpAndExit(desc, variablesMap);
    printProgramInvoked(argc, argv);

    std::string loggerConfigPath{variablesMap["config"].as<std::string>()};
    boost::trim(loggerConfigPath);
    return {variablesMap["port"].as<unsigned int>(), loggerConfigPath, variablesMap.count("debug")};
}

} // namespace conf
