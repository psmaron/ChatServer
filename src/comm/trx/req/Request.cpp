#include "Request.h"

#include <stdexcept>

#include <boost/property_tree/ptree.hpp>

#include <log4cpp/Category.hh>

#include "ChatTo.h"
#include "ChatUserListRequest.h"
#include "ChatUserName.h"
#include "InvalidXmlRequest.hpp"
#include "StatusNotificationRequest.h"
#include "comm/CommErrors.h"

namespace comm::trx::req {

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server.connection.clientCommunication.request"));

std::unique_ptr<trx::req::Request> Request::createRequest(const boost::property_tree::ptree &requestXmlTree) {
    if (requestXmlTree.empty()) {
        return std::make_unique<InvalidXmlRequest>(
            InvalidRequestDescription{std::string{"Empty XML document"}, InvalidResponseType::Error, 0});
    }
    std::string header = requestXmlTree.begin()->first;
    try {
        if (header == "ChatTo")
            return std::make_unique<ChatTo>(requestXmlTree);
        else if (header == "ChatUserName")
            return std::make_unique<ChatUserName>(requestXmlTree);
        else if (header == "ChatUserListRequest")
            return std::make_unique<ChatUserListRequest>(requestXmlTree);
        else if (header == "StatusNotificationRequest")
            return std::make_unique<StatusNotificationRequest>(requestXmlTree);

    } catch (boost::property_tree::ptree_error &ex) {
        LOG4CPP_ERROR_S(logger) << "Parsing " << header << "  was unsuccessful due to : " << ex.what();
    } catch (std::logic_error &ex) {
        LOG4CPP_ERROR_S(logger) << "Parsing " << header << "  was unsuccessful due to : " << ex.what();
    }

    throw ParsingRequestDataError{};
}

} // namespace comm::trx::req
