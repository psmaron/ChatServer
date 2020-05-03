#pragma once

#include <spawn/spawn.hpp>

#include <ozo/execute.h>
#include <ozo/request.h>
#include <ozo/transaction.h>

#include <log4cpp/Category.hh>

#include "db/impl/TransactionImpl.hpp"

namespace db {

// TODO: Rollback in dtor
template <typename... Ts> class Transaction {
public:
    explicit Transaction(ozo::impl::transaction<Ts...> &&ozoTransaction) : ozoTransaction(std::move(ozoTransaction)) {
    }

    template <typename Query, typename Timeout, typename Out>
    void request(const Query query, Timeout timeout, Out out, const spawn::yield_context &yield) {
        impl::TransactionRequest<Query, Timeout, Out, Ts...>::request(ozoTransaction, query, timeout, out, yield);
    }

    template <typename Query, typename Timeout>
    void execute(const Query query, Timeout timeout, const spawn::yield_context &yield) {
        impl::TransactionExecute<Query, Timeout, Ts...>::execute(ozoTransaction, query, timeout, yield);
    }

    // TODO: rename the 'testing' param, it's not accurate
    template <bool testing = false> auto commit(const spawn::yield_context &yield) {
        wasMovedFrom = true;
        return impl::TransactionCommit<testing, Ts...>::commit(std::move(ozoTransaction), yield);
    }

    // TODO: rename the 'testing' param, it's not accurate
    template <bool testing = false> auto rollback(const spawn::yield_context &yield) {
        wasMovedFrom = true;
        return impl::TransactionRollback<testing, Ts...>::rollback(std::move(ozoTransaction), yield);
    }

    template <bool testing = false> bool isValid() {
        return impl::TransactionIsValid<testing, Ts...>::isValid(wasMovedFrom);
    }

private:
    bool wasMovedFrom = false;
    ozo::impl::transaction<Ts...> ozoTransaction;
};

template <typename... Ts> class TransactionProxy {
public:
    explicit TransactionProxy(ozo::impl::transaction<Ts...> &&ozoTransaction) : transaction(std::move(ozoTransaction)) {
    }

    Transaction<Ts...> *operator->() {
        if (not transaction.isValid())
            throw db::InvalidTransactionOperation{};
        return &transaction;
    }

private:
    Transaction<Ts...> transaction;
};

} // namespace db