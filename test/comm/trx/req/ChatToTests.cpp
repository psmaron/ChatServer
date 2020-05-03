#include "comm/trx/req/ChatTo.h"

#include <boost/property_tree/exceptions.hpp>

#include <gtest/gtest.h>

#include "comm/CommErrors.h"
#include "helpers/generator/XmlRequestGenerator.h"

namespace pt = boost::property_tree;
using namespace comm::trx::req;

class ChatToTests : public testing::Test {
protected:
    const std::string message = "msg";
    const std::vector<int> recipients{2, 5};
    const std::vector<int> duplicatedRecipients{2, 5, 2, 6, 5};
    const std::vector<int> emptyRecipients;
    const std::string invalidTypeRecipient = "stringType";
};

TEST_F(ChatToTests, givenWrongRootTagName_whenCreatingChatTo_shouldThrow) {
    const auto xmlRequest = XmlRequestGenerator("WrongRootTagName").build();

    ASSERT_THROW(ChatTo req(xmlRequest), pt::ptree_bad_path);
}

TEST_F(ChatToTests, givenDuplicatedRecipients_whenCreatingChatToRequest_shouldOnlyInsertUniqueRecipients) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatTo>(message, duplicatedRecipients).build();

    ChatTo chatToReq(xmlRequest);

    const auto [_, parsedRecipients] = chatToReq;
    EXPECT_EQ(std::unordered_set<int>(duplicatedRecipients.begin(), duplicatedRecipients.end()), parsedRecipients);
}

TEST_F(ChatToTests, givenReqWithMissingMessageField_whenCreatingChatTo_shouldThrow) {
    auto requestGenerator = SpecificXmlRequestGenerator<ChatTo>(message, recipients).removeField("Message");
    ASSERT_EQ(requestGenerator.countField("Message"), 0);
    const auto xmlRequest = requestGenerator.build();

    ASSERT_THROW(ChatTo req(xmlRequest), comm::ParsingRequestDataError);
}

TEST_F(ChatToTests, givenReqWithMissingRecipientsField_whenCreatingChatTo_shouldParseCorrectly) {
    SpecificXmlRequestGenerator<ChatTo> requestGenerator(message, emptyRecipients);
    ASSERT_EQ(requestGenerator.countField("Recipients"), 0);
    const auto xmlRequest = requestGenerator.build();

    ChatTo chatToReq(xmlRequest);

    const auto [parsedMessage, parsedRecipients] = chatToReq;
    EXPECT_EQ(message, parsedMessage);
    EXPECT_TRUE(parsedRecipients.empty());
}

TEST_F(ChatToTests, givenReqWithIncorrectTypeRecipientField_whenCreatingChatTo_shouldThrow) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatTo>(message).addField("Recipients", invalidTypeRecipient).build();

    EXPECT_THROW(ChatTo chatToReq(xmlRequest), pt::ptree_bad_data);
}

TEST_F(ChatToTests, givenReqWithDulicatedMessageField_whenCreatingChatTo_shouldThrow) {
    auto requestGenerator = SpecificXmlRequestGenerator<ChatTo>(message, recipients).addField("Message", message);
    ASSERT_GT(requestGenerator.countField("Message"), 1);
    const auto xmlRequest = requestGenerator.build();

    EXPECT_THROW(ChatTo chatToReq(xmlRequest), comm::ParsingRequestDataError);
}

TEST_F(ChatToTests, givenCorrectChatToReqData_whenCreatingChatTo_shouldParseCorrect) {
    const auto xmlRequest = SpecificXmlRequestGenerator<ChatTo>(message, recipients).build();

    ChatTo chatToReq(xmlRequest);

    const auto [parsedMessage, parsedRecipients] = chatToReq;
    EXPECT_EQ(message, parsedMessage);
    EXPECT_EQ(std::unordered_set<int>(recipients.begin(), recipients.end()), parsedRecipients);
}
