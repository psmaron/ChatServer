#pragma once

#include <memory>

#include "comm/trx/CustomAction.hpp"

namespace comm::trx::action {

class DisconnectClient : public CustomAction {
public:
    class Impl;

    DisconnectClient(std::unique_ptr<Impl> &&pImpl);

    ~DisconnectClient() override;

    void disconnect(int clientId);

private:
    std::unique_ptr<Impl> pImpl;
};

} // namespace comm::trx::action