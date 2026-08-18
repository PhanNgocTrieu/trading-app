#include "application/order_app_service.hpp"

#include "infrastructure/db/transaction.hpp"

#include <cctype>
#include <exception>
#include <map>
#include <utility>

namespace {

std::string normalizeSymbol(std::string symbol) {
    for (char& ch : symbol) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return symbol;
}

struct PlaceOutcome {
    static std::pair<Result<OrderDto>, bool> ok(OrderDto dto) {
        return {Result<OrderDto>::ok(std::move(dto)), true};
    }

    static std::pair<Result<OrderDto>, bool> fail(ErrorCode code, std::string message, bool persist) {
        return {Result<OrderDto>::fail(code, std::move(message)), persist};
    }
};

OrderDto makeDto(const std::string& symbol,
                 OrderSide side,
                 OrderType type,
                 int qty,
                 std::optional<double> limitPrice) {
    OrderDto dto;
    dto.symbol = symbol;
    dto.side = side;
    dto.type = type;
    dto.quantity = qty;
    dto.limitPrice = limitPrice;
    return dto;
}

} // namespace

OrderAppService::OrderAppService(SqliteConnection& db,
                                 IAccountRepository& accounts,
                                 ILedgerRepository& ledger,
                                 IQuoteRepository& quotes,
                                 IPositionRepository& positions,
                                 IOrderRepository& orders,
                                 ITradeRepository& trades)
    : db_(db)
    , accounts_(accounts)
    , ledger_(ledger)
    , quotes_(quotes)
    , positions_(positions)
    , orders_(orders)
    , trades_(trades) {}

Position OrderAppService::loadPosition(int accountId, const std::string& symbol) const {
    auto row = positions_.find(accountId, symbol);
    if (!row) {
        return Position{symbol, 0, 0.0};
    }
    return Position{row->symbol, row->quantity, row->avgCost};
}

double OrderAppService::reservedBuyNotional(int accountId, int excludeOrderId) const {
    double sum = 0.0;
    for (const auto& order : orders_.listPendingByAccount(accountId)) {
        if (order.id == excludeOrderId || order.side != OrderSide::Buy || !order.limitPrice) {
            continue;
        }
        sum += static_cast<double>(order.quantity) * *order.limitPrice;
    }
    return sum;
}

int OrderAppService::reservedSellQty(int accountId,
                                     const std::string& symbol,
                                     int excludeOrderId) const {
    int qty = 0;
    for (const auto& order : orders_.listPendingByAccount(accountId)) {
        if (order.id == excludeOrderId || order.side != OrderSide::Sell || order.symbol != symbol) {
            continue;
        }
        qty += order.quantity;
    }
    return qty;
}

namespace {

std::pair<Result<OrderDto>, bool> applyFillInTx(IAccountRepository& accounts,
                           ILedgerRepository& ledger,
                           IPositionRepository& positions,
                           IOrderRepository& orders,
                           ITradeRepository& trades,
                           const AccountRow& account,
                           int existingOrderId,
                           const std::string& symbol,
                           OrderSide side,
                           OrderType type,
                           int qty,
                           std::optional<double> limitPrice,
                           const MatchResult& match,
                           double reservedBuys,
                           int reservedSells,
                           Position currentPosition) {
    OrderDto dto = makeDto(symbol, side, type, match.fillQty, limitPrice);
    dto.fillPrice = match.fillPrice;

    const double notional = match.fillPrice * static_cast<double>(match.fillQty);

    auto persistReject = [&](ErrorCode code, const std::string& message) {
        int orderId = existingOrderId;
        if (orderId == 0) {
            orderId = orders.insert(account.id, symbol, side, type, qty, limitPrice,
                                    OrderStatus::Rejected);
        } else {
            orders.updateStatus(orderId, OrderStatus::Rejected);
        }
        dto.orderId = orderId;
        dto.status = OrderStatus::Rejected;
        return PlaceOutcome::fail(code, message, true);
    };

    if (side == OrderSide::Buy) {
        const double availableCash = account.cashBalance - reservedBuys;
        if (availableCash < notional) {
            return persistReject(ErrorCode::InsufficientFunds, "insufficient funds");
        }

        const double nextCash = account.cashBalance - notional;
        auto bought = currentPosition.applyBuy(match.fillQty, match.fillPrice);
        if (!bought.ok()) {
            return PlaceOutcome::fail(bought.code(), bought.message(), false);
        }

        int orderId = existingOrderId;
        if (orderId == 0) {
            orderId = orders.insert(account.id, symbol, side, type, qty, limitPrice,
                                    OrderStatus::Filled);
        } else {
            orders.updateStatus(orderId, OrderStatus::Filled);
        }
        accounts.updateCash(account.id, nextCash);
        positions.upsert(account.id, symbol, currentPosition.quantity(),
                         currentPosition.avgCost());
        ledger.add(account.id, LedgerType::Buy, -notional, nextCash, "order", orderId);
        trades.insert(orderId, account.id, symbol, side, match.fillQty, match.fillPrice);

        dto.orderId = orderId;
        dto.status = OrderStatus::Filled;
        return PlaceOutcome::ok(std::move(dto));
    }

    const int availableQty = currentPosition.quantity() - reservedSells;
    if (match.fillQty > availableQty) {
        return persistReject(ErrorCode::InsufficientPosition, "not enough shares");
    }

    auto sold = currentPosition.applySell(match.fillQty, match.fillPrice);
    if (!sold.ok()) {
        return persistReject(sold.code(), sold.message());
    }

    const double nextCash = account.cashBalance + notional;
    int orderId = existingOrderId;
    if (orderId == 0) {
        orderId = orders.insert(account.id, symbol, side, type, qty, limitPrice,
                                OrderStatus::Filled);
    } else {
        orders.updateStatus(orderId, OrderStatus::Filled);
    }
    accounts.updateCash(account.id, nextCash);
    positions.upsert(account.id, symbol, currentPosition.quantity(), currentPosition.avgCost());
    ledger.add(account.id, LedgerType::Sell, notional, nextCash, "order", orderId);
    trades.insert(orderId, account.id, symbol, side, match.fillQty, match.fillPrice);

    dto.orderId = orderId;
    dto.status = OrderStatus::Filled;
    dto.realizedPnl = sold.value();
    return PlaceOutcome::ok(std::move(dto));
}

} // namespace

std::pair<Result<OrderDto>, bool> OrderAppService::placeOrderInTx(const AuthSession& session,
                                                                  const std::string& symbol,
                                                                  OrderSide side,
                                                                  OrderType type,
                                                                  int qty,
                                                                  std::optional<double> limitPrice) {
    auto account = accounts_.findById(session.accountId);
    if (!account) {
        return PlaceOutcome::fail(ErrorCode::NotFound, "account not found", false);
    }
    if (account->status != "ACTIVE") {
        return PlaceOutcome::fail(ErrorCode::ValidationFailed, "account not active", false);
    }

    auto quote = quotes_.find(symbol);
    if (!quote) {
        return PlaceOutcome::fail(ErrorCode::NotFound, "symbol not found", false);
    }

    IncomingOrder incoming{symbol, side, type, qty, limitPrice.value_or(0.0)};
    MarketSnapshot snapshot{symbol, quote->lastPrice};
    const MatchResult match = engine_.match(incoming, snapshot);

    if (match.filled) {
        return applyFillInTx(accounts_, ledger_, positions_, orders_, trades_, *account, 0, symbol,
                             side, type, qty, limitPrice, match,
                             reservedBuyNotional(account->id, 0),
                             reservedSellQty(account->id, symbol, 0),
                             loadPosition(account->id, symbol));
    }

    if (type == OrderType::Limit) {
        if (side == OrderSide::Buy) {
            const double need = static_cast<double>(qty) * limitPrice.value_or(0.0);
            const double available = account->cashBalance - reservedBuyNotional(account->id, 0);
            if (available < need) {
                orders_.insert(account->id, symbol, side, type, qty, limitPrice,
                               OrderStatus::Rejected);
                return PlaceOutcome::fail(ErrorCode::InsufficientFunds, "insufficient funds",
                                          true);
            }
        } else {
            Position pos = loadPosition(account->id, symbol);
            const int available = pos.quantity() - reservedSellQty(account->id, symbol, 0);
            if (qty > available) {
                orders_.insert(account->id, symbol, side, type, qty, limitPrice,
                               OrderStatus::Rejected);
                return PlaceOutcome::fail(ErrorCode::InsufficientPosition, "not enough shares",
                                          true);
            }
        }

        const int orderId =
            orders_.insert(account->id, symbol, side, type, qty, limitPrice, OrderStatus::Pending);
        OrderDto dto = makeDto(symbol, side, type, qty, limitPrice);
        dto.orderId = orderId;
        dto.status = OrderStatus::Pending;
        return PlaceOutcome::ok(std::move(dto));
    }

    orders_.insert(account->id, symbol, side, type, qty, limitPrice, OrderStatus::Rejected);
    return PlaceOutcome::fail(ErrorCode::ValidationFailed, match.rejectReason, true);
}

void OrderAppService::tryFillPendingInTx(const std::string& symbol) {
    auto quote = quotes_.find(symbol);
    if (!quote) {
        return;
    }
    const MarketSnapshot snapshot{symbol, quote->lastPrice};
    const auto pending = orders_.listPendingBySymbol(symbol);
    for (const auto& order : pending) {
        IncomingOrder incoming{order.symbol, order.side, order.type, order.quantity,
                               order.limitPrice.value_or(0.0)};
        const MatchResult match = engine_.match(incoming, snapshot);
        if (!match.filled) {
            continue;
        }
        auto account = accounts_.findById(order.accountId);
        if (!account || account->status != "ACTIVE") {
            orders_.updateStatus(order.id, OrderStatus::Rejected);
            continue;
        }
        applyFillInTx(accounts_, ledger_, positions_, orders_, trades_, *account, order.id,
                      order.symbol, order.side, order.type, order.quantity, order.limitPrice,
                      match, reservedBuyNotional(order.accountId, order.id),
                      reservedSellQty(order.accountId, order.symbol, order.id),
                      loadPosition(order.accountId, order.symbol));
    }
}

Result<OrderDto> OrderAppService::placeMarketOrder(const AuthSession& session,
                                                   const std::string& rawSymbol,
                                                   OrderSide side,
                                                   int qty) {
    if (qty <= 0) {
        return Result<OrderDto>::fail(ErrorCode::ValidationFailed, "qty must be > 0");
    }
    const std::string symbol = normalizeSymbol(rawSymbol);
    try {
        Transaction tx(db_, true);
        auto out = placeOrderInTx(session, symbol, side, OrderType::Market, qty, std::nullopt);
        if (out.second) {
            tx.commit();
        }
        return out.first;
    } catch (const std::exception& ex) {
        return Result<OrderDto>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<OrderDto> OrderAppService::placeLimitOrder(const AuthSession& session,
                                                  const std::string& rawSymbol,
                                                  OrderSide side,
                                                  int qty,
                                                  double limitPrice) {
    if (qty <= 0) {
        return Result<OrderDto>::fail(ErrorCode::ValidationFailed, "qty must be > 0");
    }
    if (limitPrice <= 0.0) {
        return Result<OrderDto>::fail(ErrorCode::ValidationFailed, "limit price must be > 0");
    }
    const std::string symbol = normalizeSymbol(rawSymbol);
    try {
        Transaction tx(db_, true);
        auto out = placeOrderInTx(session, symbol, side, OrderType::Limit, qty, limitPrice);
        if (out.second) {
            tx.commit();
        }
        return out.first;
    } catch (const std::exception& ex) {
        return Result<OrderDto>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<OkTag> OrderAppService::cancelOrder(const AuthSession& session, int orderId) {
    try {
        Transaction tx(db_, true);
        auto order = orders_.findById(orderId);
        if (!order) {
            return failResult(ErrorCode::NotFound, "order not found");
        }
        if (order->accountId != session.accountId) {
            return failResult(ErrorCode::Unauthorized, "order not found");
        }
        if (order->status != OrderStatus::Pending) {
            return failResult(ErrorCode::ValidationFailed, "only pending orders can be canceled");
        }
        orders_.updateStatus(orderId, OrderStatus::Canceled);
        tx.commit();
        return okResult();
    } catch (const std::exception& ex) {
        return failResult(ErrorCode::DbError, ex.what());
    }
}

Result<std::vector<OrderRow>> OrderAppService::workingOrders(const AuthSession& session) const {
    try {
        return Result<std::vector<OrderRow>>::ok(orders_.listPendingByAccount(session.accountId));
    } catch (const std::exception& ex) {
        return Result<std::vector<OrderRow>>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<OrderBookDto> OrderAppService::orderBook(const std::string& rawSymbol) const {
    const std::string symbol = normalizeSymbol(rawSymbol);
    try {
        OrderBookDto book;
        book.symbol = symbol;
        std::map<double, BookLevelDto, std::greater<double>> bids;
        std::map<double, BookLevelDto, std::greater<double>> asks;
        for (const auto& order : orders_.listPendingBySymbol(symbol)) {
            if (!order.limitPrice) {
                continue;
            }
            auto& ladder = order.side == OrderSide::Buy ? bids : asks;
            auto& level = ladder[*order.limitPrice];
            level.side = order.side;
            level.price = *order.limitPrice;
            level.quantity += order.quantity;
            ++level.orderCount;
        }
        for (auto& [price, level] : bids) {
            (void)price;
            book.bids.push_back(level);
        }
        for (auto& [price, level] : asks) {
            (void)price;
            book.asks.push_back(level);
        }
        return Result<OrderBookDto>::ok(std::move(book));
    } catch (const std::exception& ex) {
        return Result<OrderBookDto>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<std::vector<QuoteRow>> OrderAppService::listQuotes() const {
    try {
        return Result<std::vector<QuoteRow>>::ok(quotes_.listAll());
    } catch (const std::exception& ex) {
        return Result<std::vector<QuoteRow>>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<std::vector<PositionDto>> OrderAppService::portfolio(const AuthSession& session) const {
    try {
        auto rows = positions_.listByAccount(session.accountId);
        std::vector<PositionDto> out;
        out.reserve(rows.size());
        for (const auto& row : rows) {
            PositionDto dto;
            dto.symbol = row.symbol;
            dto.quantity = row.quantity;
            dto.avgCost = row.avgCost;
            auto quote = quotes_.find(row.symbol);
            dto.marketPrice = quote ? quote->lastPrice : 0.0;
            dto.marketValue = dto.marketPrice * static_cast<double>(dto.quantity);
            dto.unrealizedPnl =
                (dto.marketPrice - dto.avgCost) * static_cast<double>(dto.quantity);
            out.push_back(std::move(dto));
        }
        return Result<std::vector<PositionDto>>::ok(std::move(out));
    } catch (const std::exception& ex) {
        return Result<std::vector<PositionDto>>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<std::vector<TradeRow>> OrderAppService::recentTrades(const AuthSession& session,
                                                            int limit) const {
    try {
        return Result<std::vector<TradeRow>>::ok(trades_.listRecent(session.accountId, limit));
    } catch (const std::exception& ex) {
        return Result<std::vector<TradeRow>>::fail(ErrorCode::DbError, ex.what());
    }
}

Result<OkTag> OrderAppService::setQuotePrice(const std::string& rawSymbol, double lastPrice) {
    if (lastPrice <= 0.0) {
        return failResult(ErrorCode::ValidationFailed, "price must be > 0");
    }
    try {
        Transaction tx(db_, true);
        const std::string symbol = normalizeSymbol(rawSymbol);
        quotes_.setLastPrice(symbol, lastPrice);
        tryFillPendingInTx(symbol);
        tx.commit();
        return okResult();
    } catch (const std::exception& ex) {
        return failResult(ErrorCode::DbError, ex.what());
    }
}
