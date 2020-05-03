#include "ErrorLogDao.h"

#include <ozo/shortcuts.h>

#include "db/DatabaseProxy.hpp"
#include "utils/Logging.hpp"

using namespace ozo::literals;

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("database.logDao"));

namespace db::dao {

ErrorLogDao::ErrorLogDao(std::string message, int category, std::chrono::milliseconds timeout)
    : message(std::move(message)), category(category), timeout(timeout) {
}

void ErrorLogDao::save(std::shared_ptr<db::DatabaseProxy> &databaseProxy, const spawn::yield_context &yield) {
    const auto query = "INSERT INTO error_log_table (action_date, action_time, error_type_id, change_desc) "_SQL
                       "VALUES (CURRENT_DATE, CURRENT_TIME, "_SQL +
                       category + ", "_SQL + message + ")"_SQL;
    try {
        databaseProxy->execute(query, timeout, yield);
    } catch (db::DatabaseError &e) {
        LOG4CPP_ERROR_S(logger) << "Could not insert nsc_error_log due to " << utils::log(e);
    }
}

} // namespace db::dao