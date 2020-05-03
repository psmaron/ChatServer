#pragma once

#include "ChatRequest.h"
#include "models/ChatStatus.hpp"

namespace comm::trx::req {

class ChatUserName : public ChatRequest {
public:
    ChatUserName(const boost::property_tree::ptree &requestXmlTree);

    template <std::size_t N> decltype(auto) get() const {
        if constexpr (N == 0)
            return id;
        else if constexpr (N == 1)
            return chatUsername;
        else if constexpr (N == 2)
            return chatStatus;
        else if constexpr (N == 3)
            return isOtherSystemUser;
    }

private:
    int id = -1;
    std::string chatUsername;
    models::ChatStatus chatStatus;
    bool isOtherSystemUser;
};

} // namespace comm::trx::req

namespace std {

template <> struct tuple_size<comm::trx::req::ChatUserName> : std::integral_constant<std::size_t, 4> {};

template <std::size_t N> struct tuple_element<N, comm::trx::req::ChatUserName> {
    using type = decltype(std::declval<comm::trx::req::ChatUserName>().get<N>());
};

} // namespace std
