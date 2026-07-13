import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

RadioButton {
    id: control
    property bool small: false
    font.pixelSize: small ? 11 : 14
    implicitHeight: small ? 22 : 28
    spacing: small ? 4 : 8
}
