import QtQuick
import QtQuick.Controls

Item {
    id: control
    property string source: ""
    property bool hasFrame: false

    Rectangle {
        anchors.fill: parent
        color: "#000000"

        Image {
            id: previewImage
            anchors.fill: parent
            anchors.margins: 2
            fillMode: Image.PreserveAspectFit
            source: control.source
            visible: control.hasFrame

            cache: false
        }

        Label {
            anchors.centerIn: parent
            text: qsTr("无视频")
            color: "#666666"
            font.pixelSize: 14
            visible: !control.hasFrame
        }
    }
}
