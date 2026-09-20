import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Button {
    id: control
    property string style: "primary" // primary, secondary, danger

    font.pixelSize: 14
    implicitHeight: 36

    Material.background: style === "danger" ? "#D32F2F" :
                         style === "secondary" ? "#757575" : "#1976D2"
    Material.foreground: "#FFFFFF"
    Material.roundedScale: Material.SmallScale
}
