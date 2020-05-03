#pragma once

#include "ChatRequest.h"

namespace comm::trx::req {

class StatusNotificationRequest : public ChatRequest {
public:
    StatusNotificationRequest(const boost::property_tree::ptree &requestXmlTree);

    template <std::size_t N> decltype(auto) get() const {
        if constexpr (N == 0)
            return isNotificationOn;
    }

private:
    bool isNotificationOn;
};

} // namespace comm::trx::req

namespace std {

template <> struct tuple_size<comm::trx::req::StatusNotificationRequest> : std::integral_constant<std::size_t, 1> {};

template <std::size_t N> struct tuple_element<N, comm::trx::req::StatusNotificationRequest> {
    using type = decltype(std::declval<comm::trx::req::StatusNotificationRequest>().get<N>());
};

} // namespace std