import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../IptvComponents"

Item {
    id: checkTab

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 4

        //启动检查按钮 + 频道下拉列表
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            IptvSmallButton {
                text: qsTr("启动检查")
                enabled: !backend.checking
                onClicked: {
                    backend.startCheck(rbNormal.checked, rbOrderByIP.checked)
                }
            }

            //检测地址下拉框
            ComboBox {
                id: cbCheckAddress
                Layout.fillWidth: true
                implicitHeight: 26
                font.pixelSize: 11
                model: backend.checkListModel
                textRole: "fullText"
                currentIndex: backend.checkIndex

                onActivated: function(index) {
                    //更新后端索引后重新检测
                    backend.checkIndex = index
                    backend.recheck()
                }

                delegate: ItemDelegate {
                    width: cbCheckAddress.width
                    height: 28
                    highlighted: cbCheckAddress.currentIndex === index

                    contentItem: Row {
                        spacing: 4
                        Text {
                            text: model.bracket || ""
                            font.bold: true
                            font.pixelSize: 11
                            color: highlighted ? "#FFFFFF" : "#1565C0"
                        }
                        Text {
                            text: model.name || ""
                            font.bold: true
                            font.pixelSize: 11
                            color: highlighted ? "#FFFFFF" : "#212121"
                        }
                        Text {
                            text: " | " + (model.type || "") + " | " + (model.ipPort || "")
                            font.pixelSize: 11
                            color: highlighted ? "#FFFFFF" : "#757575"
                        }
                    }

                    background: Rectangle {
                        color: highlighted ? "#1976D2" : "transparent"
                    }
                }
            }
        }

        //检测模式
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label { text: qsTr("检测模式:"); font.pixelSize: 11 }
            ButtonGroup { id: checkTypeGroup }
            RadioButton {
                id: rbNormal
                text: qsTr("正式频道")
                font.pixelSize: 11
                implicitHeight: 22
                checked: true
                ButtonGroup.group: checkTypeGroup
            }
            RadioButton {
                id: rbTest
                text: qsTr("测试频道")
                font.pixelSize: 11
                implicitHeight: 22
                ButtonGroup.group: checkTypeGroup
            }
            Item { Layout.fillWidth: true }
        }

        //排序方式
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label { text: qsTr("排序方式:"); font.pixelSize: 11 }
            ButtonGroup { id: sortOrderGroup }
            RadioButton {
                id: rbOrderByIP
                text: qsTr("按IP排序")
                font.pixelSize: 11
                implicitHeight: 22
                checked: true
                ButtonGroup.group: sortOrderGroup
            }
            RadioButton {
                id: rbOrderById
                text: qsTr("按ID排序")
                font.pixelSize: 11
                implicitHeight: 22
                ButtonGroup.group: sortOrderGroup
            }
            Item { Layout.fillWidth: true }
        }

        //检测内容区（视频预览 + 信息）
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            Row {
                anchors.fill: parent
                spacing: 8

                //左侧：频道预览（占4/5宽度）
                Rectangle {
                    width: (parent.width - 8) * 0.8
                    height: width * 9 / 16
                    color: "#000000"
                    border.color: "#E0E0E0"
                    border.width: 1

                    Image {
                        id: framePreview
                        anchors.fill: parent
                        anchors.margins: 2
                        fillMode: Image.PreserveAspectFit
                        source: "image://frame/"
                        cache: false
                        visible: status === Image.Ready

                        //当framePixmapChanged信号触发时刷新图像
                        Connections {
                            target: backend
                            function onFramePixmapChanged() {
                                framePreview.source = ""
                                framePreview.source = "image://frame/" + Date.now()
                            }
                        }
                    }

                    Label {
                        anchors.centerIn: parent
                        text: qsTr("频道预览")
                        color: "#666666"
                        font.pixelSize: 12
                        visible: !framePreview.visible
                    }
                }

                //右侧：检测信息（占1/5宽度，单列排列）
                Column {
                    width: (parent.width - 8) * 0.2
                    height: parent.height
                    spacing: 4

                    Label { text: qsTr("台标:"); font.pixelSize: 11 }
                    Rectangle {
                        width: parent.width
                        height: 80
                        color: "#F5F5F5"
                        border.color: "#E0E0E0"
                        border.width: 1

                        Image {
                            id: lbLogo
                            anchors.fill: parent
                            anchors.margins: 4
                            fillMode: Image.PreserveAspectFit
                            source: "image://logo/"
                            cache: false

                            Connections {
                                target: backend
                                function onLogoPixmapChanged() {
                                    lbLogo.source = ""
                                    lbLogo.source = "image://logo/" + Date.now()
                                }
                            }
                        }
                    }

                    Label { text: qsTr("检测地址:"); font.pixelSize: 11 }
                    Rectangle {
                        width: parent.width
                        height: Math.max(28, lbCheckAddress.contentHeight + 4)
                        color: "#FFFFFF"
                        border.color: "#BDBDBD"
                        border.width: 1
                        radius: 2

                        TextEdit {
                            id: lbCheckAddress
                            anchors.fill: parent
                            anchors.margins: 4
                            readOnly: true
                            text: backend.checkUrl
                            font.pixelSize: 10
                            wrapMode: TextEdit.WrapAnywhere
                            selectByMouse: true
                        }
                    }

                    Label { text: qsTr("频道名称:"); font.pixelSize: 11 }
                    TextField {
                        id: leCheckName
                        width: parent.width
                        text: backend.checkName
                        font.pixelSize: 11
                        implicitHeight: 28
                        topPadding: 2; bottomPadding: 2; leftPadding: 4; rightPadding: 4
                        onTextChanged: backend.checkName = text
                    }

                    Label { text: qsTr("备注:"); font.pixelSize: 11 }
                    TextField {
                        id: leCheckNotes
                        width: parent.width
                        text: backend.checkNotes
                        font.pixelSize: 11
                        implicitHeight: 28
                        topPadding: 2; bottomPadding: 2; leftPadding: 4; rightPadding: 4
                        onTextChanged: backend.checkNotes = text
                    }

                    Label { text: qsTr("类型:"); font.pixelSize: 11 }
                    ComboBox {
                        id: cbCheckSourceType
                        width: parent.width
                        model: ["TS", "SD", "HD", "4K", "8K"]
                        font.pixelSize: 11
                        implicitHeight: 28
                        // topPadding: 1
                        // bottomPadding: 1
                        currentIndex: backend.checkSourceType
                        onActivated: function(index) { backend.checkSourceType = index }
                    }

                    //宽、高、帧率（每组一行，值与数据库不同时标红）
                    Row {
                        spacing: 4
                        Label { text: qsTr("宽:"); font.pixelSize: 11 }
                        Label {
                            id: lbCheckWidth
                            text: backend.checkWidth > 0 ? backend.checkWidth : ""
                            font.bold: true; font.pixelSize: 11
                            color: backend.valuesChanged && backend.checkWidth !== backend.dbWidth ? "#D32F2F" : "#212121"
                        }
                    }

                    Row {
                        spacing: 4
                        Label { text: qsTr("高:"); font.pixelSize: 11 }
                        Label {
                            id: lbCheckHeight
                            text: backend.checkHeight > 0 ? backend.checkHeight : ""
                            font.bold: true; font.pixelSize: 11
                            color: backend.valuesChanged && backend.checkHeight !== backend.dbHeight ? "#D32F2F" : "#212121"
                        }
                    }

                    Row {
                        spacing: 4
                        Label { text: qsTr("帧率:"); font.pixelSize: 11 }
                        Label {
                            id: lbCheckFps
                            text: backend.checkFps > 0 ? backend.checkFps : ""
                            font.bold: true; font.pixelSize: 11
                            color: backend.valuesChanged && backend.checkFps !== backend.dbFps ? "#D32F2F" : "#212121"
                        }
                    }

                    //进度（同行显示）
                    Row {
                        spacing: 4
                        Label { text: qsTr("进度:"); font.pixelSize: 11 }
                        Label { id: lbCheckCount; text: (backend.checkIndex + 1) + " / " + backend.checkTotal; font.bold: true; font.pixelSize: 11 }
                    }
                }
            }
        }

        //检测进度条
        IptvProgressBar {
            Layout.fillWidth: true
            implicitHeight: 16
            value: backend.checkProgress / 100.0
        }

        //操作按钮区
        RowLayout {
            Layout.fillWidth: true
            spacing: 4

            IptvSmallButton { text: qsTr("上一个"); enabled: backend.checkIndex > 0; onClicked: backend.prevCheck() }
            IptvSmallButton {
                text: qsTr("下一个")
                enabled: backend.checkIndex < backend.checkTotal - 1
                onClicked: {
                    if (backend.valuesChanged) {
                        confirmNextDialog.open()
                    } else {
                        backend.nextCheck()
                    }
                }
            }
            IptvSmallButton { text: qsTr("重试"); onClicked: backend.recheck() }
            IptvSmallButton { text: qsTr("删除"); style: "danger"; onClicked: { confirmRemoveDialog.open() } }
            IptvSmallButton {
                text: qsTr("修改")
                onClicked: {
                    //验证分辨率与类型是否匹配
                    var error = backend.validateSourceType(backend.checkSourceType, backend.checkWidth, backend.checkHeight)
                    if (error.length > 0) {
                        warningDialog.text = error
                        warningDialog.open()
                        return
                    }

                    //检查频道是否存在
                    if (!backend.channelExists(backend.checkName)) {
                        confirmCreateDialog.open()
                    } else {
                        doSubmit()
                    }
                }
            }
            Item { Layout.fillWidth: true }
        }
    }

    //删除确认对话框
    Dialog {
        id: confirmRemoveDialog
        title: qsTr("确认")
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        font.pixelSize: 11
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: qsTr("确定删除该源？")
            font.pixelSize: 11
        }

        onAccepted: {
            backend.removeCheck()
        }
    }

    //创建新频道确认对话框
    Dialog {
        id: confirmCreateDialog
        title: qsTr("确认创建")
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        font.pixelSize: 11
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: qsTr("频道\"%1\"不存在，是否创建？").arg(backend.checkName)
            font.pixelSize: 11
        }

        onAccepted: {
            doSubmit()
        }
    }

    //执行提交
    function doSubmit() {
        if (backend.submitCheck()) {
            successDialog.open()
        } else {
            warningDialog.text = qsTr("更新失败")
            warningDialog.open()
        }
    }

    //成功提示对话框
    Dialog {
        id: successDialog
        title: qsTr("成功")
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        font.pixelSize: 11
        standardButtons: Dialog.Ok

        Label {
            text: qsTr("频道源已更新")
            font.pixelSize: 12
        }
    }

    //下一个确认对话框（值有变化时）
    Dialog {
        id: confirmNextDialog
        title: qsTr("数据已变更")
        modal: true
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        font.pixelSize: 11
        standardButtons: Dialog.Ok | Dialog.Cancel

        Label {
            text: qsTr("检测到分辨率或帧率与数据库不一致，是否提交更新？")
            font.pixelSize: 11
        }

        onAccepted: {
            doSubmit()
            backend.nextCheck()
        }

        onRejected: {
            backend.nextCheck()
        }
    }
}
