#pragma once

#include "ChatRequest.h"

namespace comm::trx::req {

class ChatUserListRequest : public ChatRequest {
public:
    explicit ChatUserListRequest(const boost::property_tree::ptree &) {
    }
};

} // namespace comm::trx::req
