#pragma once

#include <memory>

#include <boost/asio/io_service.hpp>

#include <spawn/spawn.hpp>

#include "comm/ClientCommunication.h"
#include "models/ClientData.h"

namespace core {

class Connection : public std::enable_shared_from_this<Connection> {
public:
    static std::shared_ptr<Connection> create(std::shared_ptr<models::ClientData> clientData,
                                              std::unique_ptr<comm::ClientCommunication> clientCommunication);

    ~Connection();

    void start(boost::asio::io_service &io_service);

private:
    explicit Connection(std::shared_ptr<models::ClientData> clientData,
                        std::unique_ptr<comm::ClientCommunication> clientCommunication);

    void startCommunication(const spawn::yield_context &yield);

    std::shared_ptr<models::ClientData> clientData;
    std::unique_ptr<comm::ClientCommunication> clientCommunication;
};

} // namespace core
