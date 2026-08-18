import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property var theme
    property var model

    radius: theme ? theme.radiusLg : 18
    color: theme ? theme.surface : "#152036"
    border.color: theme ? theme.border : "#334766"
    border.width: 1
    clip: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme ? theme.spaceMd : 16
        spacing: 10

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: theme ? theme.warning : "#FBBF24"
            }
            Text {
                text: "Portfolio"
                font: theme ? theme.headingFont : font
                color: theme ? theme.textPrimary : "#EAF0FA"
            }
            Item { Layout.fillWidth: true }
        }

        ListView {
            id: list
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.model
            spacing: 8
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                required property int index
                required property string symbol
                required property int quantity
                required property double avgCost
                required property double marketPrice
                required property double unrealizedPnl
                width: ListView.view.width
                height: 72
                radius: 12
                color: theme ? theme.surfaceMuted : "#223250"
                border.width: 1
                border.color: theme ? theme.border : "#334766"
                clip: true

                Rectangle {
                    width: 4
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    radius: 2
                    color: unrealizedPnl >= 0
                           ? (theme ? theme.buy : "#34D399")
                           : (theme ? theme.sell : "#FB7185")
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 14
                    anchors.rightMargin: 12
                    anchors.topMargin: 10
                    anchors.bottomMargin: 10
                    spacing: 6

                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: symbol
                            color: theme ? theme.textPrimary : "#EAF0FA"
                            font: theme ? theme.headingFont : font
                        }
                        Text {
                            text: quantity + " sh"
                            color: theme ? theme.textSecondary : "#8FA3C0"
                            font: theme ? theme.bodyFont : font
                        }
                        Item { Layout.fillWidth: true }
                        Text {
                            text: (unrealizedPnl >= 0 ? "+" : "") + Number(unrealizedPnl).toFixed(2)
                            color: unrealizedPnl >= 0
                                   ? (theme ? theme.success : "#34D399")
                                   : (theme ? theme.danger : "#FB7185")
                            font: theme ? theme.monoFont : font
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            Layout.fillWidth: true
                            text: "Avg " + Number(avgCost).toFixed(2)
                            elide: Text.ElideRight
                            color: theme ? theme.textMuted : "#6B7F9A"
                            font: theme ? theme.captionFont : font
                        }
                        Text {
                            text: "Last " + Number(marketPrice).toFixed(2)
                            color: theme ? theme.textMuted : "#6B7F9A"
                            font: theme ? theme.captionFont : font
                        }
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: list.count === 0
                text: "No open positions"
                color: theme ? theme.textSecondary : "#8FA3C0"
                font: theme ? theme.bodyFont : font
            }
        }
    }
}
