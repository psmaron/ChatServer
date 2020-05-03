#pragma once
#include "InvalidRequest.h"

namespace comm::trx::req {

class InvalidHeaderRequest : public InvalidRequest {
public:
    explicit InvalidHeaderRequest() {
    }
};

} // namespace comm::trx::req