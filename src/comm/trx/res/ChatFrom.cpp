#include "ChatFrom.h"

#include <sstream>
#include <utility>

namespace comm::trx::res {

ChatFrom::ChatFrom(std::string message, models::ClientsIdSet recipientsList)
    : CustomIdRecipientsResponse(std::move(recipientsList)), message(std::move(message)) {
}

std::ostringstream ChatFrom::serialize() const {
    std::ostringstream msg;
    // clang-format off
    msg << XML_PREAMBLE
        << R"(<ChatFrom>)"
            << R"(<Message>)" << message << R"(</Message>)";
    for (auto &r : recipientsList)
        msg << R"(<Recipients>)" << r << R"(</Recipients>)";
    msg << R"(</ChatFrom>)";
    // clang-format on
    return msg;
}

} // namespace comm::trx::res