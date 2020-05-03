#pragma once

#include <ozo/execute.h>

#include <log4cpp/Category.hh>

namespace db {

template <typename Conn> static void handleError(const ozo::error_code &ec, Conn &connection) {
    static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("database"));
    if (ec) {
        LOG4CPP_ERROR_S(logger) << "Database request failed with error: " << ec.message();
        if (!ozo::is_null_recursive(connection)) {
            if (auto msg = ozo::error_message(connection); !msg.empty())
                LOG4CPP_ERROR_S(logger) << "Error message: " << msg;
            if (auto ctx = ozo::get_error_context(connection); !ctx.empty())
                LOG4CPP_ERROR_S(logger) << "Error context: " << ctx;
        }
        throw db::DatabaseUnknownError();
    }
}

} // namespace db