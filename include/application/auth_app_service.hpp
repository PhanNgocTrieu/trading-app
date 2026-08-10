#pragma once

#include "application/ports.hpp"
#include "domain/result.hpp"
#include "infrastructure/db/sqlite_connection.hpp"

class AuthAppService {
public:
    AuthAppService(SqliteConnection& db,
                   IUserRepository& users,
                   IAccountRepository& accounts,
                   IPasswordHasher& hasher);

    [[nodiscard]] Result<AuthSession> registerUser(const std::string& username,
                                                   const std::string& password,
                                                   const std::string& phoneNumber = "");

    [[nodiscard]] Result<AuthSession> login(const std::string& username,
                                            const std::string& password);

private:
    SqliteConnection& db_;
    IUserRepository& users_;
    IAccountRepository& accounts_;
    IPasswordHasher& hasher_;
};
