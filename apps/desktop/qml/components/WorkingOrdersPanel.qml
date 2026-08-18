import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property var theme
    property var appBridge
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
                color: theme ? theme.orbViolet : "#A78BFA"
            }
            Text {
                text: "Working Orders"
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
            spacing: 6
            boundsBehavior: Flickable.StopAtBounds

            delegate: Rectangle {
                required property int orderId
                required property string symbol
                required property string side
                required property int quantity
                required property double limitPrice
                required property string status
                width: ListView.view.width
                height: 52
                radius: 10
                color: theme ? theme.surfaceMuted : "#223250"
                border.width: 1
                border.color: theme ? theme.border : "#334766"
                clip: true

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    spacing: 8

                    Rectangle {
                        width: 46
                        height: 22
                        radius: 6
                        color: side === "BUY"
                               ? (theme ? theme.successDim : "#064E3B")
                               : (theme ? theme.dangerDim : "#4C1D24")
                        Text {
                            anchors.centerIn: parent
                            text: side
                            color: side === "BUY"
                                   ? (theme ? theme.buy : "#34D399")
                                   : (theme ? theme.sell : "#FB7185")
                            font: theme ? theme.captionFont : font
                        }
                    }

                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 2
                        Text {
                            Layout.fillWidth: true
                            text: symbol + " · " + quantity + " @ " + Number(limitPrice).toFixed(2)
                            elide: Text.ElideRight
                            color: theme ? theme.textPrimary : "#EAF0FA"
                            font: theme ? theme.bodyFont : font
                        }
                        Text {
                            text: status.length ? status : "PENDING"
                            color: theme ? theme.textMuted : "#6B7F9A"
                            font: theme ? theme.captionFont : font
                        }
                    }

                    GhostButton {
                        theme: root.theme
                        destructive: true
                        text: "Cancel"
                        Layout.alignment: Qt.AlignVCenter
                        onClicked: {
                            if (root.appBridge)
                                root.appBridge.cancelOrder(orderId)
                        }
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: list.count === 0
                text: "No resting limits"
                color: theme ? theme.textSecondary : "#8FA3C0"
                font: theme ? theme.bodyFont : font
            }
        }
    }
}
