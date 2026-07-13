import QtQuick
import QtQuick.Controls

Item {
    id: control
    property alias model: comboBox.model
    property int currentIndex: comboBox.currentIndex
    signal activated(int index)

    implicitHeight: 36

    ComboBox {
        id: comboBox
        anchors.fill: parent
        font.pixelSize: 13

        delegate: ItemDelegate {
            width: comboBox.width
            height: 36
            highlighted: comboBox.currentIndex === index

            contentItem: Text {
                leftPadding: 8
                verticalAlignment: Text.AlignVCenter
                text: model.display || ""
                font.pixelSize: 13
                elide: Text.ElideRight
                color: highlighted ? "#FFFFFF" : "#212121"
            }

            background: Rectangle {
                color: highlighted ? "#1976D2" : "transparent"
            }
        }

        onActivated: function(index) {
            control.activated(index)
        }
    }
}
