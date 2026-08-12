import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root
    property var theme
    property var appBridge

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme.spaceLg
        spacing: theme.spaceMd

        RowLayout {
            Layout.fillWidth: true
            spacing: theme.spaceMd

            ColumnLayout {
                spacing: 2
                Text {
                    text: "Trading App"
                    font: theme.titleFont
                    color: theme.textPrimary
                }
                Text {
                    text: appBridge ? ("Signed in as " + appBridge.username) : ""
                    font: theme.bodyFont
                    color: theme.textSecondary
                }
            }

            Item { Layout.fillWidth: true }

            MetricChip {
                id: cashChip
                theme: root.theme
                label: "Cash"
                value: appBridge ? Number(appBridge.cash).toFixed(2) : "-"
            }
            MetricChip {
                id: equityChip
                theme: root.theme
                label: "Equity"
                value: appBridge ? Number(appBridge.equity).toFixed(2) : "-"
            }
            MetricChip {
                id: pnlChip
                theme: root.theme
                label: "uPnL"
                value: {
                    if (!appBridge)
                        return "-"
                    const v = appBridge.unrealizedPnl
                    return (v >= 0 ? "+" : "") + Number(v).toFixed(2)
                }
                valueColor: !appBridge
                            ? theme.textPrimary
                            : (appBridge.unrealizedPnl >= 0 ? theme.success : theme.danger)
            }

            PrimaryButton {
                theme: root.theme
                text: appBridge && appBridge.feedActive ? "Stop Feed" : "Start Feed"
                onClicked: {
                    if (appBridge)
                        appBridge.setFeedActive(!(appBridge.feedActive))
                }
            }
            PrimaryButton {
                theme: root.theme
                text: "Refresh"
                onClicked: if (appBridge) appBridge.refresh()
            }
            PrimaryButton {
                theme: root.theme
                destructive: true
                text: "Logout"
                onClicked: if (appBridge) appBridge.logout()
            }
        }

        StatusToast {
            Layout.fillWidth: true
            theme: root.theme
            message: appBridge ? appBridge.statusMessage : ""
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: theme.spaceMd

            QuotePanel {
                theme: root.theme
                model: appBridge ? appBridge.quoteModel : null
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 1
            }

            OrderTicket {
                theme: root.theme
                appBridge: root.appBridge
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 1
            }

            PortfolioPanel {
                theme: root.theme
                model: appBridge ? appBridge.positionModel : null
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 1
            }
        }

        Text {
            Layout.fillWidth: true
            text: appBridge ? ("DB: " + appBridge.dbPath) : ""
            elide: Text.ElideMiddle
            color: theme.textSecondary
            font: theme.monoFont
        }
    }

    Connections {
        target: appBridge
        function onMetricsFlashed() {
            pnlChip.playFlash()
        }
    }
}
