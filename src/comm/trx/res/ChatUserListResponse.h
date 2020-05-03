#pragma once

#include <iosfwd>

#include "Response.hpp"
#include "models/ClientFwd.h"

namespace comm::trx::res {

class ChatUserListResponse : public Response {
public:
    ChatUserListResponse(const models::ClientsById &clientsById, Recipients recipients);

    ChatUserListResponse(models::ClientsById &&clientsById, Recipients recipients);

    std::ostringstream serialize() const override;

    const models::ClientsById &getClientsById() const {
        return clientsByIdRef;
    }

private:
    models::ClientsById clientsById;
    const models::ClientsById &clientsByIdRef;
};

} // namespace comm::trx::res
