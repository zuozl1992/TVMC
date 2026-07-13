import QtQuick
import QtQuick.Controls

ProgressBar {
    id: control
    property string progressColor: "#0078d7"

    implicitHeight: 20

    background: Rectangle {
        implicitHeight: 20
        color: "#f0f0f0"
        radius: 5
        border.color: "#808080"
        border.width: 1
    }

    contentItem: Item {
        implicitHeight: 20

        Rectangle {
            width: control.visualPosition * parent.width
            height: parent.height
            radius: 5
            color: control.progressColor
        }

        Label {
            anchors.centerIn: parent
            text: Math.round(control.value) + "%"
            font.pixelSize: 11
            color: control.value > 50 ? "#FFFFFF" : "#212121"
        }
    }
}
