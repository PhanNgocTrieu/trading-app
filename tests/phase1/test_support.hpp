#pragma once

#include "application/app_bootstrap.hpp"
#include "bank.h"
#include "domain/session.hpp"
#include "logger.hpp"
#include "login.h"

#include <gtest/gtest.h>
#include <memory>

class Phase1Fixture : public ::testing::Test {
protected:
    void SetUp() override {
        resetCurrentSession();
        app_ = std::make_unique<AppBootstrap>(AppBootstrap::open(":memory:"));
        logger_ = &Service::LoggerService::getInstance();
        login_ = std::make_unique<Service::LoginService>(*logger_, app_->auth(), app_->accounts());
        bank_ = std::make_unique<Service::BankAccountService>(*logger_, app_->wallet(), *login_);
    }

    void TearDown() override {
        bank_.reset();
        login_.reset();
        app_.reset();
        resetCurrentSession();
    }

    std::unique_ptr<AppBootstrap> app_;
    Service::LoggerService* logger_{nullptr};
    std::unique_ptr<Service::LoginService> login_;
    std::unique_ptr<Service::BankAccountService> bank_;
};
