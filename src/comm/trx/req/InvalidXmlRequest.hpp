#pragma once

#include "InvalidRequestDescription.h"
#include "comm/trx/req/InvalidRequest.h"

namespace comm::trx::req {

class InvalidXmlRequest : public InvalidRequest {
public:
    InvalidXmlRequest(InvalidRequestDescription requestDescription) : requestDescription(std::move(requestDescription)) {
    }

    template <std::size_t N> decltype(auto) get() const {
        if constexpr (N == 0)
            return requestDescription;
    }

private:
    const InvalidRequestDescription requestDescription;
};
} // namespace comm::trx::req

namespace std {

template <> struct tuple_size<comm::trx::req::InvalidXmlRequest> : std::integral_constant<std::size_t, 1> {};

template <std::size_t N> struct tuple_element<N, comm::trx::req::InvalidXmlRequest> {
    using type = decltype(std::declval<comm::trx::req::InvalidXmlRequest>().get<N>());
};

} // namespace std
