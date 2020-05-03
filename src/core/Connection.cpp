#include "Connection.h"

#include <memory>
#include <string>
#include <utility>

#include <log4cpp/Category.hh>

#include "comm/Transmission.h"
#include "comm/trx/Transaction.h"
#include "proc/RequestProcessor.h"
#include "utils/Logging.hpp"

namespace core {

static log4cpp::Category &logger = log4cpp::Category::getInstance(std::string("server.connection"));
using utils::log;

std::shared_ptr<Connection> Connection::create(std::shared_ptr<models::ClientData> clientData,
                                               std::unique_ptr<comm::ClientCommunication> clientCommunication) {
    return std::shared_ptr<Connection>(new Connection(std::move(clientData), std::move(clientCommunication)));
}

Connection::~Connection() {
    LOG4CPP_DEBUG_S(logger) << log(clientData) << "Disconnected with client";
    clientData->removeClient();
}

void Connection::start(boost::asio::io_service &io_service) {
    LOG4CPP_DEBUG_S(logger) << log(clientData) << "Connected with client";
    try {
        spawn::spawn(io_service, [self = shared_from_this(), this](spawn::yield_context yield) { startCommunication(yield); });
    } catch (const std::exception &e) { // Be careful not to access client's memory in catch, as he's probably erased by then
        LOG4CPP_ERROR_S(logger) << "Unexpected exception when handling client: " << e.what() << ", terminating connection";
    } catch (...) {
        LOG4CPP_ERROR_S(logger) << "Unexpected exception when handling client, terminating connection";
    }
}

Connection::Connection(std::shared_ptr<models::ClientData> clientData,
                       std::unique_ptr<comm::ClientCommunication> clientCommunication)
    : clientData(std::move(clientData)), clientCommunication(std::move(clientCommunication)) {
}

void Connection::startCommunication(const spawn::yield_context &yield) {
    try {
        clientCommunication->sendGreeting(yield);
        for (;;) {
            try {
                auto transaction = clientCommunication->startTransaction(yield);
                proc::trx::RequestProcessor::create(transaction, clientData)->process();
                clientCommunication->finalizeTransaction(transaction, yield);
            } catch (core::AbortTransaction &e) {
                LOG4CPP_INFO_S(logger) << log(clientData) << "Dropping transaction " << utils::log(e);
            }
        }
    } catch (core::DropConnection &e) {
        LOG4CPP_INFO_S(logger) << log(clientData) << "Ending connection " << utils::log(e);
    }
}

} // namespace core
