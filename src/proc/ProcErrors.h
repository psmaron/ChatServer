#pragma once

#include "core/CoreErrors.h"

namespace proc::trx {

constexpr int procErrorCategory = 3;

enum class ProcErrorCodes : core::ErrorCodeType {
    NoError = 0,
    UnhandledRequest = 1,
    IdenticalStatus = 2,
    ReceivedGenericClose = 3,
    InvalidClientId = 4,
    CustomDataNotProvided = 5,
    ClientOutOfRange = 6
};

struct ProcError : core::Error {
    explicit ProcError(int code) : Error(procErrorCategory, code) {
    }
};

struct InvalidClientIdError : ProcError, core::AbortTransaction {
    InvalidClientIdError() : ProcError(static_cast<core::ErrorCodeType>(ProcErrorCodes::InvalidClientId)) {
    }
};

struct IdenticalStatusError : ProcError, core::AbortTransaction {
    IdenticalStatusError() : ProcError(static_cast<core::ErrorCodeType>(ProcErrorCodes::IdenticalStatus)) {
    }
};

struct ReceivedGenericCloseError : ProcError, core::AbortTransaction {
    ReceivedGenericCloseError() : ProcError(static_cast<core::ErrorCodeType>(ProcErrorCodes::ReceivedGenericClose)) {
    }
};

struct UnhandledRequestError : ProcError, core::AbortTransaction {
    UnhandledRequestError() : ProcError(static_cast<core::ErrorCodeType>(ProcErrorCodes::UnhandledRequest)) {
    }
};

struct CustomDataNotProvidedError : ProcError, core::AbortTransaction {
    CustomDataNotProvidedError() : ProcError(static_cast<core::ErrorCodeType>(ProcErrorCodes::CustomDataNotProvided)) {
    }
};

struct ClientOutOfRangeError : ProcError, core::AbortTransaction {
    ClientOutOfRangeError() : ProcError(static_cast<core::ErrorCodeType>(ProcErrorCodes::ClientOutOfRange)) {
    }
};

} // namespace proc::trx