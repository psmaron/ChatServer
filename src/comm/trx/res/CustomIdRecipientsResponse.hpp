#pragma once

#include "comm/trx/res/Response.hpp"
#include "models/ClientFwd.h"

namespace comm::trx::res {

class CustomIdRecipientsResponse : public Response {
public:
    CustomIdRecipientsResponse(models::ClientsIdSet recipientsList)
        : Response(Recipients::CUSTOM_CLIENTS_BY_ID), recipientsList(std::move(recipientsList)) {
    }

    const models::ClientsIdSet &getRecipientsList() const {
        return recipientsList;
    }

    ~CustomIdRecipientsResponse() override = 0;

protected:
    models::ClientsIdSet recipientsList;
};

inline CustomIdRecipientsResponse::~CustomIdRecipientsResponse() = default;

} // namespace comm::trx::res