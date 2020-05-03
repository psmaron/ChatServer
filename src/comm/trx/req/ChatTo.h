#pragma once

#include <unordered_set>

#include "ChatRequest.h"
#include "models/ChatStatus.hpp"

namespace comm::trx::req {

class ChatTo : public ChatRequest {
public:
    ChatTo(const boost::property_tree::ptree &requestXmlTree);

    template <std::size_t N> decltype(auto) get() const {
        if constexpr (N == 0)
            return message;
        else if constexpr (N == 1)
            return recipients;
    }

private:
    std::string message;
    std::unordered_set<int> recipients;
};

} // namespace comm::trx::req

namespace std {

template <> struct tuple_size<comm::trx::req::ChatTo> : std::integral_constant<std::size_t, 2> {};

template <std::size_t N> struct tuple_element<N, comm::trx::req::ChatTo> {
    using type = decltype(std::declval<comm::trx::req::ChatTo>().get<N>());
};

} // namespace std
