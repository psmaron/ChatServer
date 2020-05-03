#pragma once

#include "DisconnectClient.h"

#include <boost/asio/ip/tcp.hpp>

namespace comm::trx::action {

using tcp_socket = boost::asio::basic_stream_socket<boost::asio::ip::tcp>;
using ClientsSockets = std::unordered_map<int, std::shared_ptr<tcp_socket>>;

class DisconnectClient::Impl {
public:
    explicit Impl(std::shared_ptr<const ClientsSockets> clientsSockets);

    void disconnect(int clientId);

private:
    std::shared_ptr<const ClientsSockets> clientsSockets;
};

} // namespace comm::trx::action