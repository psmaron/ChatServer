#include "comm/trx/req/ChatUserName.h"

#include <stdexcept>

#include <boost/property_tree/ptree.hpp>

#include <comm/CommErrors.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "helpers/generator/XmlRequestGenerator.h"

namespace pt = boost::property_tree;
using namespace comm::trx::req;

class ChatUserNameTests : public testing::Test {
protected:
    const int userId = 1;
    std::string invalidTypeUserId = "stringType";
    const long long overflowTypeUserId = static_cast<long long>(std::numeric_limits<int>::max()) + 1;
    const std::string userName = "username1";
    const std::string otherSystemUser = "TRUE";
    const std::string otherSystemUserCaseMixed = "TrUe";
    const std::string notOtherSystemUser = "FALSE";
    const std::string otherSystemUserPrefixedTRUE = "TRUEsomething";
    const std::string notOtherSystemUserInvalidValue = "invalid";
    const models::ChatStatus chatStatus = models::ChatStatus::LOGOUT;
    std::string invalidTypeChatStatus = "stringType";
    const long long overflowTypeChatStatus = static_cast<long long>(std::numeric_limits<int>::max()) + 1;
};

TEST_F(ChatUserNameTests, givenWrongRootTagName_whenCreatingChatUserNameReq_shouldThrow) {
    const auto xmlRequest = XmlRequestGenerator("WrongRootTagName").build();

    ASSERT_THROW(ChatUserName req(xmlRequest), pt::ptree_bad_path);
}

TEST_F(ChatUserNameTests, givenReqWithDuplicatedUserIdField_whenCreatingChatUserNameReq_shouldThrow) {
    auto requestGenerator = SpecificXmlRequestGenerator<ChatUserName>().addField("UserID", userId).addField("UserID", userId);
    ASSERT_GT(requestGenerator.countField("UserID"), 1);
    const auto xmlRequest = requestGenerator.build();

    EXPECT_THROW(ChatUserName chatUserNameReq(xmlRequest), comm::ParsingRequestDataError);
}

TEST_F(ChatUserNameTests, givenReqWithMissingUserIdField_whenCreatingChatUserNameReq_expectUserIdToBeMinus1) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().build();

    ChatUserName chatUserNameReq(xmlRequest);

    const auto [parsedUserId, _1, _2, _3] = chatUserNameReq;
    EXPECT_EQ(-1, parsedUserId);
}

TEST_F(ChatUserNameTests, givenReqWithInvalidTypeUserIdField_whenCreatingChatUserNameReq_shouldThrow) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().addField("UserID", invalidTypeUserId).build();

    EXPECT_THROW(ChatUserName chatUserNameReq(xmlRequest), std::invalid_argument);
}

TEST_F(ChatUserNameTests, givenReqWithOverflowTypeUserIdField_whenCreatingChatUserNameReq_shouldThrow) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().addField("UserID", overflowTypeUserId).build();

    EXPECT_THROW(ChatUserName chatUserNameReq(xmlRequest), std::out_of_range);
}

TEST_F(ChatUserNameTests, givenReqWithDuplicatedUserNameField_whenCreatingChatUserNameReq_shouldThrow) {
    auto requestGenerator =
        SpecificXmlRequestGenerator<ChatUserName>().addField("UserName", userName).addField("UserName", userName);
    ASSERT_GT(requestGenerator.countField("UserName"), 1);
    const auto xmlRequest = requestGenerator.build();

    EXPECT_THROW(ChatUserName chatUserNameReq(xmlRequest), comm::ParsingRequestDataError);
}

TEST_F(ChatUserNameTests, givenReqWithMissingUserNameField_whenCreatingChatUserNameReq_shouldNotThrow) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().build();

    ChatUserName chatUserNameReq(xmlRequest);

    EXPECT_NO_THROW(ChatUserName chatUserNameReq(xmlRequest));
}

TEST_F(ChatUserNameTests, givenReqWithDuplicatedStatusField_whenCreatingChatUserNameReq_shouldThrow) {
    auto requestGenerator =
        SpecificXmlRequestGenerator<ChatUserName>().addField("Status", chatStatus).addField("Status", chatStatus);
    ASSERT_GT(requestGenerator.countField("Status"), 1);
    const auto xmlRequest = requestGenerator.build();

    EXPECT_THROW(ChatUserName chatUserNameReq(xmlRequest), comm::ParsingRequestDataError);
}

TEST_F(ChatUserNameTests, givenReqWithMissingChatStatusField_whenCreatingChatUserNameReq_expectStatusToBeLogin) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().build();

    ChatUserName chatUserNameReq(xmlRequest);

    const auto [_1, _2, parsedChatStatus, _3] = chatUserNameReq;
    EXPECT_EQ(parsedChatStatus, models::ChatStatus::LOGIN);
}

TEST_F(ChatUserNameTests, givenReqWithInvalidTypeChatStatusField_whenCreatingChatUserNameReq_shouldThrow) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().addField("Status", invalidTypeChatStatus).build();

    EXPECT_THROW(ChatUserName chatUserNameReq(xmlRequest), std::invalid_argument);
}

TEST_F(ChatUserNameTests, givenReqWithOverflowTypeChatStatusField_whenCreatingChatUserNameReq_shouldThrow) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().addField("Status", overflowTypeChatStatus).build();

    EXPECT_THROW(ChatUserName chatUserNameReq(xmlRequest), std::out_of_range);
}

TEST_F(ChatUserNameTests, givenReqWithMissingIsOtherSystemField_whenCreatingChatUserNameReq_expectIsOtherSystemToBeFalse) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>().build();

    ChatUserName chatUserNameReq(xmlRequest);

    const auto [_1, _2, _3, parsedIsOtherSystemUser] = chatUserNameReq;
    EXPECT_FALSE(parsedIsOtherSystemUser);
}

TEST_F(ChatUserNameTests, givenReqWithisOtherSystemFieldPrefixedTRUE_whenCreatingChatUserNameReq_expectIsOtherSystemToBeTrue) {
    ASSERT_THAT(otherSystemUserPrefixedTRUE, ::testing::StartsWith("TRUE"));
    const auto xmlRequest =
        SpecificXmlRequestGenerator<ChatUserName>().addField("isOtherSystemUser", otherSystemUserPrefixedTRUE).build();

    ChatUserName chatUserNameReq(xmlRequest);

    const auto [_1, _2, _3, parsedIsOtherSystemUser] = chatUserNameReq;
    EXPECT_TRUE(parsedIsOtherSystemUser);
}

TEST_F(ChatUserNameTests, givenReqWithisOtherSystemFieldIsTrueMixedCase_whenCreatingChatUserNameReq_shouldBeCaseInsensitive) {
    ASSERT_THAT(otherSystemUserCaseMixed, ::testing::StrCaseEq("true"));
    const auto xmlRequest =
        SpecificXmlRequestGenerator<ChatUserName>().addField("isOtherSystemUser", otherSystemUserPrefixedTRUE).build();

    ChatUserName chatUserNameReq(xmlRequest);

    const auto [_1, _2, _3, parsedIsOtherSystemUser] = chatUserNameReq;
    EXPECT_TRUE(parsedIsOtherSystemUser);
}

TEST_F(ChatUserNameTests, givenReqWithInvalidIsOtherSystemValue_whenCreatingChatUserNameReq_expectIsOtherSystemToBeFalse) {
    ASSERT_NE(notOtherSystemUserInvalidValue, "TRUE");
    ASSERT_NE(notOtherSystemUserInvalidValue, "FALSE");
    const auto xmlRequest =
        SpecificXmlRequestGenerator<ChatUserName>().addField("isOtherSystemUser", notOtherSystemUserInvalidValue).build();

    ChatUserName chatUserNameReq(xmlRequest);

    const auto [_1, _2, _3, parsedIsOtherSystemUser] = chatUserNameReq;
    EXPECT_FALSE(parsedIsOtherSystemUser);
}

TEST_F(ChatUserNameTests, givenReqWithDuplicatedIsOtherSystemUserField_whenCreatingChatUserNameReq_shouldThrow) {
    ASSERT_NE(otherSystemUser, notOtherSystemUser);
    auto requestGenerator = SpecificXmlRequestGenerator<ChatUserName>()
                                .addField("isOtherSystemUser", otherSystemUser)
                                .addField("isOtherSystemUser", notOtherSystemUser);
    ASSERT_GT(requestGenerator.countField("isOtherSystemUser"), 1);
    const auto xmlRequest = requestGenerator.build();

    EXPECT_THROW(ChatUserName chatUserNameReq(xmlRequest), comm::ParsingRequestDataError);
}

TEST_F(ChatUserNameTests, givenCorrectChatUserNameReqData_whenCreatingChatUserNameReq_shouldParseCorrect) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatUserName>()
                                .addField("UserID", userId)
                                .addField("UserName", userName)
                                .addField("Status", chatStatus)
                                .addField("isOtherSystemUser", otherSystemUser)
                                .build();

    ChatUserName chatUserNameReq(xmlRequest);

    const auto [parsedUserId, parsedUserName, parsedStatus, parsedIsOtherSystemUser] = chatUserNameReq;
    EXPECT_EQ(parsedUserId, userId);
    EXPECT_EQ(parsedUserName, userName);
    EXPECT_EQ(parsedStatus, chatStatus);
    EXPECT_TRUE(parsedIsOtherSystemUser);
}