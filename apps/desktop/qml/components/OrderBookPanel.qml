import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    property var theme
    property var bidModel
    property var askModel
    property string symbol: ""

    radius: theme ? theme.radiusLg : 18
    color: theme ? theme.surface : "#152036"
    border.color: theme ? theme.border : "#334766"
    border.width: 1
    clip: true

    function maxQty(model) {
        if (!model)
            return 1
        let peak = 1
        const n = model.rowCount()
        for (let i = 0; i < n; ++i) {
            const idx = model.index(i, 0)
            const qty = Number(model.data(idx, Qt.UserRole + 3))
            if (qty > peak)
                peak = qty
        }
        return peak
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: theme ? theme.spaceMd : 16
        spacing: 8

        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: theme ? theme.orbBlue : "#60A5FA"
            }
            Text {
                Layout.fillWidth: true
                text: root.symbol.length > 0 ? ("Book · " + root.symbol) : "Order Book"
                elide: Text.ElideRight
                font: theme ? theme.headingFont : font
                color: theme ? theme.textPrimary : "#EAF0FA"
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 1
                spacing: 6

                Text {
                    text: "BIDS"
                    color: theme ? theme.buy : "#34D399"
                    font: theme ? theme.captionFont : font
                }

                ListView {
                    id: bids
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: root.bidModel
                    spacing: 4
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: bookRow
                    Text {
                        anchors.centerIn: parent
                        visible: bids.count === 0
                        text: "No bids"
                        color: theme ? theme.textMuted : "#6B7F9A"
                        font: theme ? theme.bodyFont : font
                    }
                }
            }

            Rectangle {
                Layout.fillHeight: true
                width: 1
                color: theme ? theme.border : "#334766"
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: 1
                spacing: 6

                Text {
                    text: "ASKS"
                    color: theme ? theme.sell : "#FB7185"
                    font: theme ? theme.captionFont : font
                }

                ListView {
                    id: asks
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: root.askModel
                    spacing: 4
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: bookRow
                    Text {
                        anchors.centerIn: parent
                        visible: asks.count === 0
                        text: "No asks"
                        color: theme ? theme.textMuted : "#6B7F9A"
                        font: theme ? theme.bodyFont : font
                    }
                }
            }
        }
    }

    Component {
        id: bookRow
        Rectangle {
            required property int index
            required property double price
            required property int quantity
            width: ListView.view.width
            height: 30
            radius: 6
            color: theme ? theme.surfaceMuted : "#223250"
            clip: true

            readonly property bool isAsk: ListView.view === asks
            readonly property real peak: root.maxQty(ListView.view.model)
            readonly property real frac: peak > 0 ? Math.min(1, quantity / peak) : 0

            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                width: parent.width * frac
                radius: 6
                color: isAsk
                       ? (theme ? theme.dangerDim : "#4C1D24")
                       : (theme ? theme.successDim : "#064E3B")
                opacity: 0.9
                Behavior on width { NumberAnimation { duration: 220; easing.type: Easing.OutCubic } }
            }

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 8
                anchors.rightMargin: 8
                Text {
                    Layout.fillWidth: true
                    text: Number(price).toFixed(2)
                    color: isAsk ? (theme ? theme.sell : "#FB7185")
                                 : (theme ? theme.buy : "#34D399")
                    font: theme ? theme.monoFont : font
                }
                Text {
                    text: quantity
                    color: theme ? theme.textPrimary : "#EAF0FA"
                    font: theme ? theme.monoFont : font
                }
            }
        }
    }
}
