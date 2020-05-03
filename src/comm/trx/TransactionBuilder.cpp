#include "TransactionBuilder.h"

#include <utility>

#include "action/DisconnectClientPimpl.h"
#include "comm/trx/req/InvalidHeaderRequest.h"
#include "core/CoreErrors.h"
#include "data/AllClientsData.h"
#include "data/AllClientsDataPimpl.hpp"
#include "req/ChatUserListRequest.h"
#include "req/ChatUserName.h"
#include "utils/Types.hpp"

namespace comm::trx {

TransactionBuilder::TransactionBuilder(std::weak_ptr<models::DataProvider> dataProvider) : dataProvider(std::move(dataProvider)) {
}

std::unique_ptr<Transaction> TransactionBuilder::build(std::unique_ptr<req::Request> request) const {
    auto transaction = std::make_unique<Transaction>();
    transaction->request = std::move(request);

    std::shared_ptr<models::DataProvider> dataProvider = this->dataProvider.lock();
    if (not dataProvider)
        throw core::DropConnection();

    if (utils::isRequestTypeOf<req::InvalidHeaderRequest>(transaction) ||
        utils::isRequestTypeOf<req::ChatUserName>(transaction)) {
        auto clientsSocketsRef = std::make_unique<action::DisconnectClient::Impl>(dataProvider->getClientsSockets());
        transaction->customAction = std::make_unique<action::DisconnectClient>(std::move(clientsSocketsRef));
        transaction->customAction->invokingTime = (utils::isRequestTypeOf<req::InvalidHeaderRequest>(transaction))
                                                      ? trx::InvokingTime::AfterSendingResponses
                                                      : trx::InvokingTime::BeforeSendingResponses;
    } else if (utils::isRequestTypeOf<req::ChatUserListRequest>(transaction)) {
        auto allClientsData = std::make_unique<data::AllClientsDataImpl>(dataProvider->getClients());
        transaction->customData = std::make_unique<data::AllClientsData>(std::move(allClientsData));
    }
    return transaction;
}

} // namespace comm::trx
