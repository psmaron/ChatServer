#include "DisconnectClient.h"

#include <log4cpp/Category.hh>

#include "DisconnectClientPimpl.h"

static log4cpp::Category &logger =
    log4cpp::Category::getInstance(std::string("server.connection.clientCommunication.transaction.action"));

namespace comm::trx::action {

DisconnectClient::Impl::Impl(std::shared_ptr<const ClientsSockets> clientsSockets) : clientsSockets(std::move(clientsSockets)) {
}

void DisconnectClient::Impl::disconnect(int clientId) {
    boost::system::error_code ec;
    clientsSockets->at(clientId)->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    clientsSockets->at(clientId)->close(ec);
    if (ec)
        LOG4CPP_WARN_S(logger) << log(clientId) << "Problem occurred while disconnecting client: " << ec.message();
}

void DisconnectClient::disconnect(int clientId) {
    pImpl->disconnect(clientId);
}

DisconnectClient::DisconnectClient(std::unique_ptr<Impl> &&pImpl) : pImpl(std::move(pImpl)) {
}

DisconnectClient::~DisconnectClient() = default;

} // namespace comm::trx::action