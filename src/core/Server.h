#pragma once

#include <memory>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <spawn/spawn.hpp>

#include "models/DataProvider.h"

namespace models {
class Clients;
} // namespace models

namespace db {
class DatabaseProxy;
} // namespace db

namespace comm::trx {
class TransactionBuilder;
} // namespace comm::trx

namespace core {

class Server : public std::enable_shared_from_this<Server>, public models::DataProvider {
public:
    explicit Server(std::shared_ptr<db::DatabaseProxy> databaseProxy, boost::asio::io_service &io_service,
                    const unsigned int &port);

    ~Server() override;

    static void initialize(boost::asio::io_service &io_service, std::shared_ptr<db::DatabaseProxy> databaseProxy,
                           const unsigned int port);

    std::shared_ptr<const models::Clients> getClients() const override;
    std::shared_ptr<const ClientsSockets> getClientsSockets() override;
    std::shared_ptr<db::DatabaseProxy> getDatabaseProxy() override;

private:
    void prepareForExecution();
    void start(const spawn::yield_context &yield);
    void handleClients(const spawn::yield_context &yield);

    boost::asio::ip::tcp::acceptor acceptor;
    std::shared_ptr<models::Clients> clients;
    std::shared_ptr<ClientsSockets> clientsSockets;
    std::shared_ptr<db::DatabaseProxy> databaseProxy;
    std::shared_ptr<comm::trx::TransactionBuilder> transactionBuilder;
};

} // namespace core
