#pragma once

#include <string>

// Identity / login profile only. Cash belongs to Account (see docs/phase0-notes.md).
class User {
public:
    User(int id, std::string username, std::string phoneNumber = "")
        : id_(id)
        , username_(std::move(username))
        , phoneNumber_(std::move(phoneNumber)) {}

    int id() const { return id_; }
    const std::string& username() const { return username_; }
    const std::string& phoneNumber() const { return phoneNumber_; }

    void rename(std::string newName) { username_ = std::move(newName); }
    void setPhoneNumber(std::string phoneNumber) { phoneNumber_ = std::move(phoneNumber); }

private:
    int id_;
    std::string username_;
    std::string phoneNumber_;
};
