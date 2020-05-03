#include "comm/trx/TransactionBuilder.h"

#include <boost/pointer_cast.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "comm/trx/action/DisconnectClient.h"
#include "comm/trx/data/AllClientsData.h"
#include "comm/trx/req/ChatUserListRequest.h"
#include "comm/trx/req/InvalidHeaderRequest.h"

#include "testDoubles/models/FakeDataProvider.hpp"

using namespace comm::trx;
using testing::IsNull;
using testing::WhenDynamicCastTo;

class TransactionBuilderTest : public ::testing::Test {
public:
    TransactionBuilderTest() : transactionBuilder(dataProvider) {
    }

protected:
    std::shared_ptr<models::FakeDataProvider> dataProvider = std::make_shared<models::FakeDataProvider>();
    comm::trx::TransactionBuilder transactionBuilder;
};

TEST_F(TransactionBuilderTest, givenInvalidStructureRequest_whenBuildTransaction_shouldSetCustomActionToDisconnectClient) {
    auto request = std::make_unique<req::InvalidHeaderRequest>();

    auto transaction = transactionBuilder.build(std::move(request));

    ASSERT_NE(transaction, nullptr);
    ASSERT_THAT(transaction->customAction.get(), WhenDynamicCastTo<action::DisconnectClient *>(Not(IsNull())));
    auto customAction = boost::dynamic_pointer_cast<action::DisconnectClient>(std::move(transaction->customAction));
    ASSERT_EQ(customAction->invokingTime, comm::trx::InvokingTime::AfterSendingResponses);
}