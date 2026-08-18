import QtQuick
import QtQuick.Controls

TextField {
    id: root
    property var theme
    property string label: ""

    color: theme ? theme.textPrimary : "#EAF0FA"
    placeholderTextColor: theme ? theme.textMuted : "#6B7F9A"
    font: theme ? theme.bodyFont : font
    leftPadding: 12
    rightPadding: 12
    topPadding: 10
    bottomPadding: 10
    selectByMouse: true
    clip: true

    background: Rectangle {
        implicitHeight: 40
        radius: theme ? theme.radiusMd : 12
        color: theme ? theme.surfaceMuted : "#223250"
        border.width: root.activeFocus ? 1.5 : 1
        border.color: root.activeFocus
                      ? (theme ? theme.accent : "#2DD4BF")
                      : (theme ? theme.border : "#334766")
        Behavior on border.color { ColorAnimation { duration: 140 } }
    }
}
