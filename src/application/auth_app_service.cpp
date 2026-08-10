#include "application/auth_app_service.hpp"

#include "infrastructure/db/transaction.hpp"

#include <exception>

AuthAppService::AuthAppService(SqliteConnection& db,
                               IUserRepository& users,
                               IAccountRepository& accounts,
                               IPasswordHasher& hasher)
    : db_(db)
    , users_(users)
    , accounts_(accounts)
    , hasher_(hasher) {}

Result<AuthSession> AuthAppService::registerUser(const std::string& username,
                                                 const std::string& password,
                                                 const std::string& phoneNumber) {
    if (username.size() < 3 || password.size() < 6) {
        return Result<AuthSession>::fail(ErrorCode::ValidationFailed,
                                         "username/password too short");
    }
    if (users_.findByUsername(username)) {
        return Result<AuthSession>::fail(ErrorCode::Conflict, "username taken");
    }

    try {
        Transaction tx(db_, true);
        const std::string passwordHash = hasher_.hash(password);
        const int userId = users_.insertUser(username, passwordHash, phoneNumber);
        const int accountId = accounts_.createForUser(userId, 0.0);
        tx.commit();

        AuthSession session;
        session.userId = userId;
        session.accountId = accountId;
        session.username = username;
        return Result<AuthSession>::ok(std::move(session));
    } catch (const std::exception& ex) {
        return Result<AuthSession>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<AuthSession> AuthAppService::login(const std::string& username,
                                          const std::string& password) {
    try {
        auto user = users_.findByUsername(username);
        if (!user || !hasher_.verify(password, user->passwordHash)) {
            return Result<AuthSession>::fail(ErrorCode::Unauthorized,
                                             "invalid username or password");
        }
        auto account = accounts_.findByUserId(user->id);
        if (!account) {
            return Result<AuthSession>::fail(ErrorCode::NotFound, "account missing");
        }

        AuthSession session;
        session.userId = user->id;
        session.accountId = account->id;
        session.username = user->username;
        return Result<AuthSession>::ok(std::move(session));
    } catch (const std::exception& ex) {
        return Result<AuthSession>::fail(ErrorCode::DbError, ex.what());
    }
}
