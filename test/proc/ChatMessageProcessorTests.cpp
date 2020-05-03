#include "proc/ChatMessageProcessor.h"

#include <boost/pointer_cast.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "comm/trx/Transaction.h"
#include "comm/trx/action/DisconnectClient.h"
#include "comm/trx/data/AllClientsData.h"
#include "comm/trx/req/ChatRequest.h"
#include "comm/trx/req/ChatTo.h"
#include "comm/trx/req/ChatUserListRequest.h"
#include "comm/trx/req/ChatUserName.h"
#include "comm/trx/req/StatusNotificationRequest.h"
#include "comm/trx/res/ChatFrom.h"
#include "comm/trx/res/ChatUserListResponse.h"
#include "models/ClientData.h"
#include "models/Clients.h"
#include "proc/RequestProcessor.h"

#include "helpers/generator/TransactionGenerator.h"
#include "helpers/generator/XmlRequestGenerator.h"
#include "testDoubles/models/FakeClientData.h"

using namespace models;
using namespace comm::trx::req;
using namespace comm::trx::res;
using namespace proc::trx;
using comm::trx::data::AllClientsData;
using testing::IsNull;
using testing::WhenDynamicCastTo;

namespace comm::trx::data {
// Shadowing AllClientsDataImpl for test purpose
class AllClientsDataImpl {
    using FakeClientsByIds = std::unordered_map<int, std::shared_ptr<FakeClientData>>;
    const FakeClientsByIds &getData() {
        return returnData;
    }

private:
    const FakeClientsByIds returnData = {};
};
} // namespace comm::trx::data

class AllClientDataMock : public AllClientsData {
public:
    AllClientDataMock() : AllClientsData(std::make_unique<comm::trx::data::AllClientsDataImpl>()) {
    }
    MOCK_METHOD0(getData, const models::ClientsById &());
};

class ChatMessageProcessorTest : public testing::Test {
public:
    ChatMessageProcessorTest()
        : clientData(std::make_shared<FakeClientData>(userId, models::INVALID, std::string("userName"), true)) {
    }

protected:
    int userId = 1;
    int notExistingUserId = 2;
    std::shared_ptr<models::FakeClientData> clientData;
};

TEST_F(ChatMessageProcessorTest, givenChatTo_whenProcess_shouldSetResponseToChatFrom) {
    auto transaction = std::make_unique<comm::trx::Transaction>();
    const auto msg = std::string{"msg"};
    const auto recipients = std::vector<int>{2, 5};
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatTo>(msg, recipients).build();
    transaction->request = std::make_unique<ChatTo>(xmlRequest);
    auto ChatMsgProc = RequestProcessor::create(transaction, clientData);
    ASSERT_THAT(ChatMsgProc.get(), testing::WhenDynamicCastTo<ChatMessageProcessor *>(Not(IsNull())));

    ChatMsgProc->process();

    const auto &response = transaction->response;
    ASSERT_THAT(response.get(), testing::WhenDynamicCastTo<ChatFrom *>(Not(IsNull())));
    auto rspChatFrom = boost::dynamic_pointer_cast<ChatFrom>(std::move(transaction->response));
    EXPECT_EQ(rspChatFrom->getRecipientsList().size(), 2);
    for (auto i : recipients) {
        EXPECT_THAT(rspChatFrom->getRecipientsList(), testing::Contains(i));
    }
}

TEST_F(ChatMessageProcessorTest, givenChatUserListRequestWithNoCustomData_whenProcess_shouldNotSetResponse) {
    auto transaction = generateTransaction<ChatUserListRequest>();
    EXPECT_EQ(transaction->customData, nullptr);
    auto ChatMsgProc = RequestProcessor::create(transaction, clientData);
    ASSERT_THAT(ChatMsgProc.get(), testing::WhenDynamicCastTo<ChatMessageProcessor *>(Not(IsNull())));

    ChatMsgProc->process();

    EXPECT_EQ(transaction->response, nullptr);
    EXPECT_EQ(transaction->customData, nullptr);
}

TEST_F(ChatMessageProcessorTest, givenChatUserListRequest_whenProcess_shouldSetResponseToChatUserListResponsCorectlly) {
    std::unordered_map<int, std::shared_ptr<models::ClientData>> retVal{};
    auto transaction = generateTransaction<ChatUserListRequest>();
    transaction->customData = std::make_unique<AllClientDataMock>();
    AllClientDataMock &allClientDataRef = *dynamic_cast<AllClientDataMock *>(transaction->customData.get());
    EXPECT_NE(transaction->customData, nullptr);
    auto ChatMsgProc = RequestProcessor::create(transaction, clientData);
    ASSERT_THAT(ChatMsgProc.get(), WhenDynamicCastTo<ChatMessageProcessor *>(Not(IsNull())));
    ON_CALL(allClientDataRef, getData()).WillByDefault(::testing::Invoke([&retVal = std::as_const(retVal)]() -> decltype(auto) {
        return retVal;
    }));

    ChatMsgProc->process();

    EXPECT_NE(transaction->response, nullptr);
    EXPECT_EQ(transaction->customData, nullptr);
    ASSERT_THAT(transaction->response.get(), WhenDynamicCastTo<ChatUserListResponse *>(Not(IsNull())));
    auto response = boost::dynamic_pointer_cast<ChatUserListResponse>(std::move(transaction->response));
    ASSERT_NE(response, nullptr);
    EXPECT_EQ(response->getClientsById().size(), 0);
    EXPECT_EQ(response->getRecipients(), Recipients::ONLY_SENDER);
}

TEST_F(ChatMessageProcessorTest, givenChatUserNameWithBadClientId_whenProcess_shouldThrowInvalidClientIdError) {
    ASSERT_NE(userId, notExistingUserId);
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().addField("UserID", notExistingUserId).build();
    auto transaction = std::make_unique<comm::trx::Transaction>();
    transaction->request = std::make_unique<ChatUserName>(xmlRequest);

    auto ChatMsgProc = RequestProcessor::create(transaction, clientData);
    ASSERT_THAT(ChatMsgProc.get(), WhenDynamicCastTo<ChatMessageProcessor *>(Not(IsNull())));
    ASSERT_THROW(ChatMsgProc->process(), InvalidClientIdError);

    EXPECT_EQ(transaction->response, nullptr);
    EXPECT_EQ(transaction->customData, nullptr);
}

TEST_F(ChatMessageProcessorTest, givenChatUserName_whenProcess_shouldSetResponseToChatUserListResponsCorectlly) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().addField("UserID", userId).build();
    auto transaction = std::make_unique<comm::trx::Transaction>();
    transaction->request = std::make_unique<ChatUserName>(xmlRequest);
    auto ChatMsgProc = RequestProcessor::create(transaction, clientData);
    ASSERT_THAT(ChatMsgProc.get(), WhenDynamicCastTo<ChatMessageProcessor *>(Not(IsNull())));

    ChatMsgProc->process();

    EXPECT_NE(transaction->response, nullptr);
    ASSERT_THAT(transaction->response.get(), WhenDynamicCastTo<ChatUserListResponse *>(Not(IsNull())));
    auto chatUserListRsp = boost::dynamic_pointer_cast<ChatUserListResponse>(std::move(transaction->response));
    EXPECT_EQ(chatUserListRsp->getRecipients(), Recipients::STATUS_NOTIFICATION_SUBSCRIBERS);
}

TEST_F(ChatMessageProcessorTest, givenChatUserName_whenProcess_shouldUpdateUser) {
    const auto xmlRequest =
        SpecificXmlRequestGenerator<ChatUserName>().addField("UserID", userId).addField("UserName", "username").build();
    auto transaction = std::make_unique<comm::trx::Transaction>();
    transaction->request = std::make_unique<ChatUserName>(xmlRequest);
    auto [_, chatUserName, chatStatus, isOtherSystemUser] = *dynamic_cast<ChatUserName *>(transaction->request.get());
    EXPECT_CALL(*clientData, updateData(chatUserName, chatStatus, isOtherSystemUser)).Times(1);

    auto ChatMsgProc = RequestProcessor::create(transaction, clientData);
    ASSERT_THAT(ChatMsgProc.get(), WhenDynamicCastTo<ChatMessageProcessor *>(Not(IsNull())));
    ChatMsgProc->process();
}

TEST_F(ChatMessageProcessorTest,
       givenStatusNotificationRequest_whenProcess_shouldUpdateClientWantsNotificationStatusAndThrowAbortTransaction) {
    const auto xmlRequest = SpecificXmlRequestGenerator<StatusNotificationRequest>("true").build();
    auto transaction = std::make_unique<comm::trx::Transaction>();
    transaction->request = std::make_unique<StatusNotificationRequest>(xmlRequest);
    auto [notifyOn] = *dynamic_cast<StatusNotificationRequest *>(transaction->request.get());
    EXPECT_CALL(*clientData, setWantsStatusChangeNotifications(notifyOn)).Times(1);

    auto ChatMsgProc = RequestProcessor::create(transaction, clientData);
    ASSERT_THROW(ChatMsgProc->process(), core::AbortTransaction);
}