#pragma once

#include "core/CoreErrors.h"

namespace comm {

constexpr int commErrorCategory = 2;

enum class CommErrorCodes : int {
    NoError = 0,
    CorruptedHeaderXmlTag = 10,
    CorruptedHeaderLength = 11,
    ParsingRequestXmlTree = 20,
    ParsingRequestData = 21,
    ParsingEmptyDocument = 22,
    OperationAborted = 30,
    LostConnection = 31,
    Unknown = 32,

};

struct CommError : core::Error {
    explicit CommError(int code) : Error(commErrorCategory, code) {
    }
};

struct CorruptedHeaderXmlTagError : CommError, core::DropConnection {
    CorruptedHeaderXmlTagError() : CommError(static_cast<core::ErrorCodeType>(CommErrorCodes::CorruptedHeaderXmlTag)) {
    }
};

struct CorruptedHeaderLengthError : CommError, core::AbortTransaction {
    CorruptedHeaderLengthError() : CommError(static_cast<core::ErrorCodeType>(CommErrorCodes::CorruptedHeaderLength)) {
    }
};

struct OperationAbortedError : CommError, core::DropConnection {
    OperationAbortedError() : CommError(static_cast<core::ErrorCodeType>(CommErrorCodes::OperationAborted)) {
    }
};

struct ParsingRequestXmlTreeError : CommError, core::AbortTransaction {
    ParsingRequestXmlTreeError() : CommError(static_cast<core::ErrorCodeType>(CommErrorCodes::ParsingRequestXmlTree)) {
    }
};

struct ParsingRequestDataError : CommError, core::AbortTransaction {
    ParsingRequestDataError() : CommError(static_cast<core::ErrorCodeType>(CommErrorCodes::ParsingRequestData)) {
    }
};

struct LostConnectionError : CommError, core::DropConnection {
    LostConnectionError() : CommError(static_cast<core::ErrorCodeType>(CommErrorCodes::LostConnection)) {
    }
};

struct UnknownConnectionError : CommError, core::DropConnection {
    UnknownConnectionError() : CommError(static_cast<core::ErrorCodeType>(CommErrorCodes::Unknown)) {
    }
};

struct ParsingEmptyDocument : CommError, core::AbortTransaction {
    ParsingEmptyDocument() : CommError(static_cast<core::ErrorCodeType>(CommErrorCodes::ParsingEmptyDocument)) {
    }
};

} // namespace comm
