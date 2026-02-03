import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

Rectangle {

    id: root

    height: 120

    color: cardMouseArea.containsMouse ? "#323844" : "#2c313c"

    radius: 10

    border.color: cardMouseArea.containsMouse ? "#4b5263" : "#3e4451"

    border.width: 1

    

    Behavior on color { ColorAnimation { duration: 150 } }

    Behavior on border.color { ColorAnimation { duration: 150 } }



        property var containerData: ({}) 



        property var rootWindow: null



        signal enterClicked()

    signal stopClicked()

    signal deleteClicked()

    signal cloneClicked()

    signal upgradeClicked()

    signal exportClicked()

    

    MouseArea {

        id: cardMouseArea

        anchors.fill: parent

        hoverEnabled: true

    }

    

    RowLayout {

        anchors.fill: parent

        anchors.margins: 15

        spacing: 15

        

        // Icon

        Rectangle {

            width: 56

            height: 56

            radius: 8

            color: "#21252b"

            Layout.alignment: Qt.AlignVCenter

            

            Image {

                anchors.centerIn: parent

                source: getIconSource(containerData ? containerData.image : "")

                sourceSize.width: 40

                sourceSize.height: 40

            }

        }

        

                // Info

        

                ColumnLayout {

        

                    Layout.fillWidth: true

        

                    Layout.minimumWidth: 100 // Allow it to shrink

        

                    spacing: 2

        

                    

        

                    Text {

        

                        text: containerData ? containerData.name : ""

        

                        color: "white"

        

                        font.pixelSize: 17

        

                        font.bold: true

        

                        elide: Text.ElideRight

        

                        Layout.fillWidth: true

        

                    }

        

                    

        

                    Text {

        

                        text: containerData ? containerData.image : ""

        

                        color: "#5c6370"

        

                        font.pixelSize: 12

        

                        elide: Text.ElideRight

        

                        Layout.fillWidth: true

        

                    }

        

                    

        

                    Item { Layout.preferredHeight: 4 }

        

        

        

                    Rectangle {

        

                        width: statusText.width + 12

        

                        height: 20

        

                        radius: 4

        

                        color: (containerData && containerData.isRunning) ? Qt.rgba(152, 195, 121, 0.15) : Qt.rgba(224, 108, 117, 0.15)

        

                        

        

                        Text {

        

                            id: statusText

        

                            anchors.centerIn: parent

        

                            text: (containerData && containerData.isRunning) ? "● 运行中" : "○ 已停止"

        

                            color: (containerData && containerData.isRunning) ? "#98c379" : "#e06c75"

        

                            font.bold: true

        

                            font.pixelSize: 11

        

                        }

        

                    }

        

                }

        

                

        

                // Actions Column - Fixed width to prevent jumping

        

                ColumnLayout {

        

                    id: actionsCol

        

                    spacing: 8

        

                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter

        

                    Layout.minimumWidth: 140

        

                    Layout.maximumWidth: 140

        

                    Layout.preferredWidth: 140

        

                    

        

                    Button {

        

                        id: enterBtn

        

                        text: "进入终端"

        

                        Layout.fillWidth: true

        

                        Layout.preferredHeight: 34

        

                        hoverEnabled: true

        

                        onClicked: root.enterClicked()

        

                        background: Rectangle {

        

                            color: enterBtn.down ? "#528bca" : (enterBtn.hovered ? "#71bef2" : "#61afef")

        

                            radius: 6

        

                        }

        

                        contentItem: Text {

        

                            text: parent.text

        

                            color: "white"

        

                            font.bold: true

        

                            font.pixelSize: 13

        

                            horizontalAlignment: Text.AlignHCenter

        

                            verticalAlignment: Text.AlignVCenter

        

                        }

        

                    }

        

                    

        

                    RowLayout {

        

                        spacing: 8

        

                        Layout.fillWidth: true

        

                        

        

                        Button {

        

                            id: stopBtn

        

                            text: "⏹ 停止"

        

                            Layout.fillWidth: true

        

                            Layout.preferredHeight: 32

        

                            hoverEnabled: true

        

                            enabled: containerData && containerData.isRunning

        

                            onClicked: root.stopClicked()

        

                            background: Rectangle {

        

                                color: stopBtn.down ? "#3e4451" : (stopBtn.hovered ? "#4b5263" : "#353b45")

        

                                radius: 6

        

                                opacity: stopBtn.enabled ? 1.0 : 0.3

        

                            }

        

                            contentItem: Text {

        

                                text: parent.text

        

                                color: stopBtn.enabled ? "#e06c75" : "#5c6370"

        

                                font.pixelSize: 12

        

                                font.bold: true

        

                                horizontalAlignment: Text.AlignHCenter

        

                                verticalAlignment: Text.AlignVCenter

        

                            }

        

                        }

        

                        

        

                                                                        Button {

        

                        

        

                                                                            id: menuBtn

        

                        

        

                                                                            text: "⋮" 

        

                        

        

                                                                            Layout.preferredWidth: 32

        

                        

        

                                                                            Layout.preferredHeight: 32

        

                        

        

                                                                            hoverEnabled: true

        

                        

        

                                                                            onClicked: {

        

                        

        

                                                                                if (!rootWindow) {

        

                        

        

                                                                                    console.error("rootWindow not set for ContainerCard!")

        

                        

        

                                                                                    return

        

                        

        

                                                                                }

        

                        

        

                                                                                var parentItem = rootWindow.contentItem

        

                        

        

                                                                                var pos = menuBtn.mapToItem(parentItem, 0, menuBtn.height)

        

                        

        

                                                                                optionsMenu.x = pos.x + menuBtn.width - optionsMenu.width

        

                        

        

                                                                                optionsMenu.y = pos.y + 4

        

                        

        

                                                                                optionsMenu.open()

        

                        

        

                                                                            }

        

                        

        

                                                                            background: Rectangle {

        

                        

        

                                                                                color: menuBtn.down ? "#3e4451" : (menuBtn.hovered ? "#4b5263" : "#353b45")

        

                        

        

                                                                                radius: 6

        

                        

        

                                                                            }

        

                        

        

                                                                            contentItem: Text {

        

                        

        

                                                                                text: parent.text

        

                        

        

                                                                                color: "white"

        

                        

        

                                                                                font.pixelSize: 18

        

                        

        

                                                                                font.bold: true

        

                        

        

                                                                                horizontalAlignment: Text.AlignHCenter

        

                        

        

                                                                                verticalAlignment: Text.AlignVCenter

        

                        

        

                                                                            }

        

                        

        

                                                                            

        

                        

        

                                                                            Menu {

        

                        

        

                                                                                id: optionsMenu

        

                        

        

                                                                                width: 160

        

                        

        

                                                                                parent: rootWindow ? rootWindow.contentItem : null

        

                        

        

                                                                                

        

                        

        

                                                                                MenuItem { text: "克隆环境"; onTriggered: root.cloneClicked() }

        

                        

        

                                                                                MenuItem { text: "导出应用"; onTriggered: root.exportClicked() }

        

                        

        

                                                                                MenuItem { text: "升级包"; onTriggered: root.upgradeClicked() }

        

                        

        

                                                                                MenuSeparator { contentItem: Rectangle { implicitHeight: 1; color: "#3e4451" } }

        

                        

        

                                                                                MenuItem { 

        

                        

        

                                                                                    text: "删除环境"

        

                        

        

                                                                                    onTriggered: root.deleteClicked()

        

                        

        

                                                                                    contentItem: Text {

        

                        

        

                                                                                        text: "删除环境"

        

                        

        

                                                                                        color: "#e06c75"

        

                        

        

                                                                                        font.bold: true

        

                        

        

                                                                                        verticalAlignment: Text.AlignVCenter

        

                        

        

                                                                                        leftPadding: 10

        

                        

        

                                                                                    }

        

                        

        

                                                                                }

        

                        

        

                                                                                

        

                        

        

                                                                                background: Rectangle {

        

                        

        

                                                                                    color: "#2c313c"

        

                        

        

                                                                                    border.color: "#3e4451"

        

                        

        

                                                                                    radius: 8

        

                        

        

                                                                                }

        

                        

        

                                                                            }

        

                        

        

                                                                        }

        

                    }

        

                }

        

            }
    
    function getIconSource(imageName) {
        if (!imageName) return "qrc:/assets/icons/linux.svg"
        let lower = imageName.toLowerCase()
        if (lower.includes("ubuntu")) return "qrc:/assets/icons/ubuntu.svg"
        if (lower.includes("fedora")) return "qrc:/assets/icons/fedora.svg"
        if (lower.includes("arch")) return "qrc:/assets/icons/arch.svg"
        if (lower.includes("debian")) return "qrc:/assets/icons/debian.svg"
        if (lower.includes("alpine")) return "qrc:/assets/icons/alpine.svg"
        if (lower.includes("kali")) return "qrc:/assets/icons/kali.svg"
        if (lower.includes("kylin")) return "qrc:/assets/icons/openkylin.svg"
        return "qrc:/assets/icons/linux.svg"
    }
}
