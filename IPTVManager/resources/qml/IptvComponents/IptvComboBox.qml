import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

ComboBox {
    id: control
    property bool small: false
    font.pixelSize: small ? 11 : 14
    implicitHeight: small ? 24 : 32
    topPadding: small ? 2 : 6
    bottomPadding: small ? 2 : 6
    leftPadding: small ? 4 : 8
    rightPadding: small ? 4 : 8
}
