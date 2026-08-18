#pragma once

#include "application/app_bootstrap.hpp"
#include "application/ports.hpp"
#include "domain/result.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <string>

// In-memory app graph for unit tests. Call *AppService directly — no console adapters.
class AppFixture : public ::testing::Test {
protected:
    void SetUp() override {
        app_ = std::make_unique<AppBootstrap>(AppBootstrap::open(":memory:"));
    }

    void TearDown() override {
        logout();
        app_.reset();
    }

    [[nodiscard]] Result<AuthSession> registerUser(const std::string& username,
                                                   const std::string& password) {
        auto result = app_->auth().registerUser(username, password);
        if (result.ok()) {
            session_ = result.value();
            loggedIn_ = true;
        }
        return result;
    }

    [[nodiscard]] Result<AuthSession> login(const std::string& username,
                                            const std::string& password) {
        auto result = app_->auth().login(username, password);
        if (result.ok()) {
            session_ = result.value();
            loggedIn_ = true;
        }
        return result;
    }

    void logout() {
        session_ = AuthSession{};
        loggedIn_ = false;
    }

    std::unique_ptr<AppBootstrap> app_;
    AuthSession session_{};
    bool loggedIn_{false};
};

// Named suites so `--gtest_filter=PhaseN*` stays useful (all share AppFixture).
class Phase0AuthTest : public AppFixture {};
class Phase0WalletTest : public AppFixture {};
class Phase1AuthWalletTest : public AppFixture {};
class Phase2OrderFlowTest : public AppFixture {};
class Phase6LimitOrderTest : public AppFixture {};
