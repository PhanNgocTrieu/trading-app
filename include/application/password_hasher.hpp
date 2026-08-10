#pragma once

#include "application/ports.hpp"

// Learning hasher: v1$<salt_hex>$<sha256_hex(salt || password)>
// Prefer Argon2/bcrypt for real production systems.
class SimplePasswordHasher final : public IPasswordHasher {
public:
    std::string hash(const std::string& password) override;
    bool verify(const std::string& password, const std::string& passwordHash) override;
};
