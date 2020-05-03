#pragma once

#include "CustomIdRecipientsResponse.hpp"
#include "models/ClientFwd.h"

namespace comm::trx::res {

class ChatFrom : public CustomIdRecipientsResponse {
public:
    ChatFrom(std::string message, models::ClientsIdSet recipientsList);

    std::ostringstream serialize() const override;

private:
    std::string message;
};

} // namespace comm::trx::res