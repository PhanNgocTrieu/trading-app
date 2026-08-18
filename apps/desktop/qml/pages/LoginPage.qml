import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root
    property var theme
    property var appBridge
    property bool registerMode: false

    function submit() {
        if (!appBridge)
            return
        const user = userField.text.trim()
        const pass = passField.text
        if (registerMode)
            appBridge.registerUser(user, pass)
        else
            appBridge.login(user, pass)
    }

    MarketBackdrop {
        anchors.fill: parent
        theme: root.theme
        opacity: 0.95
    }

    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#00121A2B" }
            GradientStop { position: 1.0; color: theme ? theme.bgBottom : "#080C16" }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: Math.max(24, Math.min(root.width, root.height) * 0.04)
        spacing: 28

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: 1
            visible: root.width >= 920
            clip: true

            ColumnLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                spacing: 14

                RowLayout {
                    spacing: 12
                    Rectangle {
                        width: 44
                        height: 44
                        radius: 14
                        color: theme ? theme.accent : "#2DD4BF"
                        Text {
                            anchors.centerIn: parent
                            text: "T"
                            color: theme ? theme.accentInk : "#042F2E"
                            font: theme ? theme.titleFont : font
                        }
                    }
                    Text {
                        text: "AURORA DESK"
                        color: theme ? theme.accent : "#2DD4BF"
                        font: theme ? theme.captionFont : font
                    }
                }

                Text {
                    Layout.fillWidth: true
                    text: "Paper trading,\nbuilt like a real desk."
                    wrapMode: Text.WordWrap
                    color: theme ? theme.textPrimary : "#EAF0FA"
                    font: theme ? theme.displayFont : font
                }
                Text {
                    Layout.fillWidth: true
                    Layout.maximumWidth: 460
                    text: "Register, fund the wallet, and route market or limit orders against a live mock tape — with a resting book that fills when price crosses."
                    wrapMode: Text.WordWrap
                    color: theme ? theme.textSecondary : "#8FA3C0"
                    font: theme ? theme.bodyFont : font
                }
            }
        }

        Item {
            Layout.fillWidth: root.width < 920
            Layout.fillHeight: true
            Layout.preferredWidth: root.width < 920 ? 1 : 420
            Layout.maximumWidth: root.width < 920 ? 560 : 440
            Layout.minimumWidth: 320

            Rectangle {
                id: card
                width: Math.min(parent.width, 440)
                anchors.centerIn: parent
                radius: theme ? theme.radiusLg : 18
                color: theme ? theme.glass : "#1A2740CC"
                border.color: theme ? theme.borderStrong : "#4A6288"
                border.width: 1
                height: form.implicitHeight + 48

                Rectangle {
                    anchors.fill: parent
                    anchors.topMargin: 10
                    radius: parent.radius
                    color: "#00000040"
                    z: -1
                }

                ColumnLayout {
                    id: form
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 24
                    spacing: 14

                    Text {
                        text: registerMode ? "Create account" : "Welcome back"
                        color: theme ? theme.textPrimary : "#EAF0FA"
                        font: theme ? theme.titleFont : font
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: registerMode
                              ? "Username ≥ 3 characters, password ≥ 6."
                              : "Sign in to open the paper desk."
                        color: theme ? theme.textSecondary : "#8FA3C0"
                        font: theme ? theme.bodyFont : font
                    }

                    SegmentedControl {
                        theme: root.theme
                        Layout.fillWidth: true
                        model: ["Sign in", "Register"]
                        currentIndex: registerMode ? 1 : 0
                        onCurrentIndexChanged: registerMode = currentIndex === 1
                    }

                    Text {
                        text: "USERNAME"
                        color: theme ? theme.textMuted : "#6B7F9A"
                        font: theme ? theme.captionFont : font
                    }
                    ThemedTextField {
                        id: userField
                        theme: root.theme
                        Layout.fillWidth: true
                        placeholderText: "alice"
                        Keys.onReturnPressed: passField.forceActiveFocus()
                    }
                    Text {
                        text: "PASSWORD"
                        color: theme ? theme.textMuted : "#6B7F9A"
                        font: theme ? theme.captionFont : font
                    }
                    ThemedTextField {
                        id: passField
                        theme: root.theme
                        Layout.fillWidth: true
                        placeholderText: "••••••••"
                        echoMode: TextInput.Password
                        Keys.onReturnPressed: root.submit()
                    }

                    Text {
                        Layout.fillWidth: true
                        visible: appBridge && appBridge.authError.length > 0
                        text: appBridge ? appBridge.authError : ""
                        color: theme ? theme.danger : "#FB7185"
                        wrapMode: Text.WordWrap
                        font: theme ? theme.bodyFont : font
                    }

                    PrimaryButton {
                        theme: root.theme
                        Layout.fillWidth: true
                        text: registerMode ? "Create account" : "Enter desk"
                        onClicked: root.submit()
                    }
                }
            }
        }
    }

    Component.onCompleted: userField.forceActiveFocus()
}
