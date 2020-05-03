#pragma once

#include "core/CoreErrors.h"

namespace db {

constexpr int databaseErrorCategory = 4;

enum class DatabaseErrorCodes : int {
    NoError = 0,
    DatabaseConnectionError = 1,
    DatabaseSqlError = 2,
    DatabaseUnknownError = 3,
    InvalidTransactionOperation = 4
};

struct DatabaseError : core::Error {
    explicit DatabaseError(int code) : Error(databaseErrorCategory, code) {
    }

    const char *what() const noexcept override {
        return "DatabaseError";
    }
};

struct DatabaseConnectionError : DatabaseError {
    explicit DatabaseConnectionError()
        : DatabaseError(static_cast<core::ErrorCodeType>(DatabaseErrorCodes::DatabaseConnectionError)) {
    }

    const char *what() const noexcept override {
        return "DatabaseConnectionError";
    }
};

struct DatabaseSqlError : DatabaseError {
    explicit DatabaseSqlError() : DatabaseError(static_cast<core::ErrorCodeType>(DatabaseErrorCodes::DatabaseSqlError)) {
    }
    const char *what() const noexcept override {
        return "DatabaseSqlError";
    }
};

struct DatabaseUnknownError : DatabaseError {
    explicit DatabaseUnknownError() : DatabaseError(static_cast<core::ErrorCodeType>(DatabaseErrorCodes::DatabaseUnknownError)) {
    }
    const char *what() const noexcept override {
        return "DatabaseUnknownError";
    }
};

struct InvalidTransactionOperation : DatabaseError {
    explicit InvalidTransactionOperation()
        : DatabaseError(static_cast<core::ErrorCodeType>(DatabaseErrorCodes::InvalidTransactionOperation)) {
    }
    const char *what() const noexcept override {
        return "Transaction used after being committed/rolled-back";
    }
};

} // namespace db
