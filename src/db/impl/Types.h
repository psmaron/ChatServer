#pragma once

#include <ozo/connection_info.h>
#include <ozo/connection_pool.h>

namespace db::impl {

using ConnectionPool = ozo::connection_pool<ozo::connection_info<>>;
using ConnectionPoolProvider = ozo::connection_provider<ConnectionPool &>;

} // namespace db::impl