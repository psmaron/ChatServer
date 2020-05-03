#include "proc/InvalidRequestProcessor.h"

#include <boost/pointer_cast.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "comm/trx/Transaction.h"
#include "comm/trx/req/InvalidHeaderRequest.h"
#include "comm/trx/req/InvalidXmlRequest.hpp"
#include "comm/trx/res/ChatUserListResponse.h"
#include "comm/trx/res/GenericResponse.h"

#include "helpers/generator/TransactionGenerator.h"
#include "testDoubles/models/FakeClientData.h"

using namespace comm::trx;
using namespace proc::trx;
using testing::IsNull;
using testing::WhenDynamicCastTo;

class InvalidRequestProcessorTest : public testing::Test {
public:
    InvalidRequestProcessorTest()
        : clientData(std::make_shared<models::FakeClientData>(1, models::INVALID, std::string("userName"), true)) {
    }

protected:
    std::shared_ptr<models::ClientData> clientData;
};

TEST_F(InvalidRequestProcessorTest, givenInvalidXmlRequest_whenProcess_shouldSetResponseToGenericResponse) {
    auto transaction = std::make_unique<Transaction>();
    transaction->request =
        std::make_unique<comm::trx::req::InvalidXmlRequest>(InvalidRequestDescription("msg", InvalidResponseType::NotError, 0));

    auto invalidReqProcessor = RequestProcessor::create(transaction, clientData);
    ASSERT_THAT(invalidReqProcessor.get(), testing::WhenDynamicCastTo<InvalidRequestProcessor *>(Not(IsNull())));

    invalidReqProcessor->process();

    ASSERT_NE(transaction->response, nullptr);
    ASSERT_THAT(transaction->response.get(), WhenDynamicCastTo<res::GenericResponse *>(Not(IsNull())));
    auto genericResponse = boost::dynamic_pointer_cast<res::GenericResponse>(std::move(transaction->response));

    EXPECT_EQ(genericResponse->getRecipients(), res::Recipients::ONLY_SENDER);
}

TEST_F(InvalidRequestProcessorTest, givenInvalidHeaderRequest_whenProcess_shouldSetResponseToChatUserList) {
    auto transaction = std::make_unique<Transaction>();
    transaction->request = std::make_unique<comm::trx::req::InvalidHeaderRequest>();

    auto invalidReqProcessor = RequestProcessor::create(transaction, clientData);
    ASSERT_THAT(invalidReqProcessor.get(), testing::WhenDynamicCastTo<InvalidRequestProcessor *>(Not(IsNull())));

    invalidReqProcessor->process();

    ASSERT_NE(transaction->response, nullptr);
    ASSERT_THAT(transaction->response.get(), WhenDynamicCastTo<res::ChatUserListResponse *>(Not(IsNull())));
    auto genericResponse = boost::dynamic_pointer_cast<res::ChatUserListResponse>(std::move(transaction->response));

    EXPECT_EQ(genericResponse->getRecipients(), res::Recipients::STATUS_NOTIFICATION_SUBSCRIBERS);
}
