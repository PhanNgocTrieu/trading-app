#pragma once
// docs/samples/phase4/mock_market_feed.hpp
// QTimer-based mock feed. Persist giá qua repository trong slot.

#include <QObject>
#include <QTimer>
#include <functional>
#include <random>
#include <string>
#include <vector>

class MockMarketDataFeed : public QObject {
    Q_OBJECT
public:
    using UpdateFn = std::function<void(const std::string& symbol, double newPrice)>;

    explicit MockMarketDataFeed(QObject* parent = nullptr)
        : QObject(parent) {
        connect(&timer_, &QTimer::timeout, this, &MockMarketDataFeed::onTick);
    }

    void setSymbols(std::vector<std::string> symbols) { symbols_ = std::move(symbols); }
    void setUpdateHandler(UpdateFn fn) { updateFn_ = std::move(fn); }

    void start(int intervalMs = 1000) { timer_.start(intervalMs); }
    void stop() { timer_.stop(); }

signals:
    void quotesUpdated();

private slots:
    void onTick() {
        for (const auto& symbol : symbols_) {
            // random walk ±0.5%
            std::uniform_real_distribution<double> dist(-0.005, 0.005);
            const double shock = dist(rng_);
            // NOTE: cần giá hiện tại từ repo; ở đây giả sử updateFn tự đọc-modify-write.
            if (updateFn_) {
                // Convention: updateFn nhận shock thay vì absolute — hoặc tự viết overload.
                // Để rõ ràng khi integrate: truyền symbol + multiplier.
                updateFn_(symbol, 1.0 + shock);
            }
        }
        emit quotesUpdated();
    }

private:
    QTimer timer_;
    std::vector<std::string> symbols_;
    UpdateFn updateFn_;
    std::mt19937 rng_{std::random_device{}()};
};
