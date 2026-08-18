import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property var theme
    property var model: []
    property int currentIndex: 0
    property bool buySell: false

    implicitHeight: 40
    implicitWidth: row.implicitWidth
    clip: true

    Rectangle {
        anchors.fill: parent
        radius: theme ? theme.radiusMd : 12
        color: theme ? theme.surfaceMuted : "#223250"
        border.color: theme ? theme.border : "#334766"
        border.width: 1
    }

    RowLayout {
        id: row
        anchors.fill: parent
        anchors.margins: 3
        spacing: 3

        Repeater {
            model: root.model
            delegate: Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 9
                color: root.currentIndex === index ? selectedColor : "transparent"

                readonly property color selectedColor: {
                    if (!root.buySell)
                        return theme ? theme.accent : "#2DD4BF"
                    return index === 0
                           ? (theme ? theme.buy : "#34D399")
                           : (theme ? theme.sell : "#FB7185")
                }

                Behavior on color { ColorAnimation { duration: 140 } }

                Text {
                    anchors.centerIn: parent
                    text: modelData
                    font: theme ? theme.headingFont : font
                    color: root.currentIndex === index
                           ? (theme ? theme.accentInk : "#042F2E")
                           : (theme ? theme.textSecondary : "#8FA3C0")
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.currentIndex = index
                }
            }
        }
    }
}
