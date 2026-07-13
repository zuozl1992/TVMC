import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: control
    property alias label: labelItem.text
    property alias value: spinBox.value
    property alias from: spinBox.from
    property alias to: spinBox.to
    property bool small: false
    spacing: small ? 2 : 4

    Label {
        id: labelItem
        font.pixelSize: small ? 11 : 12
    }

    SpinBox {
        id: spinBox
        Layout.fillWidth: true
        font.pixelSize: small ? 11 : 14
        implicitHeight: small ? 24 : 32
        topPadding: small ? 2 : 6
        bottomPadding: small ? 2 : 6
        leftPadding: small ? 4 : 8
        rightPadding: small ? 4 : 8
    }
}
