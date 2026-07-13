import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

TableView {
    id: control
    property alias model: control.model
    signal headerDoubleClicked(int logicalIndex)

    clip: true
    alternatingRows: true
    boundsBehavior: Flickable.StopAtBounds

    delegate: Rectangle {
        implicitHeight: 30
        color: row % 2 === 0 ? "#FFFFFF" : "#F5F5F5"

        Text {
            anchors.fill: parent
            anchors.leftMargin: 8
            verticalAlignment: Text.AlignVCenter
            text: display || ""
            font.pixelSize: 13
            elide: Text.ElideRight
        }

        MouseArea {
            anchors.fill: parent
            onClicked: control.currentIndex = row
        }
    }

    // Header row
    Row {
        id: headerRow
        z: 2
        Repeater {
            model: control.columns > 0 ? control.columns : 0
            Rectangle {
                width: control.columnWidthProvider(index)
                height: 32
                color: "#E3F2FD"
                border.color: "#BBDEFB"
                border.width: 1

                Text {
                    anchors.fill: parent
                    anchors.leftMargin: 8
                    verticalAlignment: Text.AlignVCenter
                    text: control.model ? control.model.headerData(index, Qt.Horizontal) || "" : ""
                    font.pixelSize: 13
                    font.bold: true
                    color: "#1565C0"
                }

                MouseArea {
                    anchors.fill: parent
                    onDoubleClicked: control.headerDoubleClicked(index)
                }
            }
        }
    }

    ScrollBar.vertical: ScrollBar {}
    ScrollBar.horizontal: ScrollBar {}
}
