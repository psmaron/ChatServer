#include "proc/RequestProcessor.h"

#include <boost/pointer_cast.hpp>

#include <gmock/gmock-matchers.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "comm/trx/req/ChatTo.h"
#include "comm/trx/req/ChatUserListRequest.h"
#include "comm/trx/req/ChatUserName.h"
#include "models/ClientData.h"
#include "proc/ChatMessageProcessor.h"

#include "utils/Types.hpp"

using namespace comm::trx;
using namespace proc::trx;
using testing::IsNull;
using testing::WhenDynamicCastTo;

template <typename T> class ChatMessageProcessorTests : public ::testing::Test {};

using chatMsgProcessorTestTypes = ::testing::Types<req::ChatUserName, req::ChatTo, req::ChatUserListRequest>;

TYPED_TEST_CASE(ChatMessageProcessorTests, chatMsgProcessorTestTypes);

TYPED_TEST(ChatMessageProcessorTests, givenChatProcessorType_whenCreatingRequestProcessor_sholdReturnChatMessageProcessor) {
    std::shared_ptr<models::ClientData> clientData;
    auto transaction = generateTransaction<TypeParam>();

    auto chatMsgProc = RequestProcessor::create(transaction, clientData);

    ASSERT_THAT(chatMsgProc.get(), testing::WhenDynamicCastTo<ChatMessageProcessor *>(Not(IsNull())));
}
