#pragma once

#include <iostream>

namespace core {

using ErrorCodeType = signed int;

struct Error : std::exception {
    ErrorCodeType getErrorCode() const noexcept {
        return category * 1000 + code;
    }

protected:
    explicit Error(const ErrorCodeType category = -1, ErrorCodeType code = 0) : category(category), code(code) {
    }

    const ErrorCodeType category;
    ErrorCodeType code;
    // TODO: XXYYZZZ, where: XX - category, YY - subcategory, ZZZ - error code
    // TODO: consider introducing subcategories to further granulate error codes
    // TODO: consider reflecting severity of errors in their inheritance hierarchy
};

struct AbortTransaction : Error {
    const char *what() const noexcept override {
        return "AbortTransaction";
    }
};
struct DropConnection : Error {
    const char *what() const noexcept override {
        return "DropConnection";
    }
};

static int coreErrorCategory = 1;

struct CoreError : Error {
    explicit CoreError(int code) : Error(coreErrorCategory, code) {
    }

    const char *what() const noexcept override {
        return "CoreError";
    }
};

enum class CoreErrorCodes : ErrorCodeType {
    NoError = 0,
};

} // namespace core
