import QtQuick

// Decorative login art: drifting orbs + a faux equity curve. No network images.
Item {
    id: root
    property var theme
    clip: true

    Repeater {
        model: [
            { w: 340, bx: 0.08, by: 0.10, dy: 24, c: 0, dur: 4800 },
            { w: 250, bx: 0.48, by: 0.36, dy: -30, c: 1, dur: 5600 },
            { w: 190, bx: 0.22, by: 0.62, dy: 18, c: 2, dur: 6400 }
        ]
        delegate: Rectangle {
            required property var modelData
            width: modelData.w
            height: width
            radius: width / 2
            opacity: 0.16
            color: modelData.c === 0 ? (theme ? theme.orbTeal : "#2DD4BF")
                 : modelData.c === 1 ? (theme ? theme.orbBlue : "#60A5FA")
                                     : (theme ? theme.orbViolet : "#A78BFA")

            property real baseX: root.width * modelData.bx
            property real baseY: root.height * modelData.by
            property real drift: 0
            x: baseX
            y: baseY + drift

            SequentialAnimation on drift {
                loops: Animation.Infinite
                NumberAnimation {
                    to: modelData.dy
                    duration: modelData.dur
                    easing.type: Easing.InOutSine
                }
                NumberAnimation {
                    to: 0
                    duration: modelData.dur
                    easing.type: Easing.InOutSine
                }
            }
        }
    }

    Canvas {
        id: chart
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 28
        height: Math.min(parent.height * 0.42, 220)

        onPaint: {
            const ctx = getContext("2d")
            ctx.reset()
            const w = width
            const h = height
            if (w < 8 || h < 8)
                return

            const pts = [0.72, 0.64, 0.68, 0.55, 0.58, 0.42, 0.48, 0.33, 0.38, 0.22, 0.28, 0.18]
            ctx.beginPath()
            ctx.moveTo(0, h)
            for (let i = 0; i < pts.length; ++i) {
                const x = (i / (pts.length - 1)) * w
                const y = pts[i] * h
                ctx.lineTo(x, y)
            }
            ctx.lineTo(w, h)
            ctx.closePath()
            const fill = ctx.createLinearGradient(0, 0, 0, h)
            fill.addColorStop(0, "#2DD4BF55")
            fill.addColorStop(1, "#2DD4BF00")
            ctx.fillStyle = fill
            ctx.fill()

            ctx.beginPath()
            for (let i = 0; i < pts.length; ++i) {
                const x = (i / (pts.length - 1)) * w
                const y = pts[i] * h
                if (i === 0)
                    ctx.moveTo(x, y)
                else
                    ctx.lineTo(x, y)
            }
            ctx.strokeStyle = "#5EEAD4"
            ctx.lineWidth = 2.2
            ctx.stroke()
        }

        onWidthChanged: requestPaint()
        onHeightChanged: requestPaint()
        Component.onCompleted: requestPaint()
    }
}
