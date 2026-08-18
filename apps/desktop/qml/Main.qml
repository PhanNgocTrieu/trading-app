import QtQuick
import QtQuick.Controls
import "pages"
import "components"

ApplicationWindow {
    id: window
    width: 1440
    height: 900
    minimumWidth: 1080
    minimumHeight: 720
    visible: true
    title: "Aurora Desk"
    color: "transparent"

    property var appTheme: themeObj
    property var appBridge: app

    Theme {
        id: themeObj
    }

    palette.window: themeObj.bgBottom
    palette.windowText: themeObj.textPrimary
    palette.base: themeObj.surfaceMuted
    palette.text: themeObj.textPrimary
    palette.button: themeObj.surfaceRaised
    palette.buttonText: themeObj.textPrimary
    palette.highlight: themeObj.accent
    palette.highlightedText: themeObj.accentInk

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            orientation: Gradient.Vertical
            GradientStop { position: 0.0; color: window.appTheme.bgTop }
            GradientStop { position: 1.0; color: window.appTheme.bgBottom }
        }
    }

    StackView {
        id: stack
        anchors.fill: parent
        clip: true

        pushEnter: Transition {
            ParallelAnimation {
                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 280 }
                NumberAnimation { property: "y"; from: 18; to: 0; duration: 320; easing.type: Easing.OutCubic }
            }
        }
        pushExit: Transition {
            NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 180 }
        }
        popEnter: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 240 }
        }
        popExit: Transition {
            ParallelAnimation {
                NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 180 }
                NumberAnimation { property: "y"; from: 0; to: 12; duration: 180 }
            }
        }
    }

    function showLogin() {
        stack.clear()
        stack.push(loginComponent)
    }

    function showShell() {
        stack.clear()
        stack.push(shellComponent)
    }

    Component {
        id: loginComponent
        LoginPage {
            theme: window.appTheme
            appBridge: window.appBridge
        }
    }

    Component {
        id: shellComponent
        ShellPage {
            theme: window.appTheme
            appBridge: window.appBridge
        }
    }

    Connections {
        target: window.appBridge
        function onLoggedInChanged() {
            if (window.appBridge.loggedIn)
                window.showShell()
            else
                window.showLogin()
        }
    }

    Component.onCompleted: {
        if (window.appBridge && window.appBridge.loggedIn)
            window.showShell()
        else
            window.showLogin()
    }
}
