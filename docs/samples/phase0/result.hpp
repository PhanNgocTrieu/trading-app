#pragma once
// docs/samples/phase0/result.hpp
// Copy vào include/core/result.hpp (hoặc tương đương) ở Phase 0.

#include <string>
#include <utility>

enum class ErrorCode {
    Ok = 0,
    NotFound,
    ValidationFailed,
    InsufficientFunds,
    InsufficientPosition,
    Unauthorized,
    Conflict,
    DbError,
    Internal
};

template <typename T>
class Result {
public:
    static Result ok(T value) {
        Result r;
        r.code_ = ErrorCode::Ok;
        r.value_ = std::move(value);
        return r;
    }

    static Result fail(ErrorCode code, std::string message) {
        Result r;
        r.code_ = code;
        r.message_ = std::move(message);
        return r;
    }

    bool ok() const { return code_ == ErrorCode::Ok; }
    ErrorCode code() const { return code_; }
    const std::string& message() const { return message_; }
    const T& value() const { return value_; }
    T& value() { return value_; }

private:
    ErrorCode code_{ErrorCode::Internal};
    T value_{};
    std::string message_;
};

// Chuyên biệt cho thao tác không trả data
struct OkTag {};

inline Result<OkTag> okResult() { return Result<OkTag>::ok(OkTag{}); }

inline Result<OkTag> failResult(ErrorCode code, std::string message) {
    return Result<OkTag>::fail(code, std::move(message));
}
