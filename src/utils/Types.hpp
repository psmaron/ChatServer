#pragma once
#include "comm/trx/Transaction.h"

namespace utils {
template <typename Base, typename T> inline bool isInstanceOf(const std::unique_ptr<T> &object) {
    return dynamic_cast<const Base *>(object.get()) != nullptr;
}

template <class T> bool isRequestTypeOf(std::unique_ptr<comm::trx::Transaction> &transaction) {
    return (typeid(*transaction->request).hash_code() == typeid(T).hash_code());
}
} // namespace utils
