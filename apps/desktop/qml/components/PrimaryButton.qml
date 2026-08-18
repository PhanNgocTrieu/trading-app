import QtQuick
import QtQuick.Controls

Button {
    id: root
    property var theme
    property bool destructive: false

    leftPadding: 16
    rightPadding: 16
    topPadding: 10
    bottomPadding: 10
    font: theme ? theme.headingFont : font
    hoverEnabled: true
    clip: true

    background: Rectangle {
        radius: theme ? theme.radiusMd : 12
        color: {
            if (!root.enabled)
                return theme ? theme.surfaceMuted : "#223250"
            if (root.destructive)
                return root.down ? Qt.darker(theme ? theme.danger : "#FB7185", 1.15)
                                 : (root.hovered ? Qt.lighter(theme ? theme.danger : "#FB7185", 1.08)
                                                 : (theme ? theme.danger : "#FB7185"))
            return root.down ? (theme ? theme.accentHover : "#5EEAD4")
                             : (root.hovered ? (theme ? theme.accentHover : "#5EEAD4")
                                             : (theme ? theme.accent : "#2DD4BF"))
        }
        Behavior on color { ColorAnimation { duration: 120 } }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: {
            if (!root.enabled)
                return theme ? theme.textMuted : "#6B7F9A"
            if (root.destructive)
                return "#1A0A0C"
            return theme ? theme.accentInk : "#042F2E"
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
