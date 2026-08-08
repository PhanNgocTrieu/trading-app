#include "service.h"

namespace Service {

void ServiceAPI::requestLogin(const std::string& /*username*/, const std::string& /*password*/) {
    // Phase 1+: delegate to AuthAppService
}

void ServiceAPI::requestLogout(const std::string& /*username*/) {
}

void ServiceAPI::requestWithdrawal(const std::string& /*accountNumber*/, double /*amount*/) {
}

void ServiceAPI::requestDeposit(const std::string& /*accountNumber*/, double /*amount*/) {
}

void ServiceAPI::requestTrading(const std::string& /*stockSymbol*/, int /*quantity*/,
                                double /*price*/, OrderSide /*side*/) {
    // Phase 2: OrderAppService + MatchingEngine
}

} // namespace Service
