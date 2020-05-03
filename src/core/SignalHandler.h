#pragma once

#include <boost/asio/io_service.hpp>

namespace core {

void connectSignals(boost::asio::io_service &io_service);

} // namespace core