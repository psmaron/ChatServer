#pragma once

namespace boost::asio {
template <typename Protocol> class basic_stream_socket;
namespace ip {
class tcp;
} // namespace ip
} // namespace boost::asio

namespace db {
class DatabaseProxy;
}

using tcp_socket = boost::asio::basic_stream_socket<boost::asio::ip::tcp>;
using ClientsSockets = std::unordered_map<int, std::shared_ptr<tcp_socket>>;

namespace models {

class Clients;

class DataProvider {
public:
    virtual ~DataProvider() = default;

    virtual std::shared_ptr<const Clients> getClients() const = 0;
    virtual std::shared_ptr<const ClientsSockets> getClientsSockets() = 0;
    virtual std::shared_ptr<db::DatabaseProxy> getDatabaseProxy() = 0;
};

} // namespace models
