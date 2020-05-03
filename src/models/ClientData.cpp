#include "ClientData.h"

#include "ChatStatus.hpp"
#include "Clients.h"
#include "core/CoreErrors.h"

namespace models {
RemovedClientId ClientData::updateData(const std::string &newChatUsername, models::ChatStatus status, bool isOtherSystem) {
    if (auto clients = this->clients.lock(); clients)
        return clients->updateClient(id, newChatUsername, status, isOtherSystem);
    else
        throw core::DropConnection();
}

void ClientData::setWantsStatusChangeNotifications(bool wantsStatusChangeNotifications) {
    if (auto clients = this->clients.lock(); clients)
        clients->setClientWantingStatusChangeNotification(id, wantsStatusChangeNotifications);
    else
        throw core::DropConnection();
}

void ClientData::removeClient() {
    if (auto clients = this->clients.lock(); clients && clients->hasClient(id))
        clients->removeClient(id);
}

void ClientData::reset() {
    setChatUserName("");
    setIsOtherSystemUser(false);
}

ClientData::ClientData(int a, ChatStatus b, std::string c, std::string d, int e, bool f, bool g, std::weak_ptr<Clients> h)
    : id(a), chatStatus(b), chatUsername(std::move(c)), machineName(std::move(d)), isOtherSystemUser(g), clients(std::move(h)) {
}

} // namespace models
