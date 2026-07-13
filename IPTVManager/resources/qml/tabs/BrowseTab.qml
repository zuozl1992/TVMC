import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../IptvComponents"

Item {
    id: browseTab

    property var browseData: []
    property int currentPage: 0
    property int pageSize: 50
    property int totalRecords: browseData.length
    property int totalPages: Math.ceil(totalRecords / pageSize)

    //排序状态
    property int sortColumn: 0  // 默认按频道ID排序
    property bool sortAscending: true

    Component.onCompleted: {
        refreshData()
    }

    //监听数据变更信号
    Connections {
        target: backend
        function onBrowseDataChanged() {
            loadSortedData()
        }
    }

    function refreshData() {
        backend.sortBrowseData(sortColumn, sortAscending)
    }

    function loadSortedData() {
        browseData = backend.getBrowseData()
        currentPage = 0
        listView.model = getPageData()
    }

    //点击表头排序
    function onHeaderClicked(col) {
        if (sortColumn === col) {
            sortAscending = !sortAscending
        } else {
            sortColumn = col
            sortAscending = true
        }
        refreshData()
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
            result.push(browseData[i])
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
                                ListElement { title: "频道ID"; ratio: 0.08; col: 0 }
                                ListElement { title: "频道"; ratio: 0.15; col: 1 }
                                ListElement { title: "分组"; ratio: 0.1; col: 2 }
                                ListElement { title: "地址"; ratio: 0.17; col: 3 }
                                ListElement { title: "端口"; ratio: 0.08; col: 4 }
                                ListElement { title: "宽"; ratio: 0.1; col: 5 }
                                ListElement { title: "高"; ratio: 0.1; col: 6 }
                                ListElement { title: "FPS"; ratio: 0.08; col: 7 }
                                ListElement { title: "类型"; ratio: 0.08; col: 8 }
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

                //数据列表
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
                        color: index % 2 === 0 ? "#FFFFFF" : "#F5F5F5"

                        Row {
                            anchors.fill: parent
                            spacing: 0

                            Text { width: parent.width * 0.08; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.channelId || ""; font.pixelSize: 11 }
                            Text { width: parent.width * 0.15; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.name || ""; font.pixelSize: 11; elide: Text.ElideRight }
                            Text { width: parent.width * 0.1; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.group || ""; font.pixelSize: 11; elide: Text.ElideRight }
                            Text { width: parent.width * 0.17; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.ip || ""; font.pixelSize: 11; elide: Text.ElideRight }
                            Text { width: parent.width * 0.08; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.port || ""; font.pixelSize: 11 }
                            Text { width: parent.width * 0.1; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.width || ""; font.pixelSize: 11 }
                            Text { width: parent.width * 0.1; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.height || ""; font.pixelSize: 11 }
                            Text { width: parent.width * 0.08; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.fps || ""; font.pixelSize: 11 }
                            Text { width: parent.width * 0.08; height: parent.height; verticalAlignment: Text.AlignVCenter; leftPadding: 4; text: modelData.type || ""; font.pixelSize: 11 }
                        }
                    }

                    ScrollBar.vertical: ScrollBar {}
                }
            }
        }
    }
}
