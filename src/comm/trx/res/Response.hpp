#pragma once

#include <iosfwd>
#include <unordered_set>

#include "XmlPreamble.h"

namespace comm::trx::res {

enum class Recipients { ONLY_SENDER, CUSTOM_CLIENTS_BY_ID, CUSTOM_CLIENTS_BY_NAME, STATUS_NOTIFICATION_SUBSCRIBERS };

class Response {
public:
    Response(Recipients recipients) : recipients(recipients) {
    }

    virtual ~Response() = default;

    virtual std::ostringstream serialize() const = 0;

    Recipients getRecipients() const {
        return recipients;
    }

protected:
    Recipients recipients;
};

} // namespace comm::trx::res
