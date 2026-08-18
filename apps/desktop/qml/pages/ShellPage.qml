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
        anchors.margins: 16
        spacing: 12

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            RowLayout {
                spacing: 10
                Layout.minimumWidth: 180
                Rectangle {
                    width: 36
                    height: 36
                    radius: 11
                    color: theme ? theme.accent : "#2DD4BF"
                    Text {
                        anchors.centerIn: parent
                        text: appBridge && appBridge.username.length
                              ? appBridge.username.charAt(0).toUpperCase()
                              : "T"
                        color: theme ? theme.accentInk : "#042F2E"
                        font: theme ? theme.headingFont : font
                    }
                }
                ColumnLayout {
                    spacing: 1
                    Text {
                        text: "Aurora Desk"
                        font: theme ? theme.headingFont : font
                        color: theme ? theme.textPrimary : "#EAF0FA"
                    }
                    Text {
                        text: appBridge ? appBridge.username : ""
                        font: theme ? theme.bodyFont : font
                        color: theme ? theme.textSecondary : "#8FA3C0"
                        elide: Text.ElideRight
                        Layout.maximumWidth: 160
                    }
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: 8
                MetricChip {
                    id: cashChip
                    theme: root.theme
                    label: "CASH"
                    value: appBridge ? Number(appBridge.cash).toFixed(2) : "-"
                }
                MetricChip {
                    id: equityChip
                    theme: root.theme
                    label: "EQUITY"
                    value: appBridge ? Number(appBridge.equity).toFixed(2) : "-"
                }
                MetricChip {
                    id: pnlChip
                    theme: root.theme
                    label: "U-PNL"
                    value: {
                        if (!appBridge)
                            return "-"
                        const v = appBridge.unrealizedPnl
                        return (v >= 0 ? "+" : "") + Number(v).toFixed(2)
                    }
                    valueColor: !appBridge
                                ? (theme ? theme.textPrimary : "#EAF0FA")
                                : (appBridge.unrealizedPnl >= 0
                                   ? (theme ? theme.success : "#34D399")
                                   : (theme ? theme.danger : "#FB7185"))
                }
            }

            RowLayout {
                spacing: 8
                GhostButton {
                    theme: root.theme
                    active: appBridge && appBridge.feedActive
                    text: appBridge && appBridge.feedActive ? "● Live" : "○ Feed"
                    onClicked: {
                        if (appBridge)
                            appBridge.setFeedActive(!(appBridge.feedActive))
                    }
                }
                GhostButton {
                    theme: root.theme
                    text: "Refresh"
                    onClicked: if (appBridge) appBridge.refresh()
                }
                GhostButton {
                    theme: root.theme
                    destructive: true
                    text: "Logout"
                    onClicked: if (appBridge) appBridge.logout()
                }
            }
        }

        StatusToast {
            Layout.fillWidth: true
            theme: root.theme
            message: appBridge ? appBridge.statusMessage : ""
        }

        SplitView {
            id: workspace
            Layout.fillWidth: true
            Layout.fillHeight: true
            orientation: Qt.Vertical
            handle: Rectangle {
                implicitHeight: 8
                color: "transparent"
                Rectangle {
                    anchors.centerIn: parent
                    width: 44
                    height: 3
                    radius: 2
                    color: theme ? theme.border : "#334766"
                }
            }

            SplitView {
                SplitView.fillHeight: true
                SplitView.minimumHeight: 280
                orientation: Qt.Horizontal
                handle: Rectangle {
                    implicitWidth: 8
                    color: "transparent"
                    Rectangle {
                        anchors.centerIn: parent
                        width: 3
                        height: 44
                        radius: 2
                        color: theme ? theme.border : "#334766"
                    }
                }

                QuotePanel {
                    theme: root.theme
                    model: appBridge ? appBridge.quoteModel : null
                    SplitView.preferredWidth: workspace.width * 0.34
                    SplitView.minimumWidth: 240
                    onSymbolActivated: function(symbol) {
                        ticket.selectSymbol(symbol)
                        if (appBridge)
                            appBridge.setBookSymbol(symbol)
                    }
                }

                OrderTicket {
                    id: ticket
                    theme: root.theme
                    appBridge: root.appBridge
                    SplitView.preferredWidth: 340
                    SplitView.minimumWidth: 280
                    onSelectedSymbolChanged: {
                        if (appBridge)
                            appBridge.setBookSymbol(selectedSymbol)
                    }
                }

                PortfolioPanel {
                    theme: root.theme
                    model: appBridge ? appBridge.positionModel : null
                    SplitView.minimumWidth: 240
                    SplitView.fillWidth: true
                }
            }

            SplitView {
                SplitView.preferredHeight: 250
                SplitView.minimumHeight: 170
                orientation: Qt.Horizontal
                handle: Rectangle {
                    implicitWidth: 8
                    color: "transparent"
                    Rectangle {
                        anchors.centerIn: parent
                        width: 3
                        height: 44
                        radius: 2
                        color: theme ? theme.border : "#334766"
                    }
                }

                OrderBookPanel {
                    theme: root.theme
                    symbol: appBridge ? appBridge.bookSymbol : ""
                    bidModel: appBridge ? appBridge.bidModel : null
                    askModel: appBridge ? appBridge.askModel : null
                    SplitView.preferredWidth: workspace.width * 0.38
                    SplitView.minimumWidth: 260
                }

                WorkingOrdersPanel {
                    theme: root.theme
                    appBridge: root.appBridge
                    model: appBridge ? appBridge.workingOrderModel : null
                    SplitView.fillWidth: true
                    SplitView.minimumWidth: 280
                }
            }
        }

        Text {
            Layout.fillWidth: true
            text: appBridge ? ("DB  " + appBridge.dbPath) : ""
            elide: Text.ElideMiddle
            color: theme ? theme.textMuted : "#6B7F9A"
            font: theme ? theme.captionFont : font
        }
    }

    Connections {
        target: appBridge
        function onMetricsFlashed() {
            pnlChip.playFlash()
            equityChip.playFlash()
        }
    }
}
