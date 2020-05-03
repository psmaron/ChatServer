#include "comm/trx/res/ChatFrom.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <gtest/gtest.h>

using namespace comm::trx::res;
namespace pt = boost::property_tree;

class ChatFromTests : public testing::Test {
protected:
    const std::string msg = "msg";
    const std::unordered_set<int> recipients = {1, 4};
    const std::unordered_set<int> emptyRecipients = {};
};

TEST_F(ChatFromTests, givenValidResponseData_shouldSerializeCorrectly) {
    ChatFrom chatFrom(msg, recipients);

    std::stringstream serialized(chatFrom.serialize().str());

    pt::ptree pt;
    ASSERT_NO_THROW(pt::read_xml(serialized, pt));
    ASSERT_EQ(1, pt.count("ChatFrom"));
    const auto rootTag = pt.get_child("ChatFrom");
    EXPECT_EQ(msg, rootTag.get<std::string>("Message"));
    EXPECT_EQ(recipients.size(), rootTag.count("Recipients"));
    auto recipientsRange = rootTag.equal_range("Recipients");
    std::set<int> parsedRecipients;
    std::transform(recipientsRange.first, recipientsRange.second, std::inserter(parsedRecipients, parsedRecipients.end()),
                   [](const std::pair<std::string, pt::ptree> &it) -> int { return it.second.get_value<int>(); });
    EXPECT_EQ(std::set<int>(recipients.begin(), recipients.end()), parsedRecipients);
}

TEST_F(ChatFromTests, givenEmptyRecipientsList_whenSerialize_expectToSerializationNotContainRecipientsTag) {
    ChatFrom chatFrom(msg, emptyRecipients);
    ASSERT_TRUE(chatFrom.getRecipientsList().empty());

    std::stringstream serialized(chatFrom.serialize().str());

    pt::ptree pt;
    ASSERT_NO_THROW(pt::read_xml(serialized, pt));
    ASSERT_EQ(1, pt.count("ChatFrom"));
    const auto rootTag = pt.get_child("ChatFrom");
    EXPECT_EQ(0, rootTag.count("Recipients"));
}