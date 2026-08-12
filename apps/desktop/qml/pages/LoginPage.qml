import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../components"

Item {
    id: root
    property var theme
    property var appBridge

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(420, parent.width - 48)
        spacing: theme.spaceLg

        Text {
            Layout.fillWidth: true
            text: "Trading App"
            font: theme.titleFont
            color: theme.textPrimary
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            Layout.fillWidth: true
            text: "Paper trading desk — register or sign in to continue."
            wrapMode: Text.WordWrap
            font: theme.bodyFont
            color: theme.textSecondary
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle {
            Layout.fillWidth: true
            radius: theme.radiusLg
            color: theme.surface
            border.color: theme.border
            border.width: 1
            implicitHeight: formCol.implicitHeight + theme.spaceLg * 2

            ColumnLayout {
                id: formCol
                anchors.fill: parent
                anchors.margins: theme.spaceLg
                spacing: theme.spaceMd

                TextField {
                    id: userField
                    Layout.fillWidth: true
                    placeholderText: "Username (>= 3)"
                    font: theme.bodyFont
                }

                TextField {
                    id: passField
                    Layout.fillWidth: true
                    placeholderText: "Password (>= 6)"
                    echoMode: TextInput.Password
                    font: theme.bodyFont
                }

                Text {
                    Layout.fillWidth: true
                    visible: appBridge && appBridge.authError.length > 0
                    text: appBridge ? appBridge.authError : ""
                    color: theme.danger
                    wrapMode: Text.WordWrap
                    font: theme.bodyFont
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: theme.spaceSm

                    PrimaryButton {
                        theme: root.theme
                        Layout.fillWidth: true
                        text: "Login"
                        onClicked: {
                            if (!appBridge)
                                return
                            if (userField.text.trim().length < 3) {
                                appBridge.clearAuthError()
                                // bridge will get validation via auth service; show local tip
                            }
                            appBridge.login(userField.text.trim(), passField.text)
                        }
                    }

                    PrimaryButton {
                        theme: root.theme
                        Layout.fillWidth: true
                        text: "Register"
                        onClicked: {
                            if (!appBridge)
                                return
                            appBridge.registerUser(userField.text.trim(), passField.text)
                        }
                    }
                }
            }
        }
    }
}
