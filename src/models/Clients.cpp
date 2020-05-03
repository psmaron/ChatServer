#include "Clients.h"

#include "ClientData.h"

namespace models {

bool Clients::tryAddClient(int clientId) {
    if (clientId < 0)
        return false;

    std::shared_ptr<ClientData> clientData(
        new ClientData{clientId, ChatStatus::LOGOUT, "", "", -1, false, false, shared_from_this()});
    auto wasInserted = clientsById.emplace(clientId, std::move(clientData)).second;
    if (wasInserted)
        clientsWantingStatusChangeNotification->insert(clientId);

    return wasInserted;
}

std::shared_ptr<ClientData> Clients::getClientData(int clientId) const {
    return clientsById.at(clientId);
}

bool Clients::hasClient(int id) const {
    return clientsById.count(id) > 0;
}

void Clients::removeClient(int clientId) {
    if (!hasClient(clientId))
        return;
    const auto &client = clientsById.at(clientId);
    clientsUsernameToId->erase(client->getChatUsername());
    clientsById.erase(clientId);
    clientsWantingStatusChangeNotification->erase(clientId);
}

void Clients::resetClient(std::shared_ptr<ClientData> client) {
    clientsUsernameToId->erase(client->getChatUsername());
    client->reset();
}

const ClientsById &Clients::getClientsById() const {
    return clientsById;
}

RemovedClientId Clients::updateClient(int clientId, const std::string &newChatUsername, models::ChatStatus newStatus,
                                      bool newUserIsOtherSystem) {
    RemovedClientId deletedClientId;

    if (!hasClient(clientId))
        return deletedClientId; // TODO:: add log. This can happen if we got disconnected and try to update our data, but got
    // already removed

    const auto &currClient = clientsById.at(clientId);
    currClient->setChatStatus(newStatus);

    if (newChatUsername.empty()) {
        resetClient(currClient);
        return deletedClientId;
    }

    if (!newUserIsOtherSystem)
        deletedClientId = removeClientByNameIf(newChatUsername, [&currClient](std::shared_ptr<ClientData> client) {
            return !client->isOtherSystemUser && currClient->getId() != client->getId();
        });

    updateClientUsername(currClient, newChatUsername);
    currClient->setIsOtherSystemUser(newUserIsOtherSystem);
    return deletedClientId;
}

void Clients::setClientWantingStatusChangeNotification(int clientId, bool wantsStatusChangeNotification) {
    if (wantsStatusChangeNotification && hasClient(clientId))
        clientsWantingStatusChangeNotification->insert(clientId);
    else
        clientsWantingStatusChangeNotification->erase(clientId);
}

std::shared_ptr<const ClientsIdSet> Clients::getClientsWantingStatusChangeNotification() const {
    return clientsWantingStatusChangeNotification;
}

std::shared_ptr<const ClientUsernameToId> Clients::getClientsUsernameToId() const {
    return clientsUsernameToId;
}

RemovedClientId Clients::removeClientByNameIf(const std::string &username,
                                              std::function<bool(std::shared_ptr<ClientData>)> pred) {
    auto foundClient = getClientData(username);
    if (not foundClient.has_value() || not pred(*foundClient))
        return {};
    int foundClientId = (*foundClient)->getId();
    removeClient(foundClientId);
    return foundClientId;
}

void Clients::updateClientUsername(std::shared_ptr<ClientData> client, const std::string &newUsername) {
    clientsUsernameToId->erase(client->getChatUsername());
    client->setChatUserName(newUsername);
    clientsUsernameToId->insert({newUsername, client->getId()});
}

std::optional<std::shared_ptr<ClientData>> Clients::getClientData(const std::string &username) const {
    if (clientsUsernameToId->count(username) == 0)
        return {};
    int clientId = (*clientsUsernameToId)[username];
    if (!hasClient(clientId))
        return {};
    return getClientData(clientId);
}

} // namespace models