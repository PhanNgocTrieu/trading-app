import QtQuick

// Dark trading-desk tokens. Green/red only for buy/sell and PnL.
QtObject {
    readonly property color bgTop: "#121A2B"
    readonly property color bgBottom: "#080C16"
    readonly property color bgDeep: "#070A12"
    readonly property color surface: "#152036"
    readonly property color surfaceRaised: "#1B2944"
    readonly property color surfaceMuted: "#223250"
    readonly property color glass: "#1A2740CC"
    readonly property color border: "#334766"
    readonly property color borderStrong: "#4A6288"
    readonly property color textPrimary: "#EAF0FA"
    readonly property color textSecondary: "#8FA3C0"
    readonly property color textMuted: "#6B7F9A"
    readonly property color accent: "#2DD4BF"
    readonly property color accentHover: "#5EEAD4"
    readonly property color accentDim: "#115E59"
    readonly property color accentInk: "#042F2E"
    readonly property color danger: "#FB7185"
    readonly property color dangerDim: "#4C1D24"
    readonly property color success: "#34D399"
    readonly property color successDim: "#064E3B"
    readonly property color warning: "#FBBF24"
    readonly property color buy: "#34D399"
    readonly property color sell: "#FB7185"
    readonly property color orbTeal: "#2DD4BF"
    readonly property color orbBlue: "#60A5FA"
    readonly property color orbViolet: "#A78BFA"

    readonly property int radiusSm: 8
    readonly property int radiusMd: 12
    readonly property int radiusLg: 18
    readonly property int spaceXs: 4
    readonly property int spaceSm: 8
    readonly property int spaceMd: 16
    readonly property int spaceLg: 24
    readonly property int spaceXl: 32

    readonly property string fontFamily: Qt.platform.os === "windows" ? "Segoe UI" : "Avenir Next"
    readonly property string monoFamily: Qt.platform.os === "windows" ? "Consolas" : "Menlo"

    readonly property font displayFont: Qt.font({
        family: fontFamily,
        pixelSize: 34,
        weight: Font.DemiBold,
        letterSpacing: -0.6
    })
    readonly property font titleFont: Qt.font({
        family: fontFamily,
        pixelSize: 22,
        weight: Font.DemiBold,
        letterSpacing: -0.3
    })
    readonly property font headingFont: Qt.font({
        family: fontFamily,
        pixelSize: 15,
        weight: Font.DemiBold
    })
    readonly property font bodyFont: Qt.font({
        family: fontFamily,
        pixelSize: 13,
        weight: Font.Normal
    })
    readonly property font captionFont: Qt.font({
        family: fontFamily,
        pixelSize: 11,
        weight: Font.DemiBold,
        letterSpacing: 0.6
    })
    readonly property font monoFont: Qt.font({
        family: monoFamily,
        pixelSize: 12,
        weight: Font.Medium
    })
    readonly property font metricFont: Qt.font({
        family: monoFamily,
        pixelSize: 16,
        weight: Font.DemiBold
    })
}
