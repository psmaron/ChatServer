#include "comm/trx/res/ChatUserListResponse.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "models/ClientData.h"

#include "testDoubles/models/FakeClientData.h"

using namespace comm::trx::res;
using namespace models;
namespace pt = boost::property_tree;

class ChatUserListResponseTests : public testing::Test {
protected:
    std::shared_ptr<FakeClientData> client1 = std::make_shared<FakeClientData>(2, models::ChatStatus::LOGIN, "username1", true);
    std::shared_ptr<FakeClientData> client2 = std::make_shared<FakeClientData>(8, models::ChatStatus::AWAY, "username2", false);
    models::ClientsById clientsById;
    Recipients recipients = Recipients::STATUS_NOTIFICATION_SUBSCRIBERS;
};

TEST_F(ChatUserListResponseTests, givenSeveralClientsMap_whenSerialize_expectToSerializationContainsCorrectUserListTagNumber) {
    clientsById.insert({client1->getId(), client1});
    clientsById.insert({client2->getId(), client2});
    ChatUserListResponse chatUserListResponse(clientsById, recipients);

    std::stringstream serialized(chatUserListResponse.serialize().str());

    pt::ptree pt;
    ASSERT_NO_THROW(pt::read_xml(serialized, pt));
    ASSERT_EQ(1, pt.count("ChatUserListResponse"));
    const auto rootTag = pt.get_child("ChatUserListResponse");
    EXPECT_EQ(clientsById.size(), rootTag.count("UserList"));
}

TEST_F(ChatUserListResponseTests, givenEmptyClientsMap_whenSerialize_expectToSerializationNotContainUserListTags) {
    ASSERT_TRUE(clientsById.empty());
    ChatUserListResponse chatUserListResponse(clientsById, recipients);

    std::stringstream serialized(chatUserListResponse.serialize().str());

    pt::ptree pt;
    ASSERT_NO_THROW(pt::read_xml(serialized, pt));
    ASSERT_EQ(1, pt.count("ChatUserListResponse"));
    const auto rootTag = pt.get_child("ChatUserListResponse");
    EXPECT_EQ(0, rootTag.count("UserList"));
}

TEST_F(ChatUserListResponseTests, givenRecipients_whenCreatingChatUserListResponse_expectToRecipientsBeCorrect) {
    ChatUserListResponse chatUserListResponse(clientsById, recipients);

    EXPECT_EQ(recipients, chatUserListResponse.getRecipients());
}

TEST_F(ChatUserListResponseTests, givenValidClientData_shouldSerializeCorrectClientData) {
    clientsById.insert({client1->getId(), client1});
    ASSERT_EQ(1, clientsById.size());
    ChatUserListResponse chatUserListResponse(clientsById, recipients);

    std::stringstream serialized(chatUserListResponse.serialize().str());

    pt::ptree pt;
    ASSERT_NO_THROW(pt::read_xml(serialized, pt));
    ASSERT_EQ(1, pt.count("ChatUserListResponse"));
    const auto rootTag = pt.get_child("ChatUserListResponse");
    ASSERT_EQ(1, rootTag.count("UserList"));
    const auto userTag = rootTag.get_child("UserList");
    ASSERT_EQ(1, userTag.count("Status"));
    EXPECT_EQ(client1->getChatStatus(), userTag.get<unsigned char>("Status"));
    ASSERT_EQ(1, userTag.count("UserName"));
    EXPECT_EQ(client1->getChatUsername(), userTag.get<std::string>("UserName"));
    ASSERT_EQ(1, userTag.count("isOtherSystemUser"));
    ASSERT_TRUE(client1->getIsOtherSystemUser());
    EXPECT_THAT(userTag.get<std::string>("isOtherSystemUser"), ::testing::StrCaseEq("true"));
    ASSERT_EQ(1, userTag.count("UserID"));
    EXPECT_EQ(client1->getId(), userTag.get<int>("UserID"));
}

TEST_F(ChatUserListResponseTests, createdChatUserListResponse_afterClientDataChange_chatUserListResponseShouldAlsoChange) {
    clientsById.insert({client1->getId(), client1});
    ChatUserListResponse chatUserListResponse(clientsById, recipients);

    const auto serialized = chatUserListResponse.serialize().str();
    const auto newStatus = models::AWAY;
    ASSERT_NE(newStatus, client1->getChatStatus());
    client1->setChatStatus(newStatus);
    const auto serialized1 = chatUserListResponse.serialize().str();

    ASSERT_NE(serialized, serialized1);
}

TEST_F(ChatUserListResponseTests, createdChatUserListResponse_afterClientByIdChange_chatUserListResponseShouldAlsoChange) {
    clientsById.insert({client1->getId(), client1});
    ChatUserListResponse chatUserListResponse(clientsById, recipients);

    const auto serialized = chatUserListResponse.serialize().str();
    clientsById.insert({client2->getId(), client2});
    const auto serialized1 = chatUserListResponse.serialize().str();

    ASSERT_NE(serialized, serialized1);
}

TEST_F(ChatUserListResponseTests,
       createdChatUserListResponseByRValueClientByIdReference_afterClientDataChange_chatUserListResponseShouldAlsoChange) {
    clientsById.insert({client1->getId(), client1});
    ChatUserListResponse chatUserListResponse(std::move(clientsById), recipients);

    const auto serialized = chatUserListResponse.serialize().str();
    const auto newStatus = models::AWAY;
    ASSERT_NE(newStatus, client1->getChatStatus());
    client1->setChatStatus(newStatus);
    const auto serialized1 = chatUserListResponse.serialize().str();

    ASSERT_NE(serialized, serialized1);
}
