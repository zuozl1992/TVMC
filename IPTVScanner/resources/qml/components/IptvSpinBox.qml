import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: control
    property alias label: labelItem.text
    property alias value: spinBox.value
    property alias from: spinBox.from
    property alias to: spinBox.to
    spacing: 4

    Label {
        id: labelItem
        font.pixelSize: 12
    }

    SpinBox {
        id: spinBox
        Layout.fillWidth: true
        font.pixelSize: 14
    }
}
