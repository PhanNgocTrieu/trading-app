import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property var theme
    property var appBridge
    property string selectedSymbol: appBridge && appBridge.symbols.length > 0
                                    ? appBridge.symbols[0] : ""
    property int qty: 10

    radius: theme ? theme.radiusLg : 18
    color: theme ? theme.surface : "#152036"
    border.color: theme ? theme.border : "#334766"
    border.width: 1
    clip: true

    function selectSymbol(symbol) {
        selectedSymbol = symbol
        const idx = symbolBox.find(symbol)
        if (idx >= 0)
            symbolBox.currentIndex = idx
    }

    ScrollView {
        id: scroller
        anchors.fill: parent
        clip: true
        contentWidth: availableWidth
        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        ColumnLayout {
            width: scroller.availableWidth
            spacing: theme ? theme.spaceMd : 16

            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: theme ? theme.spaceMd : 16
                Layout.leftMargin: theme ? theme.spaceMd : 16
                Layout.rightMargin: theme ? theme.spaceMd : 16
                spacing: 8
                Rectangle {
                    width: 8
                    height: 8
                    radius: 4
                    color: theme ? theme.accent : "#2DD4BF"
                }
                Text {
                    text: "Ticket"
                    font: theme ? theme.headingFont : font
                    color: theme ? theme.textPrimary : "#EAF0FA"
                }
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: theme ? theme.spaceMd : 16
                Layout.rightMargin: theme ? theme.spaceMd : 16
                spacing: 8

                Text {
                    text: "SYMBOL"
                    color: theme ? theme.textMuted : "#6B7F9A"
                    font: theme ? theme.captionFont : font
                }
                ComboBox {
                    id: symbolBox
                    Layout.fillWidth: true
                    Layout.preferredHeight: 40
                    model: appBridge ? appBridge.symbols : []
                    font: theme ? theme.bodyFont : font
                    Component.onCompleted: syncIndex()
                    onModelChanged: syncIndex()
                    function syncIndex() {
                        if (!appBridge || appBridge.symbols.length === 0)
                            return
                        const idx = appBridge.symbols.indexOf(root.selectedSymbol)
                        currentIndex = idx >= 0 ? idx : 0
                        root.selectedSymbol = currentText
                    }
                    onActivated: root.selectedSymbol = currentText

                    delegate: ItemDelegate {
                        width: symbolBox.width
                        highlighted: symbolBox.highlightedIndex === index
                        contentItem: Text {
                            text: modelData
                            font: theme ? theme.bodyFont : font
                            color: theme ? theme.textPrimary : "#EAF0FA"
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: highlighted ? (theme ? theme.accentDim : "#115E59")
                                               : "transparent"
                            radius: 6
                        }
                    }
                    indicator: Canvas {
                        x: symbolBox.width - width - 12
                        y: (symbolBox.height - height) / 2
                        width: 10
                        height: 6
                        onPaint: {
                            const ctx = getContext("2d")
                            ctx.reset()
                            ctx.moveTo(0, 0)
                            ctx.lineTo(width, 0)
                            ctx.lineTo(width / 2, height)
                            ctx.closePath()
                            ctx.fillStyle = theme ? theme.textSecondary : "#8FA3C0"
                            ctx.fill()
                        }
                    }

                    background: Rectangle {
                        radius: theme ? theme.radiusMd : 12
                        color: theme ? theme.surfaceMuted : "#223250"
                        border.width: 1
                        border.color: symbolBox.down || symbolBox.hovered
                                      ? (theme ? theme.accent : "#2DD4BF")
                                      : (theme ? theme.border : "#334766")
                    }
                    contentItem: Text {
                        leftPadding: 12
                        rightPadding: symbolBox.indicator.width + 12
                        text: symbolBox.displayText
                        font: symbolBox.font
                        color: theme ? theme.textPrimary : "#EAF0FA"
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                    popup: Popup {
                        y: symbolBox.height + 4
                        width: symbolBox.width
                        padding: 6
                        background: Rectangle {
                            radius: 10
                            color: theme ? theme.surfaceRaised : "#1B2944"
                            border.color: theme ? theme.border : "#334766"
                            border.width: 1
                        }
                        contentItem: ListView {
                            clip: true
                            implicitHeight: Math.min(contentHeight, 220)
                            model: symbolBox.popup.visible ? symbolBox.delegateModel : null
                            currentIndex: symbolBox.highlightedIndex
                        }
                    }
                }

                Text {
                    text: "SIDE"
                    color: theme ? theme.textMuted : "#6B7F9A"
                    font: theme ? theme.captionFont : font
                }
                SegmentedControl {
                    id: sideSeg
                    theme: root.theme
                    Layout.fillWidth: true
                    model: ["BUY", "SELL"]
                    buySell: true
                }

                Text {
                    text: "TYPE"
                    color: theme ? theme.textMuted : "#6B7F9A"
                    font: theme ? theme.captionFont : font
                }
                SegmentedControl {
                    id: typeSeg
                    theme: root.theme
                    Layout.fillWidth: true
                    model: ["MARKET", "LIMIT"]
                }

                Text {
                    text: "LIMIT PRICE"
                    color: theme ? theme.textMuted : "#6B7F9A"
                    font: theme ? theme.captionFont : font
                    opacity: typeSeg.currentIndex === 1 ? 1 : 0.4
                }
                ThemedTextField {
                    id: limitField
                    theme: root.theme
                    Layout.fillWidth: true
                    enabled: typeSeg.currentIndex === 1
                    placeholderText: "Limit price"
                    text: "180"
                    validator: DoubleValidator {
                        bottom: 0.01
                        decimals: 2
                        notation: DoubleValidator.StandardNotation
                    }
                    opacity: enabled ? 1 : 0.45
                }

                Text {
                    text: "QUANTITY"
                    color: theme ? theme.textMuted : "#6B7F9A"
                    font: theme ? theme.captionFont : font
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    GhostButton {
                        theme: root.theme
                        text: "−"
                        onClicked: {
                            root.qty = Math.max(1, root.qty - 1)
                            qtyField.text = String(root.qty)
                        }
                    }
                    ThemedTextField {
                        id: qtyField
                        theme: root.theme
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        text: "10"
                        validator: IntValidator { bottom: 1; top: 1000000 }
                        onEditingFinished: {
                            const n = parseInt(text, 10)
                            root.qty = isNaN(n) ? 1 : Math.max(1, n)
                            text = String(root.qty)
                        }
                    }
                    GhostButton {
                        theme: root.theme
                        text: "+"
                        onClicked: {
                            root.qty = Math.min(1000000, root.qty + 1)
                            qtyField.text = String(root.qty)
                        }
                    }
                }

                PrimaryButton {
                    theme: root.theme
                    Layout.fillWidth: true
                    text: typeSeg.currentIndex === 1 ? "Submit Limit" : "Submit Market"
                    onClicked: {
                        if (!appBridge)
                            return
                        const symbol = symbolBox.currentText || root.selectedSymbol
                        if (typeSeg.currentIndex === 1) {
                            appBridge.placeLimitOrder(symbol,
                                                      sideSeg.currentIndex === 0 ? "BUY" : "SELL",
                                                      root.qty,
                                                      Number(limitField.text))
                        } else {
                            appBridge.placeMarketOrder(symbol,
                                                       sideSeg.currentIndex === 0 ? "BUY" : "SELL",
                                                       root.qty)
                        }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.leftMargin: theme ? theme.spaceMd : 16
                Layout.rightMargin: theme ? theme.spaceMd : 16
                height: 1
                color: theme ? theme.border : "#334766"
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.leftMargin: theme ? theme.spaceMd : 16
                Layout.rightMargin: theme ? theme.spaceMd : 16
                Layout.bottomMargin: theme ? theme.spaceMd : 16
                spacing: 8

                Text {
                    text: "Wallet"
                    font: theme ? theme.headingFont : font
                    color: theme ? theme.textPrimary : "#EAF0FA"
                }
                ThemedTextField {
                    id: cashField
                    theme: root.theme
                    Layout.fillWidth: true
                    placeholderText: "Amount"
                    text: "1000"
                    validator: DoubleValidator {
                        bottom: 0.01
                        decimals: 2
                        notation: DoubleValidator.StandardNotation
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    PrimaryButton {
                        theme: root.theme
                        Layout.fillWidth: true
                        text: "Deposit"
                        onClicked: {
                            if (appBridge)
                                appBridge.deposit(Number(cashField.text))
                        }
                    }
                    GhostButton {
                        theme: root.theme
                        Layout.fillWidth: true
                        destructive: true
                        text: "Withdraw"
                        onClicked: {
                            if (appBridge)
                                appBridge.withdraw(Number(cashField.text))
                        }
                    }
                }
            }
        }
    }
}
