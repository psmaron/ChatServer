#include "ChatMessageProcessor.h"

#include <boost/pointer_cast.hpp>

#include <log4cpp/Category.hh>

#include "ProcErrors.h"
#include "comm/trx/action/DisconnectClient.h"
#include "comm/trx/data/AllClientsData.h"
#include "comm/trx/data/ClientToBeDisconnected.h"
#include "comm/trx/req/ChatTo.h"
#include "comm/trx/req/ChatUserListRequest.h"
#include "comm/trx/req/ChatUserName.h"
#include "comm/trx/req/StatusNotificationRequest.h"
#include "comm/trx/res/ChatFrom.h"
#include "comm/trx/res/ChatUserListResponse.h"
#include "utils/Logging.hpp"
#include "utils/Types.hpp"

namespace proc::trx {

using utils::log;
using namespace comm::trx;

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server.connection.chatMessageProcessor"));

void ChatMessageProcessor::process() {
    // TODO: think of better way of dispatching requests, keywords: polymorphic/double dispatch, visitor
    // TODO: https://gieseanw.wordpress.com/2018/12/29/stop-reimplementing-the-virtual-table-and-start-using-double-dispatch/
    if (utils::isInstanceOf<req::ChatUserName>(transaction->request))
        processChatUserName();
    else if (utils::isInstanceOf<req::ChatUserListRequest>(transaction->request))
        processChatUserListRequest();
    else if (utils::isInstanceOf<req::ChatTo>(transaction->request))
        processChatTo();
    else if (utils::isInstanceOf<req::StatusNotificationRequest>(transaction->request))
        processStatusNotificationRequest();
}

void ChatMessageProcessor::processChatUserName() {
    auto [id, chatUsername, chatStatus, isOtherSystemUser] = *dynamic_cast<req::ChatUserName *>(transaction->request.get());
    if (id != clientData->getId()) {
        LOG4CPP_ERROR_S(logger) << log(clientData)
                                << "ChatUserName's client id doesn't match this connection's client id. Ignoring request";
        throw InvalidClientIdError{};
    }

    auto removedClientId = clientData->updateData(chatUsername, chatStatus, isOtherSystemUser);
    if (auto action = boost::dynamic_pointer_cast<action::DisconnectClient>(transaction->customAction.get());
        action && removedClientId.has_value()) {
        transaction->customData = std::make_unique<data::ClientToBeDisconnected>(removedClientId.value());
        LOG4CPP_INFO_S(logger) << log(clientData) << "Setting new ChatUsername caused user with id = " << removedClientId.value()
                               << " to be disconnected";
    }
    LOG4CPP_INFO_S(logger) << log(clientData) << "New data: username = " << clientData->getChatUsername()
                           << ", status = " << clientData->getChatStatus()
                           << ", otherSystemUser = " << clientData->getIsOtherSystemUser();
    transaction->response = std::make_unique<res::ChatUserListResponse>(models::ClientsById{{clientData->getId(), clientData}},
                                                                        res::Recipients::STATUS_NOTIFICATION_SUBSCRIBERS);
}

void ChatMessageProcessor::processChatUserListRequest() {
    if (auto allClients = boost::dynamic_pointer_cast<data::AllClientsData>(std::move(transaction->customData)); allClients)
        transaction->response = std::make_unique<res::ChatUserListResponse>(allClients->getData(), res::Recipients::ONLY_SENDER);
}

void ChatMessageProcessor::processChatTo() {
    auto [message, recipients] = *dynamic_cast<req::ChatTo *>(transaction->request.get());
    transaction->response = std::make_unique<res::ChatFrom>(std::move(message), std::move(recipients));
}

void ChatMessageProcessor::processStatusNotificationRequest() {
    auto [isNotificationOn] = *dynamic_cast<req::StatusNotificationRequest *>(transaction->request.get());
    clientData->setWantsStatusChangeNotifications(isNotificationOn);
    throw core::AbortTransaction();
}

} // namespace proc::trx
