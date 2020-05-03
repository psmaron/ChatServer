#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ChatStatus.hpp"
#include "ClientFwd.h"

namespace models {

class Clients : public std::enable_shared_from_this<Clients> {
public:
    friend ClientData;

    bool tryAddClient(int clientId);

    std::shared_ptr<ClientData> getClientData(int clientId) const;

    bool hasClient(int id) const;

    void removeClient(int clientId);

    void resetClient(std::shared_ptr<ClientData> client);

    const ClientsById &getClientsById() const;

    RemovedClientId updateClient(int clientId, const std::string &newChatUsername, models::ChatStatus status,
                                 bool newUserIsOtherSystem);

    void setClientWantingStatusChangeNotification(int clientId, bool wantsStatusChangeNotification);

    std::shared_ptr<const ClientsIdSet> getClientsWantingStatusChangeNotification() const;

    std::shared_ptr<const ClientUsernameToId> getClientsUsernameToId() const;

private:
    RemovedClientId removeClientByNameIf(const std::string &username, std::function<bool(std::shared_ptr<ClientData>)> pred);
    void updateClientUsername(std::shared_ptr<ClientData> client, const std::string &newUsername);
    std::optional<std::shared_ptr<ClientData>> getClientData(const std::string &username) const;

    ClientsById clientsById;
    std::shared_ptr<ClientUsernameToId> clientsUsernameToId = std::make_shared<ClientUsernameToId>();
    std::shared_ptr<ClientsIdSet> clientsWantingStatusChangeNotification = std::make_shared<ClientsIdSet>();
};

} // namespace models
