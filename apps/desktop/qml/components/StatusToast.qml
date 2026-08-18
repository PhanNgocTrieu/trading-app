import QtQuick
import QtQuick.Controls

Item {
    id: root
    property var theme
    property string message: ""

    readonly property bool isError: {
        const s = message.toLowerCase()
        return s.indexOf("fail") >= 0
            || s.indexOf("reject") >= 0
            || s.indexOf("insufficient") >= 0
            || s.indexOf("invalid") >= 0
            || s.indexOf("must") >= 0
            || s.indexOf("error") >= 0
            || s.indexOf("taken") >= 0
            || s.indexOf("not logged") >= 0
    }

    readonly property bool visibleToast: message.length > 0
    implicitHeight: visibleToast ? toast.height : 0
    height: implicitHeight
    opacity: visibleToast ? 1 : 0
    visible: opacity > 0.01
    enabled: visibleToast
    clip: true

    Behavior on opacity { NumberAnimation { duration: 180 } }
    Behavior on implicitHeight { NumberAnimation { duration: 180 } }

    Rectangle {
        id: toast
        width: parent.width
        height: Math.max(40, label.implicitHeight + 20)
        radius: theme ? theme.radiusMd : 12
        color: root.isError
               ? (theme ? theme.dangerDim : "#4C1D24")
               : (theme ? theme.successDim : "#064E3B")
        border.width: 1
        border.color: root.isError
                      ? (theme ? theme.danger : "#FB7185")
                      : (theme ? theme.success : "#34D399")
        y: visibleToast ? 0 : 10
        Behavior on y { NumberAnimation { duration: 220; easing.type: Easing.OutCubic } }

        Text {
            id: label
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 12
            text: root.message
            wrapMode: Text.WordWrap
            color: theme ? theme.textPrimary : "#EAF0FA"
            font: theme ? theme.bodyFont : font
        }
    }
}
