import QtQuick

// Design tokens for the light-studio look (Phase 5).
// Cold gray backgrounds, teal accent, green/red only for PnL — no purple/cream theme.
QtObject {
    readonly property color bgTop: "#DCE4EE"
    readonly property color bgBottom: "#F4F7FA"
    readonly property color surface: "#FFFFFF"
    readonly property color surfaceMuted: "#EEF2F6"
    readonly property color border: "#C5D0DC"
    readonly property color textPrimary: "#172033"
    readonly property color textSecondary: "#5B6B7C"
    readonly property color accent: "#0F6E6A"
    readonly property color accentHover: "#0B5854"
    readonly property color danger: "#B42318"
    readonly property color success: "#16794C"
    readonly property color warning: "#B54708"

    readonly property int radiusMd: 10
    readonly property int radiusLg: 16
    readonly property int spaceSm: 8
    readonly property int spaceMd: 16
    readonly property int spaceLg: 24

    readonly property string fontFamily: {
        const os = Qt.platform.os
        if (os === "osx" || os === "macos")
            return "Avenir Next"
        return "Segoe UI Variable"
    }

    readonly property font titleFont: Qt.font({
        family: fontFamily,
        pixelSize: 28,
        weight: Font.DemiBold
    })
    readonly property font headingFont: Qt.font({
        family: fontFamily,
        pixelSize: 18,
        weight: Font.DemiBold
    })
    readonly property font bodyFont: Qt.font({
        family: fontFamily,
        pixelSize: 14,
        weight: Font.Normal
    })
    readonly property font monoFont: Qt.font({
        family: fontFamily,
        pixelSize: 13,
        weight: Font.Medium
    })
}
