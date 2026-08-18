import QtQuick
import QtQuick.Controls

Button {
    id: root
    property var theme
    property bool destructive: false
    property bool active: false

    leftPadding: 14
    rightPadding: 14
    topPadding: 8
    bottomPadding: 8
    font: theme ? theme.bodyFont : font
    hoverEnabled: true

    background: Rectangle {
        radius: theme ? theme.radiusMd : 12
        color: {
            if (root.down)
                return theme ? theme.surfaceMuted : "#223250"
            if (root.active)
                return theme ? theme.accentDim : "#115E59"
            if (root.hovered)
                return theme ? theme.surfaceRaised : "#1B2944"
            return "transparent"
        }
        border.width: 1
        border.color: root.destructive
                      ? (theme ? theme.danger : "#FB7185")
                      : (root.active ? (theme ? theme.accent : "#2DD4BF")
                                     : (theme ? theme.border : "#334766"))
        Behavior on color { ColorAnimation { duration: 120 } }
        Behavior on border.color { ColorAnimation { duration: 120 } }
    }

    contentItem: Text {
        text: root.text
        font: root.font
        color: root.destructive
               ? (theme ? theme.danger : "#FB7185")
               : (root.active ? (theme ? theme.accent : "#2DD4BF")
                              : (theme ? theme.textPrimary : "#EAF0FA"))
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }
}
