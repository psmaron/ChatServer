#include "Server.h"

#include <memory>

#include <spawn/spawn.hpp>

#include <log4cpp/Category.hh>

#include "Connection.h"
#include "CoreErrors.h"
#include "comm/ClientCommunication.h"
#include "comm/Communication.h"
#include "comm/trx/TransactionBuilder.h"
#include "db/DatabaseProxy.hpp"
#include "models/Clients.h"
#include "models/DataProvider.h"
#include "utils/Logging.hpp"

namespace core {

using boost::asio::ip::tcp;

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server"));

Server::Server(std::shared_ptr<db::DatabaseProxy> databaseProxy, boost::asio::io_service &io_service,
               const unsigned int &port) try : acceptor(io_service, tcp::endpoint(tcp::v6(), port)),
                                               clients(std::make_shared<models::Clients>()),
                                               clientsSockets(std::make_shared<ClientsSockets>()),
                                               databaseProxy(databaseProxy) {
} catch (boost::system::system_error &e) {
    LOG4CPP_INFO_S(logger) << "Error while creating Server: " << e.what();
}

Server::~Server() {
    acceptor.get_io_service().stop();
    LOG4CPP_DEBUG_S(logger) << "Destructor ~Server called";
}

void Server::initialize(boost::asio::io_service &io_service, std::shared_ptr<db::DatabaseProxy> databaseProxy,
                        const unsigned int port) {
    try {
        std::make_shared<Server>(databaseProxy, io_service, port)->prepareForExecution();
        LOG4CPP_INFO_S(logger) << "Server successfully started at port " << port;
    } catch (boost::system::system_error &e) {
        LOG4CPP_ERROR_S(logger) << "Could not bind server to port " << port;
        throw;
    } catch (CoreError &e) {
        LOG4CPP_ERROR_S(logger) << "Could not start server due to internal error " << utils::log(e);
        throw;
    }
}

void Server::prepareForExecution() {
    transactionBuilder = std::make_shared<comm::trx::TransactionBuilder>(shared_from_this());
    spawn::spawn(acceptor.get_executor(), [self = shared_from_this(), this](spawn::yield_context yield) { start(yield); });
}

void Server::start(const spawn::yield_context &yield) {
    for (;;) {
        auto socket = std::make_unique<tcp::socket>(acceptor.get_io_service());
        boost::system::error_code ec;
        acceptor.async_accept(*socket, yield[ec]);
        if (!ec) {
            int clientId = socket->native_handle();
            if (!clients->tryAddClient(clientId)) {
                LOG4CPP_ERROR_S(logger) << "Could not add new client with id = " << clientId;
                continue;
            }
            clientsSockets->insert({clientId, std::move(socket)});
            auto clientData = clients->getClientData(clientId);
            auto clientsWantingStatusChangeNotification = clients->getClientsWantingStatusChangeNotification();
            auto clientsUsernameToId = clients->getClientsUsernameToId();
            auto clientCommunication = std::make_unique<comm::ClientCommunication>(
                clientsSockets, clientsWantingStatusChangeNotification, clientsUsernameToId, clientData, transactionBuilder);
            Connection::create(clientData, std::move(clientCommunication))->start(acceptor.get_io_service());
        } else {
            LOG4CPP_ERROR_S(logger) << "Couldn't accept connection due to: " << ec.message() << ". Waiting for another...";
        }
    }
}

std::shared_ptr<const models::Clients> Server::getClients() const {
    return clients;
}

std::shared_ptr<const ClientsSockets> Server::getClientsSockets() {
    return clientsSockets;
}

std::shared_ptr<db::DatabaseProxy> Server::getDatabaseProxy() {
    return databaseProxy;
}

} // namespace core
