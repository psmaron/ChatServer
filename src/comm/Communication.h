#pragma once

#include <memory>
#include <vector>

#include <spawn/spawn.hpp>

#include "comm/CommErrors.h"
#include "models/ClientFwd.h"
#include "models/DataProvider.h"
#include "trx/res/Response.hpp"

namespace comm {

class Communication {
public:
    Communication(std::shared_ptr<const ClientsSockets> clientsSockets,
                  std::shared_ptr<const models::ClientUsernameToId> clientUsernameToIdMapping);

    virtual ~Communication() = default;

    void sendResponse(std::unique_ptr<trx::res::Response> &response, const spawn::yield_context &yield) const;

protected:
    virtual std::vector<int> getRecipients(std::unique_ptr<trx::res::Response> &response) const;

    void sendResponseTo(const std::vector<int> &recipients, const std::ostringstream &responseXml,
                        const spawn::yield_context &yield) const;

    virtual void logCommunicationError(comm::CommError &e, int clientId) const;
    virtual void logDisconnectedClientError(int clientId) const;

    std::shared_ptr<const ClientsSockets> clientsSockets;
    std::shared_ptr<const models::ClientUsernameToId> clientUsernameToIdMapping;
};

} // namespace comm
