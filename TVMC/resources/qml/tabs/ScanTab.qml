import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts
import QtQuick.Dialogs
import "../IptvComponents"

Item {
    id: scanTab
    property bool initialized: false

    //更新URL输入框（配置加载后调用）
    function updateUrlField() {
        if (scannerBackend.useUrl) {
            leURL.text = scannerBackend.scanUrl
        } else {
            leURL.text = scannerBackend.filePath
        }
    }

    //开始确认对话框
    Dialog {
        id: confirmStartDialog
        title: qsTr("提示")
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: qsTr("确认开始？开始后将清空上次结果。")
        }

        onAccepted: {
            doStartScan()
        }
    }

    //警告对话框
    Dialog {
        id: warningDialog
        title: qsTr("警告")
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        standardButtons: Dialog.Ok
        property alias text: warningLabel.text

        Label {
            id: warningLabel
        }
    }

    //文件保存对话框
    FileDialog {
        id: saveFileDialog
        title: qsTr("保存文件")
        nameFilters: ["mc文件(*.mc)"]
        fileMode: FileDialog.SaveFile
        onAccepted: {
            scannerBackend.saveResults(selectedFile.toString().replace("file://", ""))
        }
    }

    //文件打开对话框
    FileDialog {
        id: openFileDialog
        title: qsTr("选择文件")
        nameFilters: ["model file(*.txt)"]
        onAccepted: {
            var path = selectedFile.toString().replace("file://", "")
            scannerBackend.filePath = path
            leURL.text = path
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        //1. URL输入区
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("扫描地址")
            font.pixelSize: 11

            RowLayout {
                anchors.fill: parent
                spacing: 6

                RadioButton {
                    id: rbUseUrl
                    text: qsTr("URL")
                    font.pixelSize: 11
                    implicitHeight: 22
                    checked: scannerBackend.useUrl
                    onCheckedChanged: {
                        scannerBackend.useUrl = checked
                        if (checked) {
                            leURL.text = scannerBackend.scanUrl
                        } else {
                            leURL.text = scannerBackend.filePath
                        }
                    }
                }

                RadioButton {
                    id: rbUseFile
                    text: qsTr("文件")
                    font.pixelSize: 11
                    implicitHeight: 22
                    checked: !scannerBackend.useUrl
                    onCheckedChanged: {
                        scannerBackend.useUrl = !checked
                        if (!checked) {
                            leURL.text = scannerBackend.scanUrl
                        } else {
                            leURL.text = scannerBackend.filePath
                        }
                    }
                }

                TextField {
                    id: leURL
                    Layout.fillWidth: true
                    placeholderText: rbUseUrl.checked ? qsTr("输入URL模板") : qsTr("选择文件路径")
                    text: scannerBackend.useUrl ? scannerBackend.scanUrl : scannerBackend.filePath
                    font.pixelSize: 11
                    implicitHeight: 24
                    topPadding: 2; bottomPadding: 2; leftPadding: 4; rightPadding: 4
                    onTextChanged: {
                        if (!scanTab.initialized) return
                        if (scannerBackend.useUrl) {
                            scannerBackend.scanUrl = text
                        } else {
                            scannerBackend.filePath = text
                        }
                    }
                }

                Button {
                    implicitWidth: 36
                    implicitHeight: 30
                    onClicked: openFileDialog.open()
                    visible: !rbUseUrl.checked
                    padding: 0

                    contentItem: Text {
                        text: "..."
                        font.pixelSize: 16
                        font.bold: true
                        color: "#212121"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        anchors.fill: parent
                    }

                    background: Rectangle {
                        color: parent.pressed ? "#BDBDBD" : (parent.hovered ? "#E0E0E0" : "#F5F5F5")
                        radius: 4
                        border.color: "#BDBDBD"
                        border.width: 1
                    }
                }
            }
        }

        //2. 参数区
        GroupBox {
            Layout.fillWidth: true
            title: qsTr("扫描参数")
            font.pixelSize: 11

            RowLayout {
                anchors.fill: parent
                spacing: 8

                Label { text: qsTr("线程:"); font.pixelSize: 11 }
                SpinBox {
                    id: sbThMax
                    from: 1; to: 128
                    value: scannerBackend.threadMax
                    font.pixelSize: 11
                    implicitHeight: 24
                    onValueChanged: scannerBackend.threadMax = value
                }

                Label { text: qsTr("超时(ms):"); font.pixelSize: 11 }
                SpinBox {
                    id: sbTimeout
                    from: 100; to: 10000
                    value: scannerBackend.timeout
                    font.pixelSize: 11
                    implicitHeight: 24
                    editable: true
                    onValueChanged: scannerBackend.timeout = value
                }

                CheckBox {
                    id: cbAutoStep
                    text: qsTr("自动跳过")
                    font.pixelSize: 11
                    implicitHeight: 22
                    checked: scannerBackend.autoStep
                    onCheckedChanged: scannerBackend.autoStep = checked
                }

                CheckBox {
                    id: cbAddTS
                    text: qsTr("添加TS")
                    font.pixelSize: 11
                    implicitHeight: 22
                    checked: scannerBackend.addTs
                    onCheckedChanged: scannerBackend.addTs = checked
                }

                CheckBox {
                    id: cbSlowScan
                    text: qsTr("精细扫描")
                    font.pixelSize: 11
                    implicitHeight: 22
                    checked: scannerBackend.slowScan
                    onCheckedChanged: scannerBackend.slowScan = checked
                }

                Item { Layout.fillWidth: true }
            }
        }

        //3. 按钮区
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            IptvButton {
                id: btnStart
                text: qsTr("开始")
                enabled: !scannerBackend.scanning
                onClicked: confirmStartDialog.open()
            }

            IptvButton {
                id: btnStop
                text: qsTr("停止")
                style: "danger"
                enabled: scannerBackend.scanning
                onClicked: scannerBackend.stopScan()
            }

            IptvButton {
                id: btnClear
                text: qsTr("清空")
                style: "secondary"
                onClicked: scannerBackend.clearResults()
            }

            IptvButton {
                id: btnSave
                text: qsTr("保存")
                style: "secondary"
                onClicked: saveFileDialog.open()
            }

            Item { Layout.fillWidth: true }
        }

        //4. 进度区
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                id: lbStatus
                Layout.fillWidth: true
                text: scannerBackend.statusText
                elide: Text.ElideMiddle
                font.pixelSize: 11
                color: "#757575"
            }

            Label {
                id: lbProgress
                text: scannerBackend.progressText
                font.pixelSize: 11
                font.bold: true
            }

            Label {
                id: lbStatus2
                text: qsTr("成功") + ":" + scannerBackend.okCount + ", " + qsTr("失败") + ":" + scannerBackend.errorCount
                font.pixelSize: 11
                color: "#2E7D32"
            }
        }

        //进度条
        ProgressBar {
            id: progressBar
            Layout.fillWidth: true
            implicitHeight: 16
            value: scannerBackend.progress / 100.0

            background: Rectangle {
                implicitHeight: 16
                color: "#f0f0f0"
                radius: 4
                border.color: "#808080"
                border.width: 1
            }

            contentItem: Item {
                implicitHeight: 16

                Rectangle {
                    width: progressBar.visualPosition * parent.width
                    height: parent.height
                    radius: 4
                    color: "#0078d7"
                }

                Label {
                    anchors.centerIn: parent
                    text: Math.round(progressBar.value * 100) + "%"
                    font.pixelSize: 10
                    color: progressBar.value > 0.5 ? "#FFFFFF" : "#212121"
                }
            }
        }

        //5. 扫描结果
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 4

            Label {
                text: qsTr("扫描结果")
                font.pixelSize: 11
                font.bold: true
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                border.color: "#E0E0E0"
                border.width: 1
                radius: 4

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 1
                    clip: true

                    TextArea {
                        id: tbLog
                        readOnly: true
                        font.pixelSize: 11
                        font.family: "Monaco"
                        wrapMode: TextArea.Wrap
                        background: Rectangle { color: "transparent" }

                        Connections {
                            target: scannerBackend
                            function onLogLineAdded(line) {
                                tbLog.append(line)
                            }
                            function onLogCleared() {
                                tbLog.clear()
                            }
                        }
                    }
                }
            }
        }
    }

    //开始扫描的内部函数
    function doStartScan() {
        var url = ""
        if (rbUseUrl.checked) {
            url = scannerBackend.scanUrl
        } else {
            url = scannerBackend.filePath
        }

        if (url.length === 0) {
            warningDialog.text = qsTr("输入有误")
            warningDialog.open()
            return
        }

        scannerBackend.startScan(url, rbUseUrl.checked)
    }

    Component.onCompleted: {
        //显示使用帮助
        tbLog.append(qsTr("1.对于局域网使用软路由组播转单播地址"))
        tbLog.append(qsTr("    a.由于软路由及udpxy性能问题，超时时间建议设置为10000（10s），即不做限制，否则会报连接数量不足；"))
        tbLog.append(qsTr("    b.并发线程数量应当根据udpxy最大连接数设置，受软路由性能限制，单纯将udpxy最大连接数修改为很大并没有意义，建议值：udpxy设置为20，软件中设置为8；"))
        tbLog.append(qsTr("    c.udpxy会在长时间多连接轮询的不定期崩溃，建议不要选择过多的地址。"))
        tbLog.append(qsTr("    d.一般同一个ip地址只会部署一个频道，可以勾选发现相同IP已有成功地址后自动跳过后续"))
        tbLog.append("")
        tbLog.append(qsTr("2.地址栏支持最多三个范围输入，范围规则如下："))
        tbLog.append(qsTr("    a.零散值：1#3表示：1、3；"))
        tbLog.append(qsTr("    b.范围值 [8-10]表示：8、9、10；[08-10]表示：08、09、10；"))
        tbLog.append(qsTr("    c.可混合使用：1#3#[5-7]表示1、3、5、6、7。"))
    }
}
