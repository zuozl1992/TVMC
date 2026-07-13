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
                            model: ListModel {
                                ListElement { title: "频道ID"; ratio: 0.06; col: 0 }
                                ListElement { title: "名称"; ratio: 0.12; col: 1 }
                                ListElement { title: "分组"; ratio: 0.08; col: 2 }
                                ListElement { title: "城市"; ratio: 0.08; col: 3 }
                                ListElement { title: "描述"; ratio: 0.18; col: 4 }
                                ListElement { title: "备注"; ratio: 0.22; col: 5 }
                                ListElement { title: "LOGO"; ratio: 0.26; col: 6 }
                            }

                            Rectangle {
                                width: parent.width * model.ratio
                                height: 30
                                color: headerMouseArea.containsMouse ? "#BBDEFB" : "transparent"
                                border.color: "#BBDEFB"
                                border.width: 1

                                Text {
                                    anchors.centerIn: parent
                                    text: model.title + getSortIndicator(model.col)
                                    font.bold: true
                                    font.pixelSize: 11
                                    color: sortColumn === model.col ? "#0D47A1" : "#1565C0"
                                }

                                MouseArea {
                                    id: headerMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: onHeaderClicked(model.col)
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
                        height: 26
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
                                width: parent.width * 0.06; height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.channelId || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "channelId", text)
                            }

                            TextField {
                                width: parent.width * 0.12; height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.name || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "name", text)
                            }

                            TextField {
                                width: parent.width * 0.08; height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.group || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "group", text)
                            }

                            TextField {
                                width: parent.width * 0.08; height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.city || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "city", text)
                            }

                            TextField {
                                width: parent.width * 0.18; height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.describe || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "describe", text)
                            }

                            TextField {
                                width: parent.width * 0.22; height: parent.height
                                verticalAlignment: TextInput.AlignVCenter
                                text: modelData.notes || ""
                                font.pixelSize: 11
                                background: Rectangle { color: "transparent" }
                                onEditingFinished: backend.updateChannel(globalIndex, "notes", text)
                            }

                            TextField {
                                width: parent.width * 0.26; height: parent.height
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
