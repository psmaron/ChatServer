#include "AllClientsData.h"

#include "models/ClientData.h"
#include "models/Clients.h"

namespace comm::trx::data {

class AllClientsDataImpl {
public:
    explicit AllClientsDataImpl(std::shared_ptr<const models::Clients> clients) : clients(std::move(clients)) {
    }

    const models::ClientsById &getData() {
        return clients->getClientsById();
    }

private:
    std::shared_ptr<const models::Clients> clients;
};

AllClientsData::AllClientsData(std::unique_ptr<AllClientsDataImpl> &&pImpl) : pImpl(std::move(pImpl)) {
}

AllClientsData::~AllClientsData() = default;

const models::ClientsById &AllClientsData::getData() {
    return pImpl->getData();
}

} // namespace comm::trx::data