import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property var theme
    property string label: ""
    property string value: ""
    property color valueColor: theme ? theme.textPrimary : "#EAF0FA"

    implicitWidth: 148
    implicitHeight: 62
    Layout.minimumWidth: 120
    Layout.preferredWidth: 148
    Layout.maximumWidth: 180

    Rectangle {
        id: chip
        anchors.fill: parent
        radius: theme ? theme.radiusMd : 12
        color: theme ? theme.surface : "#152036"
        border.color: theme ? theme.border : "#334766"
        border.width: 1

        ColumnLayout {
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            anchors.topMargin: 8
            anchors.bottomMargin: 8
            spacing: 2

            Text {
                Layout.fillWidth: true
                text: root.label
                color: theme ? theme.textMuted : "#6B7F9A"
                font: theme ? theme.captionFont : font
                elide: Text.ElideRight
            }
            Text {
                id: valueText
                Layout.fillWidth: true
                text: root.value
                color: root.valueColor
                font: theme ? theme.metricFont : font
                elide: Text.ElideRight
                Behavior on color { ColorAnimation { duration: 220 } }
            }
        }
    }

    function playFlash() {
        flashAnim.restart()
    }

    SequentialAnimation {
        id: flashAnim
        running: false
        NumberAnimation { target: chip; property: "opacity"; to: 0.55; duration: 90 }
        NumberAnimation { target: chip; property: "opacity"; to: 1.0; duration: 180 }
    }
}
