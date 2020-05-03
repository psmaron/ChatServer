#include <iostream>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace conf {

static const unsigned int defaultPort = 1719;
static const std::string defaultConfigPath = "./logger.config";
namespace po = boost::program_options;

void parseArgs(char argc, char **argv, const po::options_description &desc, po::variables_map &variablesMap) {
    try {
        po::store(po::parse_command_line(argc, argv, desc), variablesMap);
        po::notify(variablesMap);
    } catch (po::error &e) {
        std::cout << "Wrong program parameters provided: " << e.what() << ". Exiting...\n";
        std::cout << desc << std::endl;
        std::exit(1);
    }
}

void printProgramInvoked(char argc, char **argv) {

    std::cout << "Program was invoked as: " << *argv << " ";
    while (--argc)
        std::cout << *(++argv) << " ";
    std::cout << std::endl;
}

void printHelpAndExit(const po::options_description &desc, const po::variables_map &variablesMap) {

    if (variablesMap.count("help")) {
        std::cout << desc << std::endl;
        std::exit(1);
    }
}
} // namespace conf
