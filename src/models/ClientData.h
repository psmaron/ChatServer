#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ChatStatus.hpp"
#include "ClientFwd.h"

namespace models {

class ClientData {
    friend Clients;

public:
    TVIRTUAL ~ClientData() = default;

    const int &getId() const {
        return id;
    }
    const ChatStatus &getChatStatus() const {
        return chatStatus;
    }
    void setChatStatus(ChatStatus chatStatus) {
        this->chatStatus = chatStatus;
    }
    const std::string &getChatUsername() const {
        return chatUsername;
    }
    void setChatUserName(const std::string &username) {
        this->chatUsername = username;
    }

    TVIRTUAL RemovedClientId updateData(const std::string &newChatUsername, models::ChatStatus status, bool isOtherSystem);

    const std::string &getMachineName() const {
        return machineName;
    }

    TVIRTUAL void setWantsStatusChangeNotifications(bool wantsStatusChangeNotifications);

    const bool &getIsOtherSystemUser() const {
        return isOtherSystemUser;
    }

    void removeClient();

    void reset();

protected:
    ClientData(int a, ChatStatus b, std::string c, std::string d, int e, bool f, bool g, std::weak_ptr<Clients> h);

    const int id;
    ChatStatus chatStatus;
    std::string chatUsername;
    std::string machineName;
    bool isOtherSystemUser;

    std::weak_ptr<Clients> clients;
};

} // namespace models