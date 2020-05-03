#pragma once

#include <memory>
#include <vector>

#include <boost/asio/ip/tcp.hpp>

#include <spawn/spawn.hpp>

#include "Communication.h"
#include "comm/CommErrors.h"
#include "models/ClientFwd.h"
#include "trx/Transaction.h"
#include "trx/TransactionBuilder.h"
#include "trx/res/Response.hpp"

namespace comm {

class ClientCommunication : protected Communication {
public:
    ClientCommunication(std::shared_ptr<const ClientsSockets> clientsSockets,
                        std::shared_ptr<const models::ClientsIdSet> clientsStatusNotification,
                        std::shared_ptr<const models::ClientUsernameToId> clientUsernameToIdMapping,
                        std::shared_ptr<models::ClientData> clientData,
                        std::shared_ptr<const trx::TransactionBuilder> transactionBuilder);

    ~ClientCommunication() override;

    void sendGreeting(const spawn::yield_context &yield) const;

    std::unique_ptr<trx::Transaction> startTransaction(const spawn::yield_context &yield) const;

    void finalizeTransaction(std::unique_ptr<trx::Transaction> &transaction, const spawn::yield_context &yield) const;

private:
    std::vector<int> getRecipients(std::unique_ptr<trx::res::Response> &response) const override;

    void invokeActionBeforeSendingResponses(const std::unique_ptr<trx::Transaction> &transaction,
                                            const spawn::yield_context &yield) const;
    void invokeActionAfterSendingResponses(const std::unique_ptr<trx::Transaction> &transaction,
                                           const spawn::yield_context &yield) const;
    void invokeAction(const std::unique_ptr<trx::Transaction> &transaction, const spawn::yield_context &yield) const;

    std::unique_ptr<trx::req::Request> createRequest(const spawn::yield_context &yield) const;

    void logCommunicationError(comm::CommError &e, int clientId) const override;
    void logDisconnectedClientError(int clientId) const override;

    std::shared_ptr<models::ClientData> clientData;
    std::shared_ptr<const models::ClientsIdSet> clientsStatusNotification;
    std::shared_ptr<const trx::TransactionBuilder> transactionBuilder;
};

} // namespace comm
