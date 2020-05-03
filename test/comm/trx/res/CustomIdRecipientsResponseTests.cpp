#include "comm/trx/res/ChatFrom.h"

#include <gtest/gtest.h>

using namespace comm::trx::res;

class CustomIdRecipientsResponseTests : public testing::Test,
                                        public testing::WithParamInterface<const CustomIdRecipientsResponse *> {
public:
    static inline const models::ClientsIdSet recipients = {2, 5};
    static inline const ChatFrom chatFromResponse = {"msg", recipients};
};

INSTANTIATE_TEST_CASE_P(_, CustomIdRecipientsResponseTests, testing::Values(&CustomIdRecipientsResponseTests::chatFromResponse));

TEST_P(CustomIdRecipientsResponseTests, createdCustomIdRecipientsResponse_expectToRecipientsAlwaysBeCustomClientsById) {
    auto customIdRecipientsResponse = GetParam();

    EXPECT_EQ(Recipients::CUSTOM_CLIENTS_BY_ID, customIdRecipientsResponse->getRecipients());
}

TEST_P(CustomIdRecipientsResponseTests, createdCustomIdRecipientsResponse_expectToHasCorrectRecipientsList) {
    auto customIdRecipientsResponse = GetParam();
    ASSERT_EQ(recipients, customIdRecipientsResponse->getRecipientsList());
}
