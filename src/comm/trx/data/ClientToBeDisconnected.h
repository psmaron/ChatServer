#pragma once

#include "comm/trx/CustomData.h"

namespace comm::trx::data {

class ClientToBeDisconnected : public CustomData {
public:
    ClientToBeDisconnected(int clientId) : clientId(clientId) {
    }
    int getClientId() const {
        return clientId;
    }

private:
    int clientId;
};

} // namespace comm::trx::data