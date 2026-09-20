import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

ApplicationWindow {
    id: root
    visible: true
    width: 500
    height: 300
    title: qsTr("IPTVManager - 数据库设置")

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Label {
            text: qsTr("首次运行，请配置文件路径：")
            font.pixelSize: 14
            font.bold: true
        }

        //配置文件路径
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: qsTr("配置文件:")
                Layout.preferredWidth: 80
            }

            TextField {
                id: leConfigPath
                Layout.fillWidth: true
                placeholderText: qsTr("选择或创建配置文件(.ini)")
            }

            Button {
                text: qsTr("选择")
                onClicked: selectConfigDialog.open()
            }

            Button {
                text: qsTr("新建")
                onClicked: createConfigDialog.open()
            }
        }

        //数据库文件路径
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: qsTr("数据库文件:")
                Layout.preferredWidth: 80
            }

            TextField {
                id: leDbPath
                Layout.fillWidth: true
                placeholderText: qsTr("选择或创建数据库文件(.db)")
            }

            Button {
                text: qsTr("选择")
                onClicked: selectDbDialog.open()
            }

            Button {
                text: qsTr("新建")
                onClicked: createDbDialog.open()
            }
        }

        Item { Layout.fillHeight: true }

        //提示信息
        Label {
            text: qsTr("点击确定后应用将退出，请重新启动以加载配置。")
            font.pixelSize: 11
            color: "#757575"
        }

        //确定按钮
        RowLayout {
            Layout.fillWidth: true

            Item { Layout.fillWidth: true }

            Button {
                text: qsTr("确定")
                enabled: leConfigPath.text.length > 0 && leDbPath.text.length > 0
                onClicked: {
                    setupHelper.saveAndQuit(leConfigPath.text, leDbPath.text)
                }
            }
        }
    }

    //文件对话框
    FileDialog {
        id: selectConfigDialog
        title: qsTr("选择配置文件")
        nameFilters: ["INI文件(*.ini)"]
        onAccepted: {
            leConfigPath.text = selectedFile.toString().replace("file://", "")
        }
    }

    FileDialog {
        id: createConfigDialog
        title: qsTr("创建配置文件")
        nameFilters: ["INI文件(*.ini)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            leConfigPath.text = selectedFile.toString().replace("file://", "")
        }
    }

    FileDialog {
        id: selectDbDialog
        title: qsTr("选择数据库文件")
        nameFilters: ["SQLite数据库(*.db)"]
        onAccepted: {
            leDbPath.text = selectedFile.toString().replace("file://", "")
        }
    }

    FileDialog {
        id: createDbDialog
        title: qsTr("创建数据库文件")
        nameFilters: ["SQLite数据库(*.db)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            leDbPath.text = selectedFile.toString().replace("file://", "")
        }
    }
}
