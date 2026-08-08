#ifndef BANK_SERVICE_H
#define BANK_SERVICE_H

#include "domain/account.hpp"
#include "domain/result.hpp"
#include "logger.hpp"

namespace Service {

// In-memory wallet operations against the current Session account.
class BankAccountService {
public:
    explicit BankAccountService(LoggerService& logger);

    [[nodiscard]] Result<double> deposit(double amount);
    [[nodiscard]] Result<double> withdraw(double amount);
    [[nodiscard]] Result<double> balance() const;

private:
    LoggerService& logger_;
};

} // namespace Service

#endif
