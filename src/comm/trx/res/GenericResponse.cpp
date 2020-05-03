#include "GenericResponse.h"

#include <sstream>

namespace comm::trx::res {

GenericResponse::GenericResponse(InvalidRequestDescription requestDescription)
    : Response(Recipients::ONLY_SENDER), requestDescription(std::move(requestDescription)) {
}

std::ostringstream GenericResponse::serialize() const {
    std::ostringstream message;
    // clang-format off
    message << XML_PREAMBLE
        << R"(<GenericResponse>)"
            << R"(<TransID>)" << requestDescription.transactionId << R"(</TransID>)"
            << R"(<IsError>)" << std::boolalpha << static_cast<bool>(requestDescription.responseType) << R"(</IsError>)"
            << R"(<Message>)" << requestDescription.message << R"(</Message>)"
        << R"(</GenericResponse>)";
    // clang-format on
    return message;
}

} // namespace comm::trx::res
