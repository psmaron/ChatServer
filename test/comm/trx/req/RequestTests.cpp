#include "comm/trx/req/Request.h"

#include <stdexcept>

#include <boost/property_tree/ptree.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "comm/CommErrors.h"
#include "comm/trx/req/ChatTo.h"
#include "comm/trx/req/ChatUserListRequest.h"
#include "comm/trx/req/ChatUserName.h"
#include "comm/trx/req/InvalidXmlRequest.hpp"
#include "comm/trx/req/StatusNotificationRequest.h"
#include "utils/Types.hpp"

using namespace comm::trx::req;
using testing::IsNull;
using testing::WhenDynamicCastTo;

template <typename T> class RequestTests : public ::testing::Test {};

TYPED_TEST_CASE_P(RequestTests);

TYPED_TEST_P(RequestTests, givenCorrectTypeParamReq_whenCreatingReq_shouldCreateTypeParamReqInstance) {
    const auto xmlRequest = SpecificXmlRequestGenerator<TypeParam>().build();

    EXPECT_TRUE(utils::isInstanceOf<TypeParam>(Request::createRequest(xmlRequest)));
}

REGISTER_TYPED_TEST_CASE_P(RequestTests, givenCorrectTypeParamReq_whenCreatingReq_shouldCreateTypeParamReqInstance);
typedef ::testing::Types<ChatTo, ChatUserListRequest, ChatUserName, StatusNotificationRequest> ReqTypes;
INSTANTIATE_TYPED_TEST_CASE_P(_, RequestTests, ReqTypes);

TEST(RequestTests, givenIncorrectRootTag_whenCreatingReq_shouldThrow) {
    const auto xmlRequest = XmlRequestGenerator("InvalidRootTag").build();

    EXPECT_THROW(Request::createRequest(xmlRequest), comm::ParsingRequestDataError);
}

TEST(RequestTests, creatingReqThrowingPtreeException_whenCreatingReq_shouldThrowParsingRequestDataError) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatTo>().removeField("Message").build(); // Message is required by ChatTo
    ASSERT_ANY_THROW(ChatTo chatToReq(xmlRequest));

    EXPECT_THROW(Request::createRequest(xmlRequest), comm::ParsingRequestDataError);
}

TEST(RequestTests, creatingReqThrowingStdException_whenCreatingReq_shouldThrowParsingRequestDataError) {
    const auto xmlRequest =
        SpecificXmlRequestGenerator<ChatUserName>().addField("Status", "invalidType").build(); // fail cast status
    ASSERT_ANY_THROW(ChatTo chatToReq(xmlRequest));

    EXPECT_THROW(Request::createRequest(xmlRequest), comm::ParsingRequestDataError);
}

TEST(RequestTests, givenEmptyDocumentAsInput_whenCreatingReq_shouldCreateInvalidRequest) {
    boost::property_tree::ptree xmlRequest;
    auto request = Request::createRequest(xmlRequest);

    ASSERT_THAT(request.get(), testing::WhenDynamicCastTo<InvalidXmlRequest *>(Not(IsNull())));
}