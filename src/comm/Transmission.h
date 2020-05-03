#pragma once

#include <string>

#include <boost/asio/ip/tcp.hpp>

#include <spawn/spawn.hpp>

#include "trx/req/Request.h"

namespace comm {
using boost::asio::ip::tcp;

boost::property_tree::ptree read(const tcp::socket &receiver, const spawn::yield_context &yield, const unsigned int &timeout = 0);
void send(const tcp::socket &receiver, const std::ostringstream &message, const spawn::yield_context &yield);

} // namespace comm
