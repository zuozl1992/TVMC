import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

TextField {
    id: control
    font.pixelSize: 11
    implicitHeight: 24
    topPadding: 2
    bottomPadding: 2
    leftPadding: 4
    rightPadding: 4

    background: Rectangle {
        implicitHeight: 24
        color: control.enabled ? "#FFFFFF" : "#F5F5F5"
        border.color: control.activeFocus ? Material.accent : "#BDBDBD"
        border.width: control.activeFocus ? 2 : 1
        radius: 2
    }
}
