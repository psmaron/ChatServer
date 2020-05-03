#include "RequestProcessor.h"

#include <log4cpp/Category.hh>
#include <utility>

#include "ChatMessageProcessor.h"
#include "InvalidRequestProcessor.h"
#include "comm/trx/req/ChatRequest.h"
#include "comm/trx/req/InvalidRequest.h"
#include "utils/Logging.hpp"
#include "utils/Types.hpp"

namespace proc::trx {

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server.connection.requestProcessor"));
using utils::log;

std::unique_ptr<RequestProcessor> RequestProcessor::create(std::unique_ptr<comm::trx::Transaction> &transaction,
                                                           std::shared_ptr<models::ClientData> clientData) {

    if (utils::isInstanceOf<comm::trx::req::ChatRequest>(transaction->request))
        return std::make_unique<ChatMessageProcessor>(transaction, clientData);
    else if (utils::isInstanceOf<comm::trx::req::InvalidRequest>(transaction->request))
        return std::make_unique<InvalidRequestProcessor>(transaction, clientData);

    LOG4CPP_ERROR_S(logger) << log(clientData)
                            << "Request not handled due to missing request processor error (probably not implemented!)";
    throw UnhandledRequestError{};
}

RequestProcessor::RequestProcessor(std::unique_ptr<comm::trx::Transaction> &transaction,
                                   std::shared_ptr<models::ClientData> clientData)
    : clientData(std::move(clientData)), transaction(transaction) {
}

} // namespace proc::trx
