#pragma once

#include <string>

#include "Response.hpp"

namespace comm::trx::res {

class GenericGreeting : public Response {
public:
    GenericGreeting(int clientId);

    std::ostringstream serialize() const override;

private:
    static constexpr std::string_view dsVersion = "ChatServer  v0.0.1";
    static constexpr std::string_view newDsVersion = "ChatServer  v0.0.2";
    int clientId;
};

} // namespace comm::trx::res