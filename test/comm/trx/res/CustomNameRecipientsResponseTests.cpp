
#include <gtest/gtest.h>

using namespace comm::trx::res;

class CustomNameRecipientsResponseTests : public testing::Test,
                                          s public testing::WithParamInterface<const CustomNameRecipientsResponse *> {
public:
    static inline const std::string recipient = "username";
};

INSTANTIATE_TEST_CASE_P(_, CustomNameRecipientsResponseTests,
                        testing::Values(&CustomNameRecipientsResponseTests::voiceMailNotification));

TEST_P(CustomNameRecipientsResponseTests, createdCustomNameRecipientsResponse_expectToRecipientsAlwaysBeCustomClientsByName) {
    auto customNameRecipientsResponse = GetParam();

    EXPECT_EQ(Recipients::CUSTOM_CLIENTS_BY_NAME, customNameRecipientsResponse->getRecipients());
}

TEST_P(CustomNameRecipientsResponseTests, createdCustomNameRecipientsResponse_expectToHasCorrectRecipientsList) {
    auto customIdRecipientsResponse = GetParam();
    ASSERT_EQ(models::ClientsUsernameSet{recipient}, customIdRecipientsResponse->getRecipientsList());
}
