#pragma once

#include <boost/optional.hpp>

namespace models {

enum ChatStatus : unsigned char { LOGOUT = 0, LOGIN = 1, AWAY = 2, INVALID = 3 };

inline ChatStatus convertToChatStatus(int c) {
    if (c < 0 || c >= INVALID) {
        return INVALID;
    }
    return static_cast<ChatStatus>(c);
}

} // namespace models
