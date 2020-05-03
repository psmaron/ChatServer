#include "StatusNotificationRequest.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/ptree.hpp>

#include "comm/CommErrors.h"

namespace comm::trx::req {

StatusNotificationRequest::StatusNotificationRequest(const boost::property_tree::ptree &requestXmlTree) {
    const auto &rootTag = requestXmlTree.get_child("StatusNotificationRequest");

    if (rootTag.count("NotifyOn") != 1)
        throw ParsingRequestDataError();

    isNotificationOn = boost::istarts_with(rootTag.get<std::string>("NotifyOn", "FALSE"), "TRUE");
}

} // namespace comm::trx::req