#pragma once

#include <memory>
#include <type_traits>

#include "ProcErrors.h"
#include "comm/trx/Transaction.h"
#include "models/ClientData.h"

namespace proc::trx {

struct RequestProcessorError : std::exception {};

class RequestProcessor {
public:
    static std::unique_ptr<RequestProcessor> create(std::unique_ptr<comm::trx::Transaction> &transaction,
                                                    std::shared_ptr<models::ClientData> clientData);

    virtual ~RequestProcessor() = default;

    virtual void process() = 0;

    explicit RequestProcessor(std::unique_ptr<comm::trx::Transaction> &transaction,
                              std::shared_ptr<models::ClientData> clientData);

protected:
    std::shared_ptr<models::ClientData> clientData;
    std::unique_ptr<comm::trx::Transaction> &transaction;
};

} // namespace proc::trx
