#include "market/mock_market_data_feed.hpp"

#include <algorithm>
#include <cmath>

namespace desktop {

MockMarketDataFeed::MockMarketDataFeed(QObject* parent)
    : QObject(parent) {
    connect(&timer_, &QTimer::timeout, this, &MockMarketDataFeed::onTick);
}

void MockMarketDataFeed::setSymbols(std::vector<std::string> symbols) {
    symbols_ = std::move(symbols);
}

void MockMarketDataFeed::setPriceIO(PriceReader reader, PriceWriter writer) {
    reader_ = std::move(reader);
    writer_ = std::move(writer);
}

void MockMarketDataFeed::start(int intervalMs) {
    if (intervalMs < 50) {
        intervalMs = 50;
    }
    timer_.start(intervalMs);
}

void MockMarketDataFeed::stop() {
    timer_.stop();
}

bool MockMarketDataFeed::isActive() const {
    return timer_.isActive();
}

void MockMarketDataFeed::tickOnce() {
    applyTick();
}

void MockMarketDataFeed::setSeed(unsigned seed) {
    rng_.seed(seed);
}

void MockMarketDataFeed::onTick() {
    applyTick();
}

void MockMarketDataFeed::applyTick() {
    if (!reader_ || !writer_ || symbols_.empty()) {
        return;
    }

    std::uniform_real_distribution<double> dist(-0.005, 0.005);
    for (const auto& symbol : symbols_) {
        auto current = reader_(symbol);
        if (!current || *current <= 0.0) {
            continue;
        }
        const double shock = dist(rng_);
        double next = *current * (1.0 + shock);
        next = std::max(0.01, next);
        // Keep two-decimal display friendly without forcing exact cents domain.
        next = std::round(next * 100.0) / 100.0;
        if (next < 0.01) {
            next = 0.01;
        }
        writer_(symbol, next);
    }

    ++tickCount_;
    emit quotesUpdated();
}

} // namespace desktop
