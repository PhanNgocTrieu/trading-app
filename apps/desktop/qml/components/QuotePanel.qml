import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// Market Watch panel — binds to app.quoteModel roles: symbol, name, lastPrice.
Rectangle {
    id: root
    property var theme
    property var model

    radius: theme ? theme.radiusLg : 16
    color: theme ? theme.surface : "#FFFFFF"
    border.color: theme ? theme.border : "#C5D0DC"
    border.width: 1
    clip: true

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme ? theme.spaceMd : 16
        spacing: theme ? theme.spaceSm : 8

        Text {
            text: "Market Watch"
            font: theme ? theme.headingFont : font
            color: theme ? theme.textPrimary : "#172033"
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: theme ? theme.border : "#C5D0DC"
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: root.model
            spacing: 6

            header: RowLayout {
                width: ListView.view ? ListView.view.width : parent.width
                spacing: 8
                Text {
                    Layout.preferredWidth: 70
                    text: "Symbol"
                    color: theme ? theme.textSecondary : "#5B6B7C"
                    font: theme ? theme.monoFont : font
                }
                Text {
                    Layout.fillWidth: true
                    text: "Name"
                    color: theme ? theme.textSecondary : "#5B6B7C"
                    font: theme ? theme.monoFont : font
                }
                Text {
                    Layout.preferredWidth: 80
                    horizontalAlignment: Text.AlignRight
                    text: "Last"
                    color: theme ? theme.textSecondary : "#5B6B7C"
                    font: theme ? theme.monoFont : font
                }
            }

            delegate: Rectangle {
                width: ListView.view.width
                height: 36
                radius: 8
                color: index % 2 === 0
                       ? (theme ? theme.surfaceMuted : "#EEF2F6")
                       : "transparent"

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    anchors.rightMargin: 8
                    spacing: 8

                    Text {
                        Layout.preferredWidth: 70
                        text: symbol
                        color: theme ? theme.textPrimary : "#172033"
                        font: theme ? theme.monoFont : font
                    }
                    Text {
                        Layout.fillWidth: true
                        text: name
                        elide: Text.ElideRight
                        color: theme ? theme.textSecondary : "#5B6B7C"
                        font: theme ? theme.bodyFont : font
                    }
                    Text {
                        Layout.preferredWidth: 80
                        horizontalAlignment: Text.AlignRight
                        text: Number(lastPrice).toFixed(2)
                        color: theme ? theme.textPrimary : "#172033"
                        font: theme ? theme.monoFont : font
                    }
                }
            }

            Text {
                anchors.centerIn: parent
                visible: parent.count === 0
                text: "No quotes"
                color: theme ? theme.textSecondary : "#5B6B7C"
                font: theme ? theme.bodyFont : font
            }
        }
    }
}
