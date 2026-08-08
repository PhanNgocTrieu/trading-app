#pragma once

#include "domain/account.hpp"
#include "domain/user.hpp"

#include <optional>

// In-memory session for Phase 0 (no DB persistence yet).
struct Session {
    User user;
    Account account;

    Session(User u, Account a)
        : user(std::move(u)), account(std::move(a)) {}
};

inline std::optional<Session>& currentSession() {
    static std::optional<Session> session;
    return session;
}
