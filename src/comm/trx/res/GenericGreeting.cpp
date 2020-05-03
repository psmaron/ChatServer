#include "GenericGreeting.h"

#include <sstream>

namespace comm::trx::res {

GenericGreeting::GenericGreeting(int clientId) : Response(Recipients::ONLY_SENDER), clientId(clientId) {
}

std::ostringstream GenericGreeting::serialize() const {
    std::ostringstream message;
    // clang-format off
    message << XML_PREAMBLE
        << R"(<GenericGreeting>)"
               R"(<DSVersion>)" << dsVersion << R"(</DSVersion>)"
               R"(<NewDSVersion>)" << newDsVersion << R"(</NewDSVersion>)"
               R"(<NetID>)" << clientId << R"(</NetID>)"
           R"(</GenericGreeting>)";
    // clang-format on
    return message;
}

} // namespace comm::trx::res
