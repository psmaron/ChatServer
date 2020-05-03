#pragma once

#include "Transaction.h"
#include "models/DataProvider.h"
#include "req/Request.h"

namespace comm::trx {

class TransactionBuilder {
public:
    explicit TransactionBuilder(std::weak_ptr<models::DataProvider> dataProvider);

    std::unique_ptr<Transaction> build(std::unique_ptr<req::Request>) const;

private:
    std::weak_ptr<models::DataProvider> dataProvider;
};

} // namespace comm::trx