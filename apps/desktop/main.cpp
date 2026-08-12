#include "application/app_bootstrap.hpp"
#include "bridge/trading_app_bridge.hpp"
#include "dto.hpp"

#include <QCommandLineParser>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>
#include <QVector>

#include <iostream>
#include <string>

// Phase 5 entry: Qt Quick (QML) desktop app.
// Composition root — open DB, build TradingAppBridge, expose it as context
// property `app`, then load qrc:/qml/Main.qml.

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("Trading App"));
    QGuiApplication::setApplicationVersion(QStringLiteral("1.0"));
    // Basic style so our custom PrimaryButton / Theme colors stay consistent.
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    // Needed so QSignalSpy / queued connections can carry DTO types.
    qRegisterMetaType<desktop::SessionDto>("desktop::SessionDto");
    qRegisterMetaType<desktop::OrderUiDto>("desktop::OrderUiDto");
    qRegisterMetaType<desktop::PositionUiDto>("desktop::PositionUiDto");
    qRegisterMetaType<desktop::QuoteUiDto>("desktop::QuoteUiDto");
    qRegisterMetaType<QVector<desktop::PositionUiDto>>("QVector<desktop::PositionUiDto>");
    qRegisterMetaType<QVector<desktop::QuoteUiDto>>("QVector<desktop::QuoteUiDto>");

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Paper trading desktop app (QML)"));
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption dbOption(
        QStringList{QStringLiteral("db"), QStringLiteral("database")},
        QStringLiteral("SQLite database file path"),
        QStringLiteral("path"));
    parser.addOption(dbOption);
    parser.process(app);

    const std::string dbPath = parser.isSet(dbOption)
                                   ? parser.value(dbOption).toStdString()
                                   : AppBootstrap::defaultDbPath();

    try {
        desktop::TradingAppBridge bridge(dbPath);

        QQmlApplicationEngine engine;
        // QML refers to this as `app` (e.g. app.login(...), app.quoteModel).
        engine.rootContext()->setContextProperty(QStringLiteral("app"), &bridge);

        const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
        QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
            []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
        engine.load(url);

        if (engine.rootObjects().isEmpty()) {
            std::cerr << "Failed to load QML UI\n";
            return 1;
        }

        return app.exec();
    } catch (const std::exception& ex) {
        std::cerr << "Fatal: " << ex.what() << "\n";
        return 1;
    }
}
