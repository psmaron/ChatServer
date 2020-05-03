#include "InvalidRequestProcessor.h"

#include <log4cpp/Category.hh>

#include "comm/trx/data/ClientToBeDisconnected.h"
#include "comm/trx/req/InvalidHeaderRequest.h"
#include "comm/trx/req/InvalidXmlRequest.hpp"
#include "comm/trx/res/ChatUserListResponse.h"
#include "comm/trx/res/GenericResponse.h"

#include "utils/Types.hpp"

namespace proc::trx {

using namespace comm::trx;

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server.connection.invalidRequestProcessor"));

void InvalidRequestProcessor::process() {
    if (utils::isInstanceOf<req::InvalidXmlRequest>(transaction->request)) {
        processInvalidXmlRequest();
    } else if (utils::isInstanceOf<req::InvalidHeaderRequest>(transaction->request)) {
        processInvalidHeaderRequest();
    }
}

void InvalidRequestProcessor::processInvalidHeaderRequest() const {
    transaction->response = std::make_unique<res::ChatUserListResponse>(models::ClientsById{{clientData->getId(), clientData}},
                                                                        res::Recipients::STATUS_NOTIFICATION_SUBSCRIBERS);
    transaction->customData = std::make_unique<data::ClientToBeDisconnected>(clientData->getId());
}

void InvalidRequestProcessor::processInvalidXmlRequest() {
    auto [description] = *dynamic_cast<req::InvalidXmlRequest *>(transaction->request.get());

    LOG4CPP_INFO_S(logger) << "Handle InvalidXmlRequest, reason: " << description.message;
    transaction->response = std::make_unique<res::GenericResponse>(description);
}

} // namespace proc::trx