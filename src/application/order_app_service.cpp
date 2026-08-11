#include "application/order_app_service.hpp"

#include "infrastructure/db/transaction.hpp"

#include <cctype>
#include <exception>

namespace {

std::string normalizeSymbol(std::string symbol) {
    for (char& ch : symbol) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return symbol;
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

        auto account = accounts_.findById(session.accountId);
        if (!account) {
            return Result<OrderDto>::fail(ErrorCode::NotFound, "account not found");
        }
        if (account->status != "ACTIVE") {
            return Result<OrderDto>::fail(ErrorCode::ValidationFailed, "account not active");
        }

        auto quote = quotes_.find(symbol);
        if (!quote) {
            // Do not insert a rejected order: symbol FK would fail for unknown tickers.
            return Result<OrderDto>::fail(ErrorCode::NotFound, "symbol not found");
        }

        IncomingOrder incoming{symbol, side, OrderType::Market, qty, 0.0};
        MarketSnapshot snapshot{symbol, quote->lastPrice};
        const MatchResult match = engine_.match(incoming, snapshot);
        if (!match.filled) {
            orders_.insert(account->id, symbol, side, OrderType::Market, qty, std::nullopt,
                           OrderStatus::Rejected);
            tx.commit();
            return Result<OrderDto>::fail(ErrorCode::ValidationFailed, match.rejectReason);
        }

        const double notional = match.fillPrice * static_cast<double>(match.fillQty);
        OrderDto dto;
        dto.symbol = symbol;
        dto.side = side;
        dto.quantity = match.fillQty;
        dto.fillPrice = match.fillPrice;

        if (side == OrderSide::Buy) {
            if (account->cashBalance < notional) {
                const int orderId =
                    orders_.insert(account->id, symbol, side, OrderType::Market, qty, std::nullopt,
                                   OrderStatus::Rejected);
                tx.commit();
                dto.orderId = orderId;
                dto.status = OrderStatus::Rejected;
                return Result<OrderDto>::fail(ErrorCode::InsufficientFunds, "insufficient funds");
            }

            const double nextCash = account->cashBalance - notional;
            Position pos = loadPosition(account->id, symbol);
            auto bought = pos.applyBuy(match.fillQty, match.fillPrice);
            if (!bought.ok()) {
                return Result<OrderDto>::fail(bought.code(), bought.message());
            }

            const int orderId =
                orders_.insert(account->id, symbol, side, OrderType::Market, qty, std::nullopt,
                               OrderStatus::Filled);
            accounts_.updateCash(account->id, nextCash);
            positions_.upsert(account->id, symbol, pos.quantity(), pos.avgCost());
            ledger_.add(account->id, LedgerType::Buy, -notional, nextCash, "order", orderId);
            trades_.insert(orderId, account->id, symbol, side, match.fillQty, match.fillPrice);

            tx.commit();
            dto.orderId = orderId;
            dto.status = OrderStatus::Filled;
            return Result<OrderDto>::ok(std::move(dto));
        }

        // Sell
        Position pos = loadPosition(account->id, symbol);
        auto sold = pos.applySell(match.fillQty, match.fillPrice);
        if (!sold.ok()) {
            const int orderId =
                orders_.insert(account->id, symbol, side, OrderType::Market, qty, std::nullopt,
                               OrderStatus::Rejected);
            tx.commit();
            dto.orderId = orderId;
            dto.status = OrderStatus::Rejected;
            return Result<OrderDto>::fail(sold.code(), sold.message());
        }

        const double nextCash = account->cashBalance + notional;
        const int orderId =
            orders_.insert(account->id, symbol, side, OrderType::Market, qty, std::nullopt,
                           OrderStatus::Filled);
        accounts_.updateCash(account->id, nextCash);
        positions_.upsert(account->id, symbol, pos.quantity(), pos.avgCost());
        ledger_.add(account->id, LedgerType::Sell, notional, nextCash, "order", orderId);
        trades_.insert(orderId, account->id, symbol, side, match.fillQty, match.fillPrice);

        tx.commit();
        dto.orderId = orderId;
        dto.status = OrderStatus::Filled;
        dto.realizedPnl = sold.value();
        return Result<OrderDto>::ok(std::move(dto));
    } catch (const std::exception& ex) {
        return Result<OrderDto>::fail(ErrorCode::DbError, ex.what());
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
        quotes_.setLastPrice(normalizeSymbol(rawSymbol), lastPrice);
        return okResult();
    } catch (const std::exception& ex) {
        return failResult(ErrorCode::DbError, ex.what());
    }
}
