import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import "IptvComponents"
import "tabs"
import "dialogs"

ApplicationWindow {
    id: root
    visible: true
    width: 880
    height: 650
    title: qsTr("IPTV管理")
    Material.theme: Material.Light
    Material.primary: "#1976D2"
    Material.accent: "#FF6F00"

    //确认退出对话框
    Dialog {
        id: confirmExitDialog
        title: qsTr("提示")
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: qsTr("确认退出？")
        }

        onAccepted: {
            root.close()
        }
    }

    //重新配置对话框
    Dialog {
        id: reconfigDialog
        title: qsTr("提示")
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: qsTr("确定要重新配置路径吗？需要重启应用。")
        }

        onAccepted: {
            backend.resetConfig()
            Qt.quit()
        }
    }

    //重置数据库对话框
    Dialog {
        id: resetDbDialog
        title: qsTr("确认")
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: qsTr("确定恢复出厂设置？这将删除所有数据！")
        }

        onAccepted: {
            backend.resetDatabase()
            resetSuccessDialog.open()
        }
    }

    //重置成功对话框
    Dialog {
        id: resetSuccessDialog
        title: qsTr("提示")
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok

        Label {
            text: qsTr("修复成功，程序即将退出请重新运行。")
        }

        onAccepted: {
            Qt.quit()
        }
    }

    //警告对话框
    Dialog {
        id: warningDialog
        title: qsTr("警告")
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok
        property alias text: warningLabel.text

        Label {
            id: warningLabel
        }
    }

    //信息对话框
    Dialog {
        id: infoDialog
        title: qsTr("IPTV管理 使用说明")
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok
        width: 650
        height: 550
        font.pixelSize: 11

        ScrollView {
            anchors.fill: parent
            clip: true

            TextArea {
                readOnly: true
                wrapMode: TextArea.Wrap
                font.pixelSize: 12
                text: "IPTV管理 v2.0 使用说明\n" +
                      "========================\n\n" +
                      "【频道源浏览】\n" +
                      "查看所有频道和信号源信息。\n" +
                      "- 表格显示频道ID、名称、分组、IP地址、端口、分辨率、帧率、类型等信息\n" +
                      "- 点击表头可按该列排序（升序/降序切换）\n" +
                      "- 支持分页浏览，每页50条记录\n" +
                      "- 右上角刷新按钮可手动刷新数据\n\n" +
                      "【频道编辑】\n" +
                      "编辑频道信息，修改即时保存到数据库。\n" +
                      "- 双击单元格可编辑内容（频道ID、名称、分组、城市、描述、备注、LOGO）\n" +
                      "- 编辑后按回车或点击其他位置自动保存\n" +
                      "- 支持分页浏览和点击表头排序\n\n" +
                      "【频道源检查】\n" +
                      "检测信号源质量，查看视频画面。\n" +
                      "- 点击「启动检查」加载检测列表\n" +
                      "- 选择检测模式：正式频道/测试频道\n" +
                      "- 选择排序方式：按IP排序/按ID排序\n" +
                      "- 下拉框选择要检测的频道\n" +
                      "- 左侧显示视频预览画面（16:9比例）\n" +
                      "- 右侧显示检测信息（台标、名称、备注、类型、分辨率、帧率）\n" +
                      "- 检测值与数据库不一致时显示红色\n" +
                      "- 点击「修改」提交检测结果，不存在的频道会提示创建\n" +
                      "- 点击「上一个」/「下一个」切换频道，值有变化时会提示是否保存\n\n" +
                      "【配置】\n" +
                      "管理服务器地址和导出设置。\n" +
                      "- 单播服务地址：流媒体服务器地址（如 http://192.168.1.1:12345）\n" +
                      "- Logo服务地址：台标图片服务器地址\n" +
                      "- FCC服务地址：FCC服务器地址\n" +
                      "- 组播地址模板：支持花括号范围表达式（如 239.49.{0#1#2}）\n" +
                      "- 端口：支持#分隔的多端口（如 6000#8000#8008）\n" +
                      "- 协议：选择udp或rtp\n" +
                      "- 导出选项：合并相同频道、导出Logo、导出清晰度\n" +
                      "- 频道分组：选择要导出的频道分组\n" +
                      "- 导入频道文件：支持.mc/.m3u/.txt三种格式\n" +
                      "- 导出播放文件：导出M3U/TXT格式\n" +
                      "- 生成扫描文件：生成用于IPTVScanner的扫描配置\n\n" +
                      "【导入文件格式说明】\n" +
                      "- .mc格式：频道名 类型 分辨率(帧率),http://server/udp/IP:端口\n" +
                      "- .m3u格式：标准M3U播放列表（支持group-title分组）\n" +
                      "- .txt格式：GroupName,#genre#\\nChannelName,URL\n\n" +
                      "【地址模板语法】\n" +
                      "- 零散值：1#3 表示 1和3\n" +
                      "- 范围值：[8-10] 表示 8、9、10\n" +
                      "- 混合使用：1#3#[5-7] 表示 1、3、5、6、7\n" +
                      "- 最多支持3层花括号嵌套\n\n" +
                      "【底部工具栏】\n" +
                      "- 导出：导出Excel/CSV格式\n" +
                      "- 打开扫描软件：启动IPTVScanner\n" +
                      "- 说明：显示本帮助\n" +
                      "- 配置路径：重新选择配置和数据库文件\n" +
                      "- 修复：恢复出厂设置（删除所有数据）"
            }
        }
    }

    //文件对话框
    FileDialog {
        id: importFileDialog
        title: qsTr("选择文件")
        nameFilters: ["所有支持格式(*.mc *.m3u *.txt)", "MC文件(*.mc)", "M3U文件(*.m3u)", "TXT文件(*.txt)"]
        onAccepted: {
            var path = selectedFile.toString().replace("file://", "")
            backend.importFilePath = path
        }
    }

    FileDialog {
        id: saveScanFileDialog
        title: qsTr("保存扫描文件")
        nameFilters: ["文本文件(*.txt)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            var result = backend.createScanFile(selectedFile.toString().replace("file://", ""))
            exportResultDialog.text = result ? qsTr("扫描文件已生成") : qsTr("生成扫描文件失败")
            exportResultDialog.open()
        }
    }

    FileDialog {
        id: saveM3uDialog
        title: qsTr("保存M3U文件")
        nameFilters: ["m3u(*.m3u)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            backend.exportM3u(selectedFile.toString().replace("file://", ""))
            exportResultDialog.text = qsTr("M3U文件已导出")
            exportResultDialog.open()
        }
    }

    FileDialog {
        id: saveTxtDialog
        title: qsTr("保存TXT文件")
        nameFilters: ["txt(*.txt)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            backend.exportTxt(selectedFile.toString().replace("file://", ""))
            exportResultDialog.text = qsTr("TXT文件已导出")
            exportResultDialog.open()
        }
    }

    //导出结果提示对话框
    Dialog {
        id: exportResultDialog
        title: qsTr("导出结果")
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Ok
        property alias text: exportResultLabel.text

        Label {
            id: exportResultLabel
            font.pixelSize: 12
        }
    }

    FileDialog {
        id: saveExcelDialog
        title: qsTr("导出")
        nameFilters: ["Excel(*.xlsx);;CSV(*.csv)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            backend.exportExcel(selectedFile.toString().replace("file://", ""))
        }
    }

    //顶部TabBar
    header: TabBar {
        id: tabBar
        Material.background: "#1976D2"
        Material.foreground: "#FFFFFF"

        TabButton {
            text: qsTr("频道源浏览")
            font.pixelSize: 14
        }
        TabButton {
            text: qsTr("频道编辑")
            font.pixelSize: 14
        }
        TabButton {
            text: qsTr("频道源检查")
            font.pixelSize: 14
        }
        TabButton {
            text: qsTr("配置")
            font.pixelSize: 14
        }
    }

    //Tab内容
    StackLayout {
        id: stackLayout
        currentIndex: tabBar.currentIndex
        anchors.fill: parent
        anchors.margins: 8

        BrowseTab {
            id: browseTab
        }

        EditTab {
            id: editTab
        }

        CheckTab {
            id: checkTab
        }

        SettingsTab {
            id: settingsTab
        }
    }

    //底部工具栏
    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 8
            anchors.rightMargin: 8

            IptvProgressBar {
                id: exportProgress
                Layout.fillWidth: true
                Layout.preferredHeight: 20
                value: backend.exportProgress / 100.0
                visible: backend.exporting
                progressColor: "#28a745"
            }

            IptvButton {
                text: qsTr("导出")
                onClicked: saveExcelDialog.open()
            }

            IptvButton {
                text: qsTr("打开扫描软件")
                onClicked: backend.openScannerApp()
            }

            IptvButton {
                text: qsTr("说明")
                style: "secondary"
                onClicked: infoDialog.open()
            }

            IptvButton {
                text: qsTr("配置路径")
                style: "secondary"
                onClicked: reconfigDialog.open()
            }

            IptvButton {
                text: qsTr("修复")
                style: "danger"
                onClicked: resetDbDialog.open()
            }
        }
    }

    Component.onCompleted: {
        backend.initialize(configPath, dbPath)
    }
}
