import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../IptvComponents"

Item {
    id: settingsTab

    //监听配置变更信号，更新输入框
    Connections {
        target: backend
        function onConfigChanged() {
            leUrlFirst.text = backend.httpUrl
            leFileUrl.text = backend.fileUrl
            leFCCUrl.text = backend.fccUrl
            cbUseFCC.checked = backend.fccEnabled
            leGroupAddress.text = backend.groupAddress
            leGroupPort.text = backend.groupPort
            cbAddTest.checked = backend.addTestEnabled
            cbUrlType.currentIndex = backend.urlTypeIndex
            cbRemoveRepeat.checked = backend.mergeChannels
            cbOutAddLogo.checked = backend.addLogoEnabled
            cbOutAddHD.checked = backend.addHdSuffix
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 6

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentWidth: availableWidth

            ColumnLayout {
                width: settingsTab.width - 8
                spacing: 6

                //服务器配置
                GroupBox {
                    Layout.fillWidth: true
                    title: qsTr("服务器配置")
                    font.pixelSize: 11

                    ColumnLayout {
                        width: parent.width
                        spacing: 6

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            Label { text: qsTr("单播服务地址"); font.pixelSize: 11; Layout.preferredWidth: 70 }
                            TextField { id: leUrlFirst; Layout.fillWidth: true; font.pixelSize: 11; implicitHeight: 24; topPadding: 2; bottomPadding: 2; leftPadding: 4; rightPadding: 4; onEditingFinished: backend.httpUrl = text }
                            Label { text: qsTr("协议"); font.pixelSize: 11 }
                            ComboBox { id: cbUrlType; model: ["udp", "rtp"]; font.pixelSize: 11; implicitHeight: 28; implicitWidth: 80; onCurrentIndexChanged: backend.urlTypeIndex = currentIndex }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            Label { text: qsTr("Logo服务地址"); font.pixelSize: 11; Layout.preferredWidth: 70 }
                            TextField { id: leFileUrl; Layout.fillWidth: true; font.pixelSize: 11; implicitHeight: 24; topPadding: 2; bottomPadding: 2; leftPadding: 4; rightPadding: 4; onEditingFinished: backend.fileUrl = text }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            Label { text: qsTr("FCC服务地址"); font.pixelSize: 11; Layout.preferredWidth: 70 }
                            TextField { id: leFCCUrl; Layout.fillWidth: true; font.pixelSize: 11; implicitHeight: 24; topPadding: 2; bottomPadding: 2; leftPadding: 4; rightPadding: 4; onEditingFinished: backend.fccUrl = text }
                            CheckBox { id: cbUseFCC; text: qsTr("启用FCC"); font.pixelSize: 11; implicitHeight: 22; onCheckedChanged: backend.fccEnabled = checked }
                        }
                    }
                }

                //扫描文件配置
                GroupBox {
                    Layout.fillWidth: true
                    title: qsTr("扫描文件配置")
                    font.pixelSize: 11

                    ColumnLayout {
                        width: parent.width
                        spacing: 6

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            Label { text: qsTr("组播地址"); font.pixelSize: 11; Layout.preferredWidth: 50 }
                            TextField { id: leGroupAddress; Layout.fillWidth: true; font.pixelSize: 11; implicitHeight: 24; topPadding: 2; bottomPadding: 2; leftPadding: 4; rightPadding: 4; onEditingFinished: backend.groupAddress = text }
                            Label { text: qsTr("端口"); font.pixelSize: 11 }
                            TextField { id: leGroupPort; Layout.fillWidth: true; font.pixelSize: 11; implicitHeight: 24; topPadding: 2; bottomPadding: 2; leftPadding: 4; rightPadding: 4; onEditingFinished: backend.groupPort = text }
                        }

                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            CheckBox { id: cbAddTest; text: qsTr("包含测试"); font.pixelSize: 11; implicitHeight: 24; onCheckedChanged: backend.addTestEnabled = checked }
                            IptvSmallButton { text: qsTr("生成扫描文件"); onClicked: saveScanFileDialog.open() }
                        }
                    }
                }

                //导出播放文件（含选项、分组、按钮）
                GroupBox {
                    Layout.fillWidth: true
                    title: qsTr("导出播放文件")
                    font.pixelSize: 11

                    ColumnLayout {
                        width: parent.width
                        spacing: 4

                        //频道分组选择（自动换行）
                        Flow {
                            Layout.fillWidth: true
                            spacing: 4

                            Repeater {
                                id: groupRepeater
                                model: ["央视", "卫视", "付费", "教育", "地方", "国际", "购物", "广播", "临时", "测试"]

                                CheckBox {
                                    text: modelData
                                    font.pixelSize: 11
                                    implicitHeight: 22
                                    property bool initialized: false

                                    Component.onCompleted: {
                                        checked = backend.selectedGroups.indexOf(modelData) >= 0
                                        initialized = true
                                    }

                                    onCheckedChanged: {
                                        if (!initialized) return
                                        var groups = backend.selectedGroups.slice()
                                        var idx = groups.indexOf(modelData)
                                        if (checked && idx < 0) {
                                            groups.push(modelData)
                                        } else if (!checked && idx >= 0) {
                                            groups.splice(idx, 1)
                                        }
                                        backend.selectedGroups = groups
                                    }
                                }
                            }
                        }

                        //导出选项 + 导出按钮（同一行，垂直居中对齐）
                        RowLayout {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignVCenter
                            spacing: 6

                            CheckBox { id: cbRemoveRepeat; text: qsTr("合并相同频道"); font.pixelSize: 11; implicitHeight: 22; onCheckedChanged: backend.mergeChannels = checked }
                            CheckBox { id: cbOutAddLogo; text: qsTr("导出Logo"); font.pixelSize: 11; implicitHeight: 22; onCheckedChanged: backend.addLogoEnabled = checked }
                            CheckBox { id: cbOutAddHD; text: qsTr("导出清晰度"); font.pixelSize: 11; implicitHeight: 22; onCheckedChanged: backend.addHdSuffix = checked }
                            Item { Layout.fillWidth: true }
                            IptvSmallButton { text: qsTr("导出M3U"); onClicked: saveM3uDialog.open() }
                            IptvSmallButton { text: qsTr("导出TXT"); onClicked: saveTxtDialog.open() }
                        }
                    }
                }

                //导入频道文件
                GroupBox {
                    Layout.fillWidth: true
                    title: qsTr("导入频道文件")
                    font.pixelSize: 11

                    RowLayout {
                        width: parent.width
                        spacing: 4

                        Label { id: lbFilePath; Layout.fillWidth: true; text: backend.importFilePath; elide: Text.ElideMiddle; font.pixelSize: 11 }
                        IptvSmallButton { text: qsTr("选择文件"); onClicked: importFileDialog.open() }
                        IptvSmallButton {
                            text: qsTr("导入")
                            onClicked: {
                                if (backend.importFilePath.length > 0) {
                                    var count = backend.runImport(backend.importFilePath)
                                    if (count > 0) {
                                        importResultDialog.text = qsTr("导入成功，共导入 %1 条记录").arg(count)
                                    } else {
                                        importResultDialog.text = qsTr("导入失败，请检查文件格式")
                                    }
                                    importResultDialog.open()
                                }
                            }
                        }
                    }
                }
            }
        }

        //导入结果对话框
        Dialog {
            id: importResultDialog
            title: qsTr("导入结果")
            modal: true
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            standardButtons: Dialog.Ok
            property alias text: resultLabel.text

            Label {
                id: resultLabel
                font.pixelSize: 12
            }
        }
    }

    //组件加载完成后初始化值
    Component.onCompleted: {
        leUrlFirst.text = backend.httpUrl
        leFileUrl.text = backend.fileUrl
        leFCCUrl.text = backend.fccUrl
        cbUseFCC.checked = backend.fccEnabled
        leGroupAddress.text = backend.groupAddress
        leGroupPort.text = backend.groupPort
        cbAddTest.checked = backend.addTestEnabled
        cbUrlType.currentIndex = backend.urlTypeIndex
        cbRemoveRepeat.checked = backend.mergeChannels
        cbOutAddLogo.checked = backend.addLogoEnabled
        cbOutAddHD.checked = backend.addHdSuffix
        //分组复选框通过Repeater生成，需要单独处理绑定
    }
}
