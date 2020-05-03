#pragma once

#include <any>
#include <memory>

#include "CustomAction.hpp"
#include "CustomData.h"
#include "comm/trx/res/Response.hpp"
#include "req/Request.h"

namespace comm::trx {

class Transaction {
public:
    std::unique_ptr<req::Request> request;
    std::unique_ptr<res::Response> response;
    std::unique_ptr<CustomAction> customAction;
    std::unique_ptr<CustomData> customData;
};

} // namespace comm::trx