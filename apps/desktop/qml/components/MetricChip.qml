import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property var theme
    property string label: ""
    property string value: ""
    property color valueColor: theme ? theme.textPrimary : "#172033"

    width: chip.implicitWidth
    height: chip.implicitHeight

    Rectangle {
        id: chip
        anchors.fill: parent
        implicitWidth: col.implicitWidth + (theme ? theme.spaceMd * 2 : 24)
        implicitHeight: col.implicitHeight + (theme ? theme.spaceSm * 2 : 16)
        radius: theme ? theme.radiusMd : 10
        color: theme ? theme.surface : "#FFFFFF"
        border.color: theme ? theme.border : "#C5D0DC"
        border.width: 1

        ColumnLayout {
            id: col
            anchors.centerIn: parent
            spacing: 2

            Text {
                text: root.label
                color: theme ? theme.textSecondary : "#5B6B7C"
                font: theme ? theme.monoFont : Qt.font({pixelSize: 12})
            }
            Text {
                id: valueText
                text: root.value
                color: root.valueColor
                font: theme ? theme.headingFont : Qt.font({pixelSize: 18, weight: Font.DemiBold})

                Behavior on color { ColorAnimation { duration: 220 } }
            }
        }
    }

    function playFlash() {
        flashAnim.restart()
    }

    SequentialAnimation on opacity {
        id: flashAnim
        running: false
        NumberAnimation { to: 0.55; duration: 90 }
        NumberAnimation { to: 1.0; duration: 180 }
    }
}