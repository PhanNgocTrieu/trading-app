import QtQuick
import QtQuick.Controls

Item {
    id: root
    property var theme
    property string message: ""
    property bool visibleToast: message.length > 0

    height: toast.height
    opacity: visibleToast ? 1 : 0
    visible: opacity > 0.01

    Behavior on opacity { NumberAnimation { duration: 180 } }

    Rectangle {
        id: toast
        width: parent.width
        height: label.implicitHeight + 20
        radius: theme ? theme.radiusMd : 10
        color: theme ? theme.surface : "#FFFFFF"
        border.color: theme ? theme.border : "#C5D0DC"
        border.width: 1
        y: visibleToast ? 0 : 12

        Behavior on y { NumberAnimation { duration: 220; easing.type: Easing.OutCubic } }

        Text {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 12
            text: root.message
            wrapMode: Text.WordWrap
            color: theme ? theme.textPrimary : "#172033"
            font: theme ? theme.bodyFont : font
        }
    }
}
