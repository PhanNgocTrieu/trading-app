#include "application/password_hasher.hpp"

#include "infrastructure/crypto/sha256.hpp"

#include <random>

namespace {

std::string randomSaltHex(std::size_t bytes = 16) {
    static thread_local std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(0, 255);
    static constexpr char kHex[] = "0123456789abcdef";
    std::string out;
    out.resize(bytes * 2);
    for (std::size_t i = 0; i < bytes; ++i) {
        const int value = dist(rng);
        out[i * 2] = kHex[(value >> 4) & 0xf];
        out[i * 2 + 1] = kHex[value & 0xf];
    }
    return out;
}

} // namespace

std::string SimplePasswordHasher::hash(const std::string& password) {
    const std::string salt = randomSaltHex();
    const std::string digest = crypto::sha256Hex(salt + password);
    return "v1$" + salt + "$" + digest;
}

bool SimplePasswordHasher::verify(const std::string& password, const std::string& passwordHash) {
    // expected: v1$<salt>$<digest>
    if (passwordHash.rfind("v1$", 0) != 0) {
        return false;
    }
    const auto first = passwordHash.find('$', 3);
    if (first == std::string::npos) {
        return false;
    }
    const std::string salt = passwordHash.substr(3, first - 3);
    const std::string expected = passwordHash.substr(first + 1);
    if (salt.empty() || expected.empty()) {
        return false;
    }
    const std::string actual = crypto::sha256Hex(salt + password);
    return actual == expected;
}
