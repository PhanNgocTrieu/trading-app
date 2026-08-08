#ifndef SERVICE_H
#define SERVICE_H

#include "domain/order_types.hpp"

#include <string>

namespace Service {

// Thin façade kept for future application-layer wiring (Phase 1+).
class ServiceAPI {
public:
    static ServiceAPI& getInstance() {
        static ServiceAPI instance;
        return instance;
    }

    void requestLogin(const std::string& username, const std::string& password);
    void requestLogout(const std::string& username);
    void requestWithdrawal(const std::string& accountNumber, double amount);
    void requestDeposit(const std::string& accountNumber, double amount);
    void requestTrading(const std::string& stockSymbol, int quantity, double price,
                        OrderSide side = OrderSide::Buy);

private:
    ServiceAPI() = default;
};

} // namespace Service

#endif
