#pragma once

#include <QObject>
#include <QTimer>

#include <functional>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace desktop {

// Paper-trading price feed (Phase 4).
// Every timer tick: for each symbol, lastPrice *= (1 + random[-0.5%, +0.5%]),
// persist via PriceWriter, then emit quotesUpdated() so the UI can refresh.
class MockMarketDataFeed : public QObject {
    Q_OBJECT
public:
    using PriceReader = std::function<std::optional<double>(const std::string& symbol)>;
    using PriceWriter = std::function<void(const std::string& symbol, double newPrice)>;

    explicit MockMarketDataFeed(QObject* parent = nullptr);

    void setSymbols(std::vector<std::string> symbols);
    void setPriceIO(PriceReader reader, PriceWriter writer);

    void start(int intervalMs = 1000);
    void stop();
    bool isActive() const;

    // Deterministic helper for tests / manual step.
    void tickOnce();
    int tickCount() const { return tickCount_; }

    // Optional: inject RNG seed for reproducible tests.
    void setSeed(unsigned seed);

signals:
    void quotesUpdated();

private slots:
    void onTick();

private:
    void applyTick();

    QTimer timer_;
    std::vector<std::string> symbols_;
    PriceReader reader_;
    PriceWriter writer_;
    std::mt19937 rng_{std::random_device{}()};
    int tickCount_{0};
};

} // namespace desktop
