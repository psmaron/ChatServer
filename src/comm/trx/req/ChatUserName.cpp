#include "ChatUserName.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/ptree.hpp>

#include "comm/CommErrors.h"

namespace comm::trx::req {

ChatUserName::ChatUserName(const boost::property_tree::ptree &requestXmlTree) {
    const auto &rootTag = requestXmlTree.get_child("ChatUserName");

    if (rootTag.count("UserID") > 1 || rootTag.count("UserName") > 1 || rootTag.count("Status") > 1 ||
        rootTag.count("isOtherSystemUser") > 1)
        throw ParsingRequestDataError();

    id = std::stoi(rootTag.get<std::string>("UserID", "-1"));
    chatUsername = rootTag.get<std::string>("UserName", "");
    chatStatus = models::convertToChatStatus(std::stoi(rootTag.get<std::string>("Status", "1")));
    isOtherSystemUser = boost::istarts_with(rootTag.get<std::string>("isOtherSystemUser", "FALSE"), "TRUE");
}

} // namespace comm::trx::req
