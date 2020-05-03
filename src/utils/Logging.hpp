#pragma once

#include <log4cpp/Category.hh>

#include "core/CoreErrors.h"
#include "models/ClientData.h"

namespace utils {
inline std::string log(const std::shared_ptr<models::ClientData> &clientData) {
    return "[" + std::to_string(clientData->getId()) + "/" + clientData->getChatUsername() + "] ";
}

template <class error> inline std::string log(const error &codeError) {
    static_assert(std::is_base_of<core::Error, error>::value);

    return "errorCode: " + std::to_string(codeError.getErrorCode()) + " ";
}
} // namespace utils
