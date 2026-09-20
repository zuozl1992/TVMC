import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../IptvComponents"

Item {
    id: editTab

    property var channelData: []
    property int selectedRow: -1
    property int currentPage: 0
    property int pageSize: 50
    property int totalRecords: channelData.length
    property int totalPages: Math.ceil(totalRecords / pageSize)

    //排序状态
    property int sortColumn: 0
    property bool sortAscending: true

    Component.onCompleted: {
        refreshData()
    }

    //监听数据变更信号
    Connections {
        target: backend
        function onChannelDataChanged() {
            refreshData()
        }
    }

    function refreshData() {
        channelData = backend.getChannelData()
        currentPage = 0
        selectedRow = -1
        listView.model = getPageData()
    }

    //获取列标题（支持翻译）
    function getColumnTitle(col) {
        var titles = [
            qsTr("频道ID"), qsTr("频道"), qsTr("分组"), qsTr("城市"),
            qsTr("描述"), qsTr("备注"), qsTr("LOGO")
        ]
        return titles[col] || ""
    }

    //获取列宽比例
    function getColumnRatio(col) {
        var ratios = [0.10, 0.13, 0.08, 0.08, 0.18, 0.21, 0.22]
        return ratios[col] || 0.1
    }

    //点击表头排序（本地排序）
    function onHeaderClicked(col) {
        if (sortColumn === col) {
            sortAscending = !sortAscending
        } else {
            sortColumn = col
            sortAscending = true
        }
        sortData()
    }

    function sortData() {
        channelData.sort(function(a, b) {
            var va, vb
            switch(sortColumn) {
            case 0: va = a.channelId || ""; vb = b.channelId || ""; break;
            case 1: va = a.name || ""; vb = b.name || ""; break;
            case 2: va = a.group || ""; vb = b.group || ""; break;
            case 3: va = a.city || ""; vb = b.city || ""; break;
            case 4: va = a.describe || ""; vb = b.describe || ""; break;
            case 5: va = a.notes || ""; vb = b.notes || ""; break;
            default: va = a.channelId || ""; vb = b.channelId || "";
            }
            //尝试数字排序
            var na = parseInt(va), nb = parseInt(vb)
            if (!isNaN(na) && !isNaN(nb)) {
                return sortAscending ? na - nb : nb - na
            }
            var cmp = va.localeCompare(vb)
            return sortAscending ? cmp : -cmp
        })
        currentPage = 0
        listView.model = getPageData()
    }

    //获取排序指示符
    function getSortIndicator(col) {
        if (sortColumn !== col) return ""
        return sortAscending ? " ▲" : " ▼"
    }

    //获取当前页数据
    function getPageData() {
        var start = currentPage * pageSize
        var end = Math.min(start + pageSize, totalRecords)
        var result = []
        for (var i = start; i < end; i++) {
            result.push(channelData[i])
        }
        return result
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 4

        //工具栏
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 36

            Label {
                text: qsTr("共 %1 条记录").arg(totalRecords)
                font.pixelSize: 12
            }
            Item { Layout.fillWidth: true }

            //分页控件
            RowLayout {
                spacing: 8

                IptvButton {
                    text: qsTr("上一页")
                    enabled: currentPage > 0
                    onClicked: {
                        currentPage--
                        listView.model = getPageData()
                    }
                }

                Label {
                    text: qsTr("第 %1/%2 页").arg(currentPage + 1).arg(totalPages)
                    font.pixelSize: 12
                }

                IptvButton {
                    text: qsTr("下一页")
                    enabled: currentPage < totalPages - 1
                    onClicked: {
                        currentPage++
                        listView.model = getPageData()
                    }
                }
            }

            IptvButton {
                text: qsTr("刷新")
                onClicked: refreshData()
            }
        }

        //表格区域
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            border.color: "#E0E0E0"
            border.width: 1

            ColumnLayout {
                anchors.fill: parent
                spacing: 0

                //可点击的表头
                Rectangle {
                    Layout.fillWidth: true
                    height: 30
                    color: "#E3F2FD"

                    Row {
                        anchors.fill: parent
                        spacing: 0

                        Repeater {
                            model: 7

                            Rectangle {
                                width: parent.width * getColumnRatio(index)
                                height: 30
                                color: headerMouseArea.containsMouse ? "#BBDEFB" : "transparent"
                                border.color: "#BBDEFB"
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: getColumnTitle(index) + getSortIndicator(index)
                                    font.bold: true
                                    font.pixelSize: 11
                                    color: sortColumn === index ? "#0D47A1" : "#1565C0"
                                }

                                MouseArea {
                                    id: headerMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: onHeaderClicked(index)
                                }
                            }
                        }
                    }
                }

                //数据列表（可编辑）
                ListView {
                    id: listView
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    model: getPageData()
                    boundsBehavior: Flickable.StopAtBounds
                    flickableDirection: Flickable.VerticalFlick

                    delegate: Rectangle {
                        width: listView.width
                        height: 29
                        property int globalIndex: currentPage * pageSize + index
                        color: selectedRow === globalIndex ? "#E3F2FD" : (index % 2 === 0 ? "#FFFFFF" : "#F5F5F5")

                        MouseArea {
                            anchors.fill: parent
                            onClicked: selectedRow = globalIndex
                        }

                        Row {
                            anchors.fill: parent
                            spacing: 0

                            //频道ID（可编辑）
                            TextField {
                                width: parent.width * getColumnRatio(0); height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.channelId || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "channelId", text)
                            }

                            TextField {
                                width: parent.width * getColumnRatio(1); height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.name || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "name", text)
                            }

                            TextField {
                                width: parent.width * getColumnRatio(2); height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.group || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "group", text)
                            }

                            TextField {
                                width: parent.width * getColumnRatio(3); height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.city || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "city", text)
                            }

                            TextField {
                                width: parent.width * getColumnRatio(4); height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.describe || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "describe", text)
                            }

                            TextField {
                                width: parent.width * getColumnRatio(5); height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.notes || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "notes", text)
                            }

                            TextField {
                                width: parent.width * getColumnRatio(6); height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.logoName || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "logoName", text)
                            }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }
    }
}
