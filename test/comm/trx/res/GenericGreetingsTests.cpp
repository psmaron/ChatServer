#include "comm/trx/res/GenericGreeting.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace comm::trx::res;
namespace pt = boost::property_tree;

TEST(GenericGreetingsTests, givenValidResponse_shouldSerializeCorrectly) {
    const int clientId = 2;
    GenericGreeting genericGreetings(clientId);

    std::stringstream serialized(genericGreetings.serialize().str());

    pt::ptree pt;
    ASSERT_NO_THROW(pt::read_xml(serialized, pt));
    ASSERT_EQ(1, pt.count("GenericGreeting"));
    const auto rootTag = pt.get_child("GenericGreeting");
    const auto serverVersionRegex = ::testing::MatchesRegex(R"(ChatServer  v[0-9]+\.[0-9]+\.[0-9]+)");
    EXPECT_THAT(rootTag.get<std::string>("DSVersion"), serverVersionRegex);
    EXPECT_THAT(rootTag.get<std::string>("NewDSVersion"), serverVersionRegex);
    EXPECT_EQ(clientId, rootTag.get<int>("NetID"));
}