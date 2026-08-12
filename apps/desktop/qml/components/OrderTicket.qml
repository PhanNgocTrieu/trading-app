import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property var theme
    property var appBridge
    property string selectedSymbol: appBridge && appBridge.symbols.length > 0
                                    ? appBridge.symbols[0] : ""

    radius: theme ? theme.radiusLg : 16
    color: theme ? theme.surface : "#FFFFFF"
    border.color: theme ? theme.border : "#C5D0DC"
    border.width: 1

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme ? theme.spaceMd : 16
        spacing: theme ? theme.spaceMd : 16

        Text {
            text: "Order Ticket"
            font: theme ? theme.headingFont : font
            color: theme ? theme.textPrimary : "#172033"
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 12
            rowSpacing: 12

            Text {
                text: "Symbol"
                color: theme ? theme.textSecondary : "#5B6B7C"
                font: theme ? theme.bodyFont : font
            }
            ComboBox {
                id: symbolBox
                Layout.fillWidth: true
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
            }

            Text {
                text: "Side"
                color: theme ? theme.textSecondary : "#5B6B7C"
                font: theme ? theme.bodyFont : font
            }
            ComboBox {
                id: sideBox
                Layout.fillWidth: true
                model: ["BUY", "SELL"]
                font: theme ? theme.bodyFont : font
            }

            Text {
                text: "Quantity"
                color: theme ? theme.textSecondary : "#5B6B7C"
                font: theme ? theme.bodyFont : font
            }
            SpinBox {
                id: qtySpin
                Layout.fillWidth: true
                from: 1
                to: 1000000
                value: 10
                editable: true
                font: theme ? theme.bodyFont : font
            }
        }

        PrimaryButton {
            theme: root.theme
            Layout.fillWidth: true
            text: "Submit Market Order"
            onClicked: {
                if (!appBridge)
                    return
                const symbol = symbolBox.currentText || root.selectedSymbol
                appBridge.placeMarketOrder(symbol, sideBox.currentText, qtySpin.value)
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: theme ? theme.border : "#C5D0DC"
        }

        Text {
            text: "Wallet"
            font: theme ? theme.headingFont : font
            color: theme ? theme.textPrimary : "#172033"
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: depositField
                Layout.fillWidth: true
                placeholderText: "Deposit amount"
                text: "1000"
                validator: DoubleValidator {
                    bottom: 0.01
                    decimals: 2
                    notation: DoubleValidator.StandardNotation
                }
                font: theme ? theme.bodyFont : font
            }

            PrimaryButton {
                theme: root.theme
                text: "Deposit"
                onClicked: {
                    if (!appBridge)
                        return
                    appBridge.deposit(Number(depositField.text))
                }
            }
        }
    }
}
