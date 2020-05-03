#pragma once

#include "models/Clients.h"
#include "models/DataProvider.h"

namespace core {
class DatabaseProxy;
}

namespace models {

class FakeDataProvider : public DataProvider {
public:
    std::shared_ptr<const Clients> getClients() const override {
        return clients;
    }

    std::shared_ptr<const ClientsSockets> getClientsSockets() override {
        return sockets;
    }

    std::shared_ptr<db::DatabaseProxy> getDatabaseProxy() override {
        return nullptr;
    };

private:
    std::shared_ptr<const Clients> clients = std::make_shared<const Clients>();
    std::shared_ptr<const ClientsSockets> sockets = std::make_shared<const ClientsSockets>();
};
} // namespace models