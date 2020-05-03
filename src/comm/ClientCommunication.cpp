#include "ClientCommunication.h"

#include <boost/pointer_cast.hpp>
#include <boost/property_tree/ptree.hpp>

#include <log4cpp/Category.hh>

#include "Transmission.h"
#include "comm/CommErrors.h"
#include "comm/trx/action/DisconnectClient.h"
#include "comm/trx/data/AllClientsData.h"
#include "comm/trx/data/ClientToBeDisconnected.h"
#include "comm/trx/req/InvalidHeaderRequest.h"
#include "comm/trx/req/InvalidXmlRequest.hpp"
#include "trx/res/GenericGreeting.h"
#include "utils/Logging.hpp"

namespace comm {

using utils::log;

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server.connection.clientCommunication"));

ClientCommunication::ClientCommunication(std::shared_ptr<const ClientsSockets> clientsSockets,
                                         std::shared_ptr<const models::ClientsIdSet> clientsStatusNotification,
                                         std::shared_ptr<const models::ClientUsernameToId> clientUsernameToIdMapping,
                                         std::shared_ptr<models::ClientData> clientData,
                                         std::shared_ptr<const trx::TransactionBuilder> transactionBuilder)
    : Communication(clientsSockets, clientUsernameToIdMapping), clientData(clientData),
      clientsStatusNotification(clientsStatusNotification), transactionBuilder(transactionBuilder) {
}

void ClientCommunication::sendGreeting(const spawn::yield_context &yield) const {
    std::unique_ptr<comm::trx::res::Response> greetingResponse = std::make_unique<trx::res::GenericGreeting>(clientData->getId());
    sendResponse(greetingResponse, yield);
}

std::unique_ptr<trx::Transaction> ClientCommunication::startTransaction(const spawn::yield_context &yield) const {
    auto request = createRequest(yield);
    return transactionBuilder->build(std::move(request));
}

std::vector<int> ClientCommunication::getRecipients(std::unique_ptr<trx::res::Response> &response) const {
    // make sure to copy recipients as they can change during context switching
    std::vector<int> recipients;

    switch (response->getRecipients()) {
    case trx::res::Recipients::ONLY_SENDER:
        recipients.push_back(clientData->getId());
        break;
    case trx::res::Recipients::STATUS_NOTIFICATION_SUBSCRIBERS:
        recipients.reserve(clientsStatusNotification->size());
        std::copy_if(clientsStatusNotification->cbegin(), clientsStatusNotification->cend(), std::back_inserter(recipients),
                     [this](int recipientId) {
                         return clientData->getId() != recipientId || clientData->getChatStatus() != models::LOGOUT;
                     });
        break;
    case trx::res::Recipients::CUSTOM_CLIENTS_BY_ID:
    case trx::res::Recipients::CUSTOM_CLIENTS_BY_NAME:
    default:
        recipients = Communication::getRecipients(response);
    }

    return recipients;
}

void ClientCommunication::finalizeTransaction(std::unique_ptr<trx::Transaction> &transaction,
                                              const spawn::yield_context &yield) const {
    invokeActionBeforeSendingResponses(transaction, yield);
    sendResponse(transaction->response, yield);
    invokeActionAfterSendingResponses(transaction, yield);
}

void ClientCommunication::invokeAction(const std::unique_ptr<trx::Transaction> &transaction,
                                       const spawn::yield_context &yield) const {
    if (auto action = boost::dynamic_pointer_cast<trx::action::DisconnectClient>(transaction->customAction.get()); action) {
        auto removeClient = boost::dynamic_pointer_cast<trx::data::ClientToBeDisconnected>(transaction->customData.get());
        if (removeClient) {
            int removeClientId = removeClient->getClientId();
            LOG4CPP_INFO_S(logger) << log(clientData) << "Invoking DisconnectClient action with client id: " << removeClientId;
            action->disconnect(removeClientId);
        }
    }
}

void ClientCommunication::invokeActionBeforeSendingResponses(const std::unique_ptr<trx::Transaction> &transaction,
                                                             const spawn::yield_context &yield) const {
    if (transaction->customAction && trx::InvokingTime::BeforeSendingResponses == transaction->customAction->invokingTime)
        invokeAction(transaction, yield);
}

void ClientCommunication::invokeActionAfterSendingResponses(const std::unique_ptr<trx::Transaction> &transaction,
                                                            const spawn::yield_context &yield) const {
    if (transaction->customAction && trx::InvokingTime::AfterSendingResponses == transaction->customAction->invokingTime)
        invokeAction(transaction, yield);
}

std::unique_ptr<trx::req::Request> ClientCommunication::createRequest(const spawn::yield_context &yield) const {
    try {
        auto message = read(*clientsSockets->at(clientData->getId()), yield);
        return trx::req::Request::createRequest(message);
    } catch (boost::property_tree::ptree_error &e) {
        LOG4CPP_INFO_S(logger) << e.what();
        return std::make_unique<trx::req::InvalidXmlRequest>(
            trx::InvalidRequestDescription{std::string{"Message Received..."}, trx::InvalidResponseType::Error, 1});
    } catch (ParsingRequestDataError &e) {
        return std::make_unique<trx::req::InvalidXmlRequest>(
            trx::InvalidRequestDescription{std::string{"Message Received..."}, trx::InvalidResponseType::NotError, 0});
    } catch (CorruptedHeaderXmlTagError &e) {
        return std::make_unique<trx::req::InvalidHeaderRequest>();
    }
}

void ClientCommunication::logCommunicationError(comm::CommError &e, int clientId) const {
    LOG4CPP_ERROR_S(logger) << utils::log(clientData) << "Error while sending response to client with id = " << clientId << " "
                            << utils::log(e);
}

void ClientCommunication::logDisconnectedClientError(int clientId) const {
    LOG4CPP_ERROR_S(logger) << utils::log(clientData) << "Could not send response to client with id = " << clientId
                            << ". Client got most likely disconnected while we were sending response";
}

ClientCommunication::~ClientCommunication() {
    LOG4CPP_INFO_S(logger) << utils::log(clientData) << "Communication is closing";
    std::const_pointer_cast<ClientsSockets>(clientsSockets)->erase(clientData->getId());
}

} // namespace comm
