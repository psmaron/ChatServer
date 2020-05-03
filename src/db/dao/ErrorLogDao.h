#pragma once

#include <chrono>
#include <string>

#include <ozo/shortcuts.h>
#include <spawn/spawn.hpp>

namespace db {

class DatabaseProxy;

namespace dao {

class ErrorLogDao {
public:
    explicit ErrorLogDao(std::string message, int category = 0,
                         std::chrono::milliseconds timeout = std::chrono::milliseconds(500));

    void save(std::shared_ptr<db::DatabaseProxy> &databaseProxy, const spawn::yield_context &yield);

private:
    std::string message;
    int category;
    std::chrono::milliseconds timeout;
};

} // namespace dao
} // namespace db