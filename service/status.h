#ifndef STATUS_SERVICE_H
#define STATUS_SERVICE_H

enum class LoginStatus {
    Success,
    Failure,
    LogoutSuccess,
    LogoutFailure
};

enum class AccountStatus {
    Created,
    Deleted,
    NotFound,
    InsufficientFunds
};

enum class TransactionStatus {
    Success,
    Failure
};

#endif
