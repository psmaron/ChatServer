#include "comm/trx/res/GenericResponse.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace comm::trx;
namespace pt = boost::property_tree;

class GenericReponseTests : public ::testing::Test {
protected:
    static constexpr const char *genericResponseTag = "GenericResponse";
};

TEST_F(GenericReponseTests, givenValidGenericResponse_shouldSerializeCorrectly) {
    const std::string message{"test Message"};
    const int transactionId = 0;
    InvalidRequestDescription description(message, InvalidResponseType::NotError, transactionId);
    res::GenericResponse genericResponse{description};

    std::stringstream serialized(genericResponse.serialize().str());

    EXPECT_EQ(genericResponse.getRecipients(), comm::trx::res::Recipients::ONLY_SENDER);
    pt::ptree pt;
    ASSERT_NO_THROW(pt::read_xml(serialized, pt));
    ASSERT_EQ(1, pt.count(genericResponseTag));
    const auto rootTag = pt.get_child(genericResponseTag);
    EXPECT_EQ(rootTag.get<std::string>("Message"), message);
    EXPECT_EQ(rootTag.get<std::string>("IsError"), "false");
    EXPECT_EQ(rootTag.get<int>("TransID"), transactionId);
}
