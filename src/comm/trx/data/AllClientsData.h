#pragma once

#include "comm/trx/CustomData.h"
#include "models/ClientFwd.h"

namespace comm::trx::data {

class AllClientsDataImpl;

class AllClientsData : public CustomData {
public:
    explicit AllClientsData(std::unique_ptr<AllClientsDataImpl> &&pImpl);

    ~AllClientsData() override;

    TVIRTUAL const models::ClientsById &getData();

private:
    std::unique_ptr<AllClientsDataImpl> pImpl;
};

} // namespace comm::trx::data