#include "db/dao/ErrorLogDao.h"

#include <memory>

#include <gtest/gtest.h>

#include "db/DatabaseProxy.hpp"

using namespace testing;

struct ErrorLogDaoTests : testing::Test {};

namespace db::impl {

template <typename Query> class Execute<Query, std::chrono::milliseconds> {
public:
    static void execute([[maybe_unused]] ConnectionPoolProvider &connectionProvider, [[maybe_unused]] const Query query,
                        [[maybe_unused]] std::chrono::milliseconds timeout, [[maybe_unused]] const spawn::yield_context &yield) {
        // Here goes mocked implementation
    }
};

} // namespace db::impl

TEST_F(ErrorLogDaoTests, instantiateMockedDatabaseCalls) {
    using namespace ozo::literals;

    boost::asio::io_context io_context;
    spawn::spawn(io_context, [&](const spawn::yield_context yield) {
        std::shared_ptr<db::DatabaseProxy> databaseProxy = std::make_shared<db::DatabaseProxy>(io_context);
        databaseProxy->execute("INSERT INTO error_log_table (action_date, action_time, error_type_id, change_desc) "_SQL
                               "VALUES (CURRENT_DATE, CURRENT_TIME, 16, )"_SQL,
                               std::chrono::milliseconds(500), yield);
    });
    io_context.run();
}

TEST_F(ErrorLogDaoTests, performRealTest) {
    // After linker is "cheated" and fed with overriden specializations
    // you can write test code that calls mocked methods
}