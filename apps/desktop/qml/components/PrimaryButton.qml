import QtQuick
import QtQuick.Controls

Button {
    id: root
    property var theme
    property bool destructive: false

    leftPadding: 18
    rightPadding: 18
    topPadding: 10
    bottomPadding: 10
    font: theme ? theme.bodyFont : font

    background: Rectangle {
        radius: theme ? theme.radiusMd : 10
        color: {
            if (!root.enabled)
                return theme ? theme.surfaceMuted : "#EEF2F6"
            if (root.down)
                return root.destructive
                      ? (theme ? theme.danger : "#B42318")
                      : (theme ? theme.accentHover : "#0B5854")
            if (root.hovered)
                return root.destructive
                      ? Qt.lighter(theme ? theme.danger : "#B42318", 1.08)
                      : Qt.lighter(theme ? theme.accent : "#0F6E6A", 1.08)
            return root.destructive
                   ? (theme ? theme.danger : "#B42318")
                   : (theme ? theme.accent : "#0F6E6A")
        }
        Behavior on color { ColorAnimation { duration: 120 } }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.enabled ? "#FFFFFF" : (theme ? theme.textSecondary : "#5B6B7C")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
