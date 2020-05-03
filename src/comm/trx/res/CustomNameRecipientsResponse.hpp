#pragma once

#include "comm/trx/res/Response.hpp"
#include "models/ClientFwd.h"

namespace comm::trx::res {

class CustomNameRecipientsResponse : public Response {
public:
    CustomNameRecipientsResponse(models::ClientsUsernameSet recipientsList)
        : Response(Recipients::CUSTOM_CLIENTS_BY_NAME), recipientsList(std::move(recipientsList)) {
    }

    const models::ClientsUsernameSet &getRecipientsList() const {
        return recipientsList;
    }

    ~CustomNameRecipientsResponse() override = 0;

protected:
    models::ClientsUsernameSet recipientsList;
};

inline CustomNameRecipientsResponse::~CustomNameRecipientsResponse() = default;

} // namespace comm::trx::res