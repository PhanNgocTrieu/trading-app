import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property var theme
    property var model
    signal symbolActivated(string symbol)

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
                color: theme ? theme.accent : "#2DD4BF"
            }
            Text {
                text: "Market Watch"
                font: theme ? theme.headingFont : font
                color: theme ? theme.textPrimary : "#EAF0FA"
            }
            Item { Layout.fillWidth: true }
            Text {
                text: list.count + " names"
                color: theme ? theme.textMuted : "#6B7F9A"
                font: theme ? theme.captionFont : font
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Text {
                Layout.preferredWidth: 64
                text: "SYM"
                color: theme ? theme.textMuted : "#6B7F9A"
                font: theme ? theme.captionFont : font
            }
            Text {
                Layout.fillWidth: true
                text: "NAME"
                color: theme ? theme.textMuted : "#6B7F9A"
                font: theme ? theme.captionFont : font
            }
            Text {
                Layout.preferredWidth: 76
                horizontalAlignment: Text.AlignRight
                text: "LAST"
                color: theme ? theme.textMuted : "#6B7F9A"
                font: theme ? theme.captionFont : font
            }
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
                id: row
                required property int index
                required property string symbol
                required property string name
                required property double lastPrice
                width: ListView.view.width
                height: 48
                radius: 10
                color: hover.hovered
                       ? (theme ? theme.surfaceRaised : "#1B2944")
                       : (index % 2 === 0 ? (theme ? theme.surfaceMuted : "#223250")
                                          : "transparent")

                property real prevPrice: lastPrice
                property color tickColor: "transparent"

                onLastPriceChanged: {
                    if (prevPrice === lastPrice)
                        return
                    tickColor = lastPrice > prevPrice
                                ? (theme ? theme.buy : "#34D399")
                                : (theme ? theme.sell : "#FB7185")
                    tickFlash.restart()
                    prevPrice = lastPrice
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    anchors.rightMargin: 10
                    spacing: 8

                    Rectangle {
                        width: 3
                        Layout.fillHeight: true
                        Layout.topMargin: 8
                        Layout.bottomMargin: 8
                        radius: 2
                        color: row.tickColor
                        opacity: row.tickColor === Qt.rgba(0, 0, 0, 0) ? 0 : 1
                        Behavior on color { ColorAnimation { duration: 180 } }
                    }

                    Text {
                        Layout.preferredWidth: 56
                        text: row.symbol
                        color: theme ? theme.textPrimary : "#EAF0FA"
                        font: theme ? theme.headingFont : font
                        elide: Text.ElideRight
                    }
                    Text {
                        Layout.fillWidth: true
                        text: row.name
                        elide: Text.ElideRight
                        color: theme ? theme.textSecondary : "#8FA3C0"
                        font: theme ? theme.bodyFont : font
                    }
                    Text {
                        Layout.preferredWidth: 76
                        horizontalAlignment: Text.AlignRight
                        text: Number(row.lastPrice).toFixed(2)
                        color: row.tickColor === Qt.rgba(0, 0, 0, 0)
                               ? (theme ? theme.textPrimary : "#EAF0FA")
                               : row.tickColor
                        font: theme ? theme.monoFont : font
                        Behavior on color { ColorAnimation { duration: 180 } }
                    }
                }

                HoverHandler { id: hover }
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.symbolActivated(row.symbol)
                }

                SequentialAnimation {
                    id: tickFlash
                    running: false
                    PauseAnimation { duration: 700 }
                    ColorAnimation {
                        target: row
                        property: "tickColor"
                        to: "transparent"
                        duration: 400
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: list.count === 0
                text: "No quotes"
                color: theme ? theme.textSecondary : "#8FA3C0"
                font: theme ? theme.bodyFont : font
            }
        }
    }
}
