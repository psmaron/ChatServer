#include "Communication.h"

#include <boost/pointer_cast.hpp>

#include <log4cpp/Category.hh>

#include "Transmission.h"
#include "comm/CommErrors.h"
#include "comm/trx/res/CustomIdRecipientsResponse.hpp"
#include "comm/trx/res/CustomNameRecipientsResponse.hpp"
#include "utils/Logging.hpp"

namespace comm {

using utils::log;

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server.connection.communication"));

Communication::Communication(std::shared_ptr<const ClientsSockets> clientsSockets,
                             std::shared_ptr<const models::ClientUsernameToId> clientUsernameToIdMapping)
    : clientsSockets(clientsSockets), clientUsernameToIdMapping(clientUsernameToIdMapping) {
}

std::vector<int> Communication::getRecipients(std::unique_ptr<trx::res::Response> &response) const {
    std::vector<int> recipients;

    switch (response->getRecipients()) {
    case trx::res::Recipients::CUSTOM_CLIENTS_BY_ID: {
        auto customIdRecipientsResponse = boost::dynamic_pointer_cast<trx::res::CustomIdRecipientsResponse>(std::move(response));
        auto responseIdRecipients = customIdRecipientsResponse->getRecipientsList();
        recipients.reserve(responseIdRecipients.size());
        std::copy(responseIdRecipients.cbegin(), responseIdRecipients.cend(), std::back_inserter(recipients));
        break;
    }
    case trx::res::Recipients::CUSTOM_CLIENTS_BY_NAME: {
        auto customNameRecipientsResponse =
            boost::dynamic_pointer_cast<trx::res::CustomNameRecipientsResponse>(std::move(response));
        auto responseNamesRecipients = customNameRecipientsResponse->getRecipientsList();
        recipients.reserve(responseNamesRecipients.size());
        for (const auto &recipientName : responseNamesRecipients)
            if (auto found = clientUsernameToIdMapping->find(recipientName); found != clientUsernameToIdMapping->end())
                recipients.push_back(found->second);
        break;
    }
    case trx::res::Recipients::ONLY_SENDER:
    case trx::res::Recipients::STATUS_NOTIFICATION_SUBSCRIBERS:
    default:
        LOG4CPP_WARN_S(logger) << "Could not select the right recipients. The response will not be sent.";
        break;
    }

    return recipients;
}

void Communication::sendResponse(std::unique_ptr<trx::res::Response> &response, const spawn::yield_context &yield) const {
    auto responseXml = response->serialize();
    std::vector<int> recipients = getRecipients(response);
    sendResponseTo(recipients, responseXml, yield);
}

void Communication::sendResponseTo(const std::vector<int> &recipients, const std::ostringstream &responseXml,
                                   const spawn::yield_context &yield) const {
    for (auto &clientId : recipients) {
        try {
            // Be careful when yielding while iterating over clients data, because they may change after context switch
            comm::send(*clientsSockets->at(clientId), responseXml, yield);
        } catch (comm::CommError &e) {
            logCommunicationError(e, clientId);
        } catch (std::out_of_range &) {
            logDisconnectedClientError(clientId);
        }
    }
}

void Communication::logCommunicationError(comm::CommError &e, int clientId) const {
    LOG4CPP_ERROR_S(logger) << "Error while sending response to client with id = " << clientId << " " << utils::log(e);
}

void Communication::logDisconnectedClientError(int clientId) const {
    LOG4CPP_ERROR_S(logger) << "Could not send response to client with id = " << clientId
                            << ". Client got most likely disconnected while we were sending response";
}

} // namespace comm
