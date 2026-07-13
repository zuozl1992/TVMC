import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: control
    property alias label: labelItem.text
    property alias text: textField.text
    property alias placeholderText: textField.placeholderText
    property alias readOnly: textField.readOnly
    spacing: 2

    Label {
        id: labelItem
        font.pixelSize: 12
        visible: text.length > 0
    }

    TextField {
        id: textField
        Layout.fillWidth: true
        font.pixelSize: 14
    }
}
