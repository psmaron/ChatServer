#pragma once

#include <string>

#include <gmock/gmock.h>

#include "models/ChatStatus.hpp"
#include "models/ClientData.h"

namespace models {

class FakeClientData : public models::ClientData {
public:
    FakeClientData(int id, models::ChatStatus status, std::string username, bool isOtherSystemUser,
                   std::weak_ptr<models::Clients> i = std::weak_ptr<models::Clients>())
        : models::ClientData(id, status, username, "machineName", 0, false, isOtherSystemUser, i) {
    }

    MOCK_METHOD(void, setWantsStatusChangeNotifications, (bool));
    MOCK_METHOD(RemovedClientId, updateData, (const std::string &newChatUsername, models::ChatStatus status, bool isOtherSystem));
};
} // namespace models