import QtQuick
import QtQuick.Controls
import "pages"
import "components"

// Root window: gradient backdrop + StackView switching LoginPage <-> ShellPage
// when `app.loggedIn` changes (TradingAppBridge).
ApplicationWindow {
    id: window
    width: 1280
    height: 820
    visible: true
    title: "Trading App"
    color: "transparent"

    // Re-exposed so Component instances can bind without scope issues.
    property var appTheme: themeObj
    property var appBridge: app

    Theme {
        id: themeObj
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: window.appTheme.bgTop }
            GradientStop { position: 1.0; color: window.appTheme.bgBottom }
        }
    }

    StackView {
        id: stack
        anchors.fill: parent

        pushEnter: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 220 }
        }
        pushExit: Transition {
            NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 180 }
        }
        popEnter: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 220 }
        }
        popExit: Transition {
            NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 180 }
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
