#pragma once

#include <string>

namespace comm::trx {

enum class InvalidResponseType : bool { Error = true, NotError = false };

class InvalidRequestDescription {
public:
    InvalidRequestDescription(std::string inMessage, InvalidResponseType type, int transactionId)
        : message(std::move(inMessage)), transactionId(transactionId), responseType(type) {
    }

    const std::string message;
    const int transactionId;
    const InvalidResponseType responseType;
};
} // namespace comm::trx
