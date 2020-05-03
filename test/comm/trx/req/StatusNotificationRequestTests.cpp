#include "comm/trx/req/StatusNotificationRequest.h"

#include <boost/property_tree/ptree.hpp>

#include <comm/CommErrors.h>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "helpers/generator/XmlRequestGenerator.h"

namespace pt = boost::property_tree;
using namespace comm::trx::req;

class StatusNotificationRequestTests : public testing::Test {
protected:
    const std::string notifyOnTrue = "TRUE";
    const std::string notifyOnTrueCaseMixed = "TrUe";
    const std::string notifyOnFalse = "FALSE";
    const std::string notifyOnPrefixedTRUE = "TRUEsomething";
    const std::string notifyOnInvalidValue = "invalid";
};

TEST_F(StatusNotificationRequestTests, givenWrongRootTagName_whenCreatingStatusNotificationRequest_shouldThrow) {
    const auto xmlRequest = XmlRequestGenerator("WrongRootTagName").build();

    ASSERT_THROW(StatusNotificationRequest req(xmlRequest), pt::ptree_bad_path);
}

TEST_F(StatusNotificationRequestTests, givenReqWithMissingNotifyOnField_whenCreatingStatusNotificationRequest_shouldThrow) {
    auto requestGenerator = SpecificXmlRequestGenerator<StatusNotificationRequest>().removeField("NotifyOn");
    ASSERT_EQ(requestGenerator.countField("NotifyOn"), 0);
    const auto xmlRequest = requestGenerator.build();

    ASSERT_THROW(StatusNotificationRequest req(xmlRequest), comm::ParsingRequestDataError);
}

TEST_F(StatusNotificationRequestTests,
       givenReqWithNotifyOnFieldPrefixedTRUE_whenCreatingStatusNotificationRequest_expectNotifyOnToBeTrue) {
    ASSERT_THAT(notifyOnPrefixedTRUE, ::testing::StartsWith("TRUE"));
    const auto xmlRequest = SpecificXmlRequestGenerator<StatusNotificationRequest>(notifyOnPrefixedTRUE).build();

    StatusNotificationRequest statusNotificationRequest(xmlRequest);

    const auto [parsedNotifyOn] = statusNotificationRequest;
    EXPECT_TRUE(parsedNotifyOn);
}

TEST_F(StatusNotificationRequestTests,
       givenReqWithNotifyOnFieldTrueMixedCase_whenCreatingStatusNotificationRequest_shouldBeCaseInsensitive) {
    ASSERT_THAT(notifyOnTrueCaseMixed, ::testing::StrCaseEq("true"));
    const auto xmlRequest = SpecificXmlRequestGenerator<StatusNotificationRequest>(notifyOnTrueCaseMixed).build();

    StatusNotificationRequest statusNotificationRequest(xmlRequest);

    const auto [parsedNotifyOn] = statusNotificationRequest;
    EXPECT_TRUE(parsedNotifyOn);
}

TEST_F(StatusNotificationRequestTests,
       givenReqWithInvalidNotifyOnValue_whenCreatingStatusNotificationRequest_expectNotifyOnToBeFalse) {
    ASSERT_NE(notifyOnInvalidValue, "TRUE");
    ASSERT_NE(notifyOnInvalidValue, "FALSE");
    const auto xmlRequest = SpecificXmlRequestGenerator<StatusNotificationRequest>(notifyOnInvalidValue).build();

    StatusNotificationRequest statusNotificationRequest(xmlRequest);

    const auto [parsedNotifyOn] = statusNotificationRequest;
    EXPECT_FALSE(parsedNotifyOn);
}

TEST_F(StatusNotificationRequestTests, givenReqWithDuplicatedNotifyOnField_whenCreatingStatusNotificationRequest_shouldThrow) {
    auto requestGenerator = SpecificXmlRequestGenerator<StatusNotificationRequest>().addField("NotifyOn", notifyOnTrue);
    ASSERT_GT(requestGenerator.countField("NotifyOn"), 1);
    const auto xmlRequest = requestGenerator.build();

    EXPECT_THROW(StatusNotificationRequest statusNotificationRequest(xmlRequest), comm::ParsingRequestDataError);
}

TEST_F(StatusNotificationRequestTests,
       givenReqWithNotifyOnFieldFalse_whenCreatingStatusNotificationRequest_expecToNotifyOnBeFalse) {
    const auto xmlRequest = SpecificXmlRequestGenerator<StatusNotificationRequest>(notifyOnFalse).build();

    StatusNotificationRequest statusNotificationRequest(xmlRequest);

    const auto [parsedNotifyOn] = statusNotificationRequest;
    EXPECT_FALSE(parsedNotifyOn);
}