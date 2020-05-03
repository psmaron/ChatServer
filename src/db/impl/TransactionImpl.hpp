#pragma once

#include <spawn/spawn.hpp>

#include <ozo/execute.h>
#include <ozo/request.h>
#include <ozo/transaction.h>

#include <log4cpp/Category.hh>

#include "db/impl/ErrorHandler.hpp"

namespace db {

namespace impl {

template <typename Query, typename Timeout, typename Out, typename... Ts> class TransactionRequest {
public:
    static void request(ozo::impl::transaction<Ts...> &ozoTransaction, const Query query, Timeout timeout, Out out,
                        const spawn::yield_context &yield) {
        ozo::error_code ec;
        auto connection = ozo::request(ozoTransaction, query, timeout, out, yield[ec]);
        handleError(ec, connection);
    }
};

template <typename Query, typename Timeout, typename... Ts> class TransactionExecute {
public:
    static void execute(ozo::impl::transaction<Ts...> &ozoTransaction, const Query query, Timeout timeout,
                        const spawn::yield_context &yield) {
        ozo::error_code ec;
        auto connection = ozo::execute(ozoTransaction, query, timeout, yield[ec]);
        handleError(ec, connection);
    }
};

template <bool fake = false, typename... Ts> class TransactionCommit {
public:
    static auto commit(ozo::impl::transaction<Ts...> &&ozoTransaction, const spawn::yield_context &yield) {
        return ozo::commit(std::move(ozoTransaction), yield);
    }
};

template <bool fake = false, typename... Ts> class TransactionRollback {
public:
    static auto rollback(ozo::impl::transaction<Ts...> &&ozoTransaction, const spawn::yield_context &yield) {
        return ozo::rollback(std::move(ozoTransaction), yield);
    }
};

template <bool fake = false, typename... Ts> class TransactionIsValid {
public:
    static bool isValid(bool wasMovedFrom) {
        return !wasMovedFrom;
    }
};

} // namespace impl
} // namespace db