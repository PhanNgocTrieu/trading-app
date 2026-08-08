#pragma once
// docs/samples/phase1/auth_app_service.hpp
// Pseudo-implementation: repository interfaces cần bạn viết theo docs/04.

#include "../phase0/result.hpp"
#include <memory>
#include <optional>
#include <string>

struct Session {
    int userId{};
    int accountId{};
    std::string username;
};

struct UserRow {
    int id{};
    std::string username;
    std::string passwordHash;
};

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual std::optional<UserRow> findByUsername(const std::string& username) = 0;
    virtual int insertUser(const std::string& username, const std::string& passwordHash) = 0;
};

class IAccountRepository {
public:
    virtual ~IAccountRepository() = default;
    virtual int createForUser(int userId) = 0;
    virtual int findIdByUserId(int userId) = 0;
};

class IPasswordHasher {
public:
    virtual ~IPasswordHasher() = default;
    virtual std::string hash(const std::string& password) = 0;
    virtual bool verify(const std::string& password, const std::string& passwordHash) = 0;
};

class AuthAppService {
public:
    AuthAppService(std::shared_ptr<IUserRepository> users,
                   std::shared_ptr<IAccountRepository> accounts,
                   std::shared_ptr<IPasswordHasher> hasher)
        : users_(std::move(users))
        , accounts_(std::move(accounts))
        , hasher_(std::move(hasher)) {}

    Result<Session> registerUser(const std::string& username, const std::string& password) {
        if (username.size() < 3 || password.size() < 6) {
            return Result<Session>::fail(ErrorCode::ValidationFailed, "username/password too short");
        }
        if (users_->findByUsername(username)) {
            return Result<Session>::fail(ErrorCode::Conflict, "username taken");
        }

        // NOTE: bọc transaction ở tầng infrastructure/unit-of-work khi integrate.
        const auto hash = hasher_->hash(password);
        const int userId = users_->insertUser(username, hash);
        const int accountId = accounts_->createForUser(userId);

        Session s;
        s.userId = userId;
        s.accountId = accountId;
        s.username = username;
        return Result<Session>::ok(std::move(s));
    }

    Result<Session> login(const std::string& username, const std::string& password) {
        auto user = users_->findByUsername(username);
        if (!user || !hasher_->verify(password, user->passwordHash)) {
            return Result<Session>::fail(ErrorCode::Unauthorized, "invalid username or password");
        }
        Session s;
        s.userId = user->id;
        s.accountId = accounts_->findIdByUserId(user->id);
        s.username = user->username;
        return Result<Session>::ok(std::move(s));
    }

private:
    std::shared_ptr<IUserRepository> users_;
    std::shared_ptr<IAccountRepository> accounts_;
    std::shared_ptr<IPasswordHasher> hasher_;
};
