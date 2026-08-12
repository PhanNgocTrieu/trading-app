import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property var theme
    property var model

    radius: theme ? theme.radiusLg : 16
    color: theme ? theme.surface : "#FFFFFF"
    border.color: theme ? theme.border : "#C5D0DC"
    border.width: 1
    clip: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme ? theme.spaceMd : 16
        spacing: theme ? theme.spaceSm : 8

        Text {
            text: "Portfolio"
            font: theme ? theme.headingFont : font
            color: theme ? theme.textPrimary : "#172033"
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: theme ? theme.border : "#C5D0DC"
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.model
            spacing: 6

            header: RowLayout {
                width: ListView.view ? ListView.view.width : parent.width
                spacing: 6
                Text { Layout.preferredWidth: 58; text: "Symbol"; color: theme.textSecondary; font: theme.monoFont }
                Text { Layout.preferredWidth: 40; text: "Qty"; color: theme.textSecondary; font: theme.monoFont }
                Text { Layout.preferredWidth: 70; horizontalAlignment: Text.AlignRight; text: "Avg"; color: theme.textSecondary; font: theme.monoFont }
                Text { Layout.preferredWidth: 70; horizontalAlignment: Text.AlignRight; text: "Last"; color: theme.textSecondary; font: theme.monoFont }
                Text { Layout.fillWidth: true; horizontalAlignment: Text.AlignRight; text: "uPnL"; color: theme.textSecondary; font: theme.monoFont }
            }

            delegate: Rectangle {
                width: ListView.view.width
                height: 38
                radius: 8
                color: index % 2 === 0 ? theme.surfaceMuted : "transparent"

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 6

                    Text {
                        Layout.preferredWidth: 58
                        text: symbol
                        color: theme.textPrimary
                        font: theme.monoFont
                    }
                    Text {
                        Layout.preferredWidth: 40
                        text: quantity
                        color: theme.textPrimary
                        font: theme.monoFont
                    }
                    Text {
                        Layout.preferredWidth: 70
                        horizontalAlignment: Text.AlignRight
                        text: Number(avgCost).toFixed(2)
                        color: theme.textPrimary
                        font: theme.monoFont
                    }
                    Text {
                        Layout.preferredWidth: 70
                        horizontalAlignment: Text.AlignRight
                        text: Number(marketPrice).toFixed(2)
                        color: theme.textPrimary
                        font: theme.monoFont
                    }
                    Text {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignRight
                        text: (unrealizedPnl >= 0 ? "+" : "") + Number(unrealizedPnl).toFixed(2)
                        color: unrealizedPnl >= 0 ? theme.success : theme.danger
                        font: theme.monoFont
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: parent.count === 0
                text: "No open positions"
                color: theme.textSecondary
                font: theme.bodyFont
            }
        }
    }
}
