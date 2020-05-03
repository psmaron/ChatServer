#include "models/ClientData.h"

#include <gtest/gtest.h>

#include "models/ChatStatus.hpp"
#include "models/Clients.h"

using namespace models;

class ClientsTest : public testing::Test {

protected:
    std::shared_ptr<models::Clients> clients = std::make_shared<models::Clients>();
    int invalidClientId = -1;
    int clientId1 = 1;
    int clientId2 = 2;
    int clientId3 = 3;
    int clientId4 = 4;
};

TEST_F(ClientsTest, whenAddingClientsWithDifferentIds_shouldSucceed) {
    EXPECT_TRUE(clients->tryAddClient(clientId1));
    EXPECT_TRUE(clients->tryAddClient(clientId2));
    EXPECT_TRUE(clients->tryAddClient(clientId3));
    EXPECT_TRUE(clients->tryAddClient(clientId4));
    EXPECT_TRUE(clients->hasClient(clientId1));
    EXPECT_TRUE(clients->hasClient(clientId2));
    EXPECT_TRUE(clients->hasClient(clientId3));
    EXPECT_TRUE(clients->hasClient(clientId4));
    EXPECT_EQ(clients->getClientsById().size(), 4);
}

TEST_F(ClientsTest, whenAddingClient_shouldProperlySetItsDefaultValues) {
    clients->tryAddClient(clientId1);
    auto clientData = clients->getClientData(clientId1);
    EXPECT_EQ(clientData->getId(), clientId1);
    EXPECT_EQ(clientData->getChatStatus(), models::ChatStatus::LOGOUT);
    EXPECT_EQ(clientData->getChatUsername(), "");
    EXPECT_EQ(clientData->getMachineName(), "");
    EXPECT_EQ(clientData->getIsOtherSystemUser(), false);
    EXPECT_EQ(clientData->getId(), clientId1);
    // ChatStatus::LOGOUT, "", "", -1, true, false, false, clientSocket, this
}

TEST_F(ClientsTest, whenAddingClientWithInvalidClientId_shouldFail) {
    EXPECT_FALSE(clients->tryAddClient(invalidClientId));
    EXPECT_FALSE(clients->hasClient(clientId3));
    EXPECT_TRUE(clients->getClientsById().empty());
}

TEST_F(ClientsTest, givenClientWasAdded_whenAddingClientWithTheSameId_shouldFail) {
    clients->tryAddClient(clientId1);
    EXPECT_FALSE(clients->tryAddClient(clientId1));
}

class UpdateClientDataTests : public ClientsTest, public testing::WithParamInterface<bool> {

    void SetUp() {
        clients->tryAddClient(clientId1);
        notOtherSystemUser1 = clients->getClientData(clientId1);
        notOtherSystemUser1->updateData("User1", models::LOGIN, false);
        clients->tryAddClient(clientId2);
        notOtherSystemUser2 = clients->getClientData(clientId2);
        notOtherSystemUser2->updateData("User2", models::LOGIN, false);
        clients->tryAddClient(clientId3);
        otherSystemUser1 = clients->getClientData(clientId3);
        otherSystemUser1->updateData("User3", models::LOGIN, true);
        clients->tryAddClient(clientId4);
        otherSystemUser2 = clients->getClientData(clientId4);
        otherSystemUser2->updateData("User4", models::LOGIN, true);

        ASSERT_FALSE(notOtherSystemUser1->getIsOtherSystemUser());
        ASSERT_FALSE(notOtherSystemUser2->getIsOtherSystemUser());
        ASSERT_TRUE(otherSystemUser1->getIsOtherSystemUser());
        ASSERT_TRUE(otherSystemUser2->getIsOtherSystemUser());

        for (auto [_, client] : clients->getClientsById()) {
            ASSERT_NE(client->getChatUsername(), newUserName);
            ASSERT_NE(newChatStatus, client->getChatStatus());
        }
    }

protected:
    std::shared_ptr<models::ClientData> otherSystemUser1;
    std::shared_ptr<models::ClientData> otherSystemUser2;
    std::shared_ptr<models::ClientData> notOtherSystemUser1;
    std::shared_ptr<models::ClientData> notOtherSystemUser2;

    models::ChatStatus newChatStatus = models::AWAY;
    std::string newUserName = "newUsername";
};

INSTANTIATE_TEST_CASE_P(_, UpdateClientDataTests, testing::Values(true, false));

TEST_P(UpdateClientDataTests, emptyNewUsername_whenUpdatingClient_shouldResetUserData) {
    auto user = GetParam() ? otherSystemUser1 : notOtherSystemUser1;
    auto oldUserName = user->getChatUsername();

    EXPECT_FALSE(user->updateData("", newChatStatus, true).has_value());
    EXPECT_EQ("", user->getChatUsername());
    EXPECT_FALSE(user->getIsOtherSystemUser());
}

TEST_P(UpdateClientDataTests, givenNotEmptyUserName_whenUpdatingClient_shouldChangeCorrectlyUserData) {
    auto user = GetParam() ? otherSystemUser1 : notOtherSystemUser1;
    ASSERT_NE(newChatStatus, user->getChatStatus());
    ASSERT_NE(newUserName, user->getChatUsername());
    bool newIsOtherSystem = !user->getIsOtherSystemUser();

    EXPECT_FALSE(user->updateData(newUserName, newChatStatus, newIsOtherSystem).has_value());
    EXPECT_EQ(newChatStatus, user->getChatStatus());
    EXPECT_EQ(newUserName, user->getChatUsername());
    EXPECT_EQ(newIsOtherSystem, user->getIsOtherSystemUser());
}

TEST_P(UpdateClientDataTests, changeToNotOtherSystemExistingUsername_whenUpdatingClientData_shouldRemoveExistingUser) {
    auto user = GetParam() ? otherSystemUser1 : notOtherSystemUser1;
    auto oldUserName = user->getChatUsername();
    auto newUserName = notOtherSystemUser2->getChatUsername();

    EXPECT_EQ(user->updateData(newUserName, newChatStatus, false).value(), notOtherSystemUser2->getId());

    EXPECT_TRUE(clients->hasClient(user->getId()));
    EXPECT_FALSE(clients->hasClient(notOtherSystemUser2->getId()));
}

TEST_P(UpdateClientDataTests, changeToNotOtherSystemNewUsername_whenUpdatingClientData_shouldNotRemoveAnyUser) {
    auto user = GetParam() ? otherSystemUser1 : notOtherSystemUser1;
    auto oldUserName = user->getChatUsername();

    EXPECT_FALSE(user->updateData(newUserName, newChatStatus, false).has_value());

    EXPECT_TRUE(clients->hasClient(otherSystemUser1->getId()));
    EXPECT_TRUE(clients->hasClient(otherSystemUser2->getId()));
    EXPECT_TRUE(clients->hasClient(notOtherSystemUser1->getId()));
    EXPECT_TRUE(clients->hasClient(notOtherSystemUser2->getId()));
}

TEST_P(UpdateClientDataTests, changeToOtherSystemExistingUsername_whenUpdatingClientData_shouldNotRemoveAnyUser) {
    auto user = GetParam() ? otherSystemUser1 : notOtherSystemUser1;
    auto oldUserName = user->getChatUsername();
    auto newUserName = otherSystemUser2->getChatUsername();

    EXPECT_FALSE(user->updateData(newUserName, newChatStatus, true).has_value());

    EXPECT_TRUE(clients->hasClient(otherSystemUser1->getId()));
    EXPECT_TRUE(clients->hasClient(otherSystemUser2->getId()));
    EXPECT_TRUE(clients->hasClient(notOtherSystemUser1->getId()));
    EXPECT_TRUE(clients->hasClient(notOtherSystemUser2->getId()));
}

TEST_P(UpdateClientDataTests, notChangeUsername_whenUpdatingClientData_shouldChangeOtherDataCorrectly) {
    auto user = GetParam() ? otherSystemUser1 : notOtherSystemUser1;
    bool newIsOtherSystemUser = not user->getIsOtherSystemUser();
    auto oldUserName = user->getChatUsername();

    EXPECT_FALSE(user->updateData(user->getChatUsername(), newChatStatus, newIsOtherSystemUser).has_value());

    EXPECT_TRUE(clients->hasClient(user->getId()));
    EXPECT_EQ(oldUserName, user->getChatUsername());
    EXPECT_EQ(newChatStatus, user->getChatStatus());
    EXPECT_EQ(newIsOtherSystemUser, user->getIsOtherSystemUser());
}

class StatusNotificationsClientDataTests : public ClientsTest {
protected:
    StatusNotificationsClientDataTests()
        : clientsWantsStatusChangeNotifications(clients->getClientsWantingStatusChangeNotification()) {
    }

    std::shared_ptr<const models::ClientsIdSet> clientsWantsStatusChangeNotifications;
};

TEST_F(StatusNotificationsClientDataTests,
       givenNewClientId_whenTryAddClient_shouldAddThisCLientToClientsWantStatusChangeNotification) {
    ASSERT_FALSE(clients->hasClient(clientId1));
    ASSERT_TRUE(clients->tryAddClient(clientId1));

    EXPECT_EQ(1, clientsWantsStatusChangeNotifications->count(clientId1));
}

TEST_F(StatusNotificationsClientDataTests,
       givenInvalidClientId_whenTryAddClient_shouldNotAddInalidCLientIdToClientsWantStatusChangeNotification) {
    ASSERT_FALSE(clients->tryAddClient(invalidClientId));

    EXPECT_EQ(0, clientsWantsStatusChangeNotifications->count(invalidClientId));
}

TEST_F(StatusNotificationsClientDataTests,
       givenExistingStatusNotificationEnabledClientId_whenTryAddClient_shouldNotChangeClientsWantStatusChangeNotificationSet) {
    ASSERT_TRUE(clients->tryAddClient(clientId1));
    ASSERT_EQ(1, clientsWantsStatusChangeNotifications->count(clientId1));
    auto copyClientsWantsStatusChangeNotifications = clientsWantsStatusChangeNotifications;

    ASSERT_FALSE(clients->tryAddClient(clientId1));

    EXPECT_EQ(copyClientsWantsStatusChangeNotifications, clientsWantsStatusChangeNotifications);
}

TEST_F(StatusNotificationsClientDataTests,
       givenExistingStatusNotificationDisabledClientId_whenTryAddClient_shouldNotChangeClientsWantStatusChangeNotificationSet) {
    ASSERT_TRUE(clients->tryAddClient(clientId1));
    clients->setClientWantingStatusChangeNotification(clientId1, false);
    ASSERT_EQ(0, clientsWantsStatusChangeNotifications->count(clientId1));
    auto copyClientsWantsStatusChangeNotifications = clientsWantsStatusChangeNotifications;

    ASSERT_FALSE(clients->tryAddClient(clientId1));

    EXPECT_EQ(copyClientsWantsStatusChangeNotifications, clientsWantsStatusChangeNotifications);
}

TEST_F(StatusNotificationsClientDataTests,
       giveNewClientId_whenAddNewClient_shouldAddThisCLientToCLientsWantStatusChangeNotification) {
    clients->tryAddClient(clientId1);
    ASSERT_EQ(1, clientsWantsStatusChangeNotifications->count(clientId1));

    clients->removeClient(clientId1);

    EXPECT_TRUE(clientsWantsStatusChangeNotifications->count(clientId1) == 0);
}

TEST_F(StatusNotificationsClientDataTests,
       setClientWantingStatusChangeNotification_shouldUpdateClientsWantsStatusChangeNotificationsCorectlly) {
    clients->tryAddClient(clientId1);
    ASSERT_EQ(1, clientsWantsStatusChangeNotifications->count(clientId1));

    clients->setClientWantingStatusChangeNotification(clientId1, false);

    EXPECT_TRUE(clientsWantsStatusChangeNotifications->count(clientId1) == 0);

    clients->setClientWantingStatusChangeNotification(clientId1, true);

    EXPECT_TRUE(clientsWantsStatusChangeNotifications->count(clientId1) == 1);
}
