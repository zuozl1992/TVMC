import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

CheckBox {
    id: control
    property bool small: false
    font.pixelSize: small ? 11 : 14
    implicitHeight: small ? 22 : 28
    spacing: small ? 4 : 8
}
