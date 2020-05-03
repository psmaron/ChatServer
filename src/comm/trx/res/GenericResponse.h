#pragma once

#include <string>

#include "comm/trx/req/InvalidRequestDescription.h"
#include "comm/trx/res/Response.hpp"

namespace comm::trx::res {

class GenericResponse : public Response {
public:
    GenericResponse(InvalidRequestDescription requestDescription);

    std::ostringstream serialize() const override;

private:
    const InvalidRequestDescription requestDescription;
};

} // namespace comm::trx::res