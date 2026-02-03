import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import Distrobox 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 1000
    height: 600
    title: "Kylin 开发环境管理器"
    
    // Define a dark theme palette
    property color bgDark: "#282c34"
    property color bgLight: "#2c313c"
    property color accent: "#61afef"
    property color textPrimary: "#abb2bf"
    
    color: bgDark
    
    DistroboxManager {
        id: manager
        onCreateBoxFinished: {
            busyIndicator.running = false
            if (success) {
                refreshContainers()
            } else {
                console.error("Failed to create container: " + message)
                messageDialog.title = "创建失败"
                messageDialog.message = message
                messageDialog.open()
            }
        }
        onActionFinished: {
            busyIndicator.running = false
            if (success) {
                refreshContainers()
                messageDialog.title = "操作成功"
                messageDialog.message = message
                messageDialog.open()
            } else {
                messageDialog.title = "操作失败"
                messageDialog.message = message
                messageDialog.open()
            }
        }
    }
    
    header: ToolBar {
        background: Rectangle { color: bgLight }
        RowLayout {
            anchors.fill: parent
            spacing: 15
            
            ToolButton {
                text: "📦"
                font.pixelSize: 24
                background: null
            }
            
            Label {
                text: "开发环境管理器"
                font.pixelSize: 20
                font.bold: true
                color: textPrimary
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "🔄 刷新"
                onClicked: refreshContainers()
                background: Rectangle {
                    color: parent.down ? Qt.darker("#3e4451") : "#3e4451"
                    radius: 6
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font.bold: true
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
    
    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal
        
        // Left Panel: Running Containers
        Rectangle {
            SplitView.preferredWidth: window.width * 0.40
            SplitView.minimumWidth: 400
            SplitView.maximumWidth: 600
            color: "transparent"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                
                Label {
                    text: "正在运行的环境"
                    font.pixelSize: 16
                    font.bold: true
                    color: textPrimary
                }
                
                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    
                    ListView {
                        id: containerList
                        model: containerModel
                        spacing: 15
                        delegate: ContainerCard {
                            width: containerList.width
                            containerData: modelData
                            rootWindow: window
                            
                            onEnterClicked: {
                                console.log("Enter clicked for " + modelData.name)
                                manager.enterBox(modelData.name)
                                refreshTimer.restart()
                            }
                            onStopClicked: {
                                console.log("Stop clicked for " + modelData.name)
                                if (manager.stopBox(modelData.name)) refreshContainers()
                            }
                            onDeleteClicked: {
                                console.log("Delete clicked for " + modelData.name)
                                confirmDeleteDialog.containerName = modelData.name
                                confirmDeleteDialog.open()
                            }
                            onCloneClicked: {
                                cloneDialog.sourceContainer = modelData.name
                                cloneDialog.newName = modelData.name + "-clone"
                                cloneDialog.open()
                            }
                            onExportClicked: {
                                exportDialog.containerName = modelData.name
                                exportDialog.open()
                            }
                            onUpgradeClicked: {
                                manager.upgradeBox(modelData.name)
                            }
                        }
                    }
                }
            }
        }
        
        // Right Panel: Create New
        Rectangle {
            SplitView.fillWidth: true
            color: "transparent"
            
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 15
                
                Label {
                    text: "新建开发环境"
                    font.pixelSize: 16
                    font.bold: true
                    color: "#ff8c42"
                }
                
                GridView {
                    id: distroGrid
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    cellWidth: 120
                    cellHeight: 140
                    clip: true
                    model: distroModel
                    
                    delegate: Item {
                        width: distroGrid.cellWidth
                        height: distroGrid.cellHeight
                        
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 5
                            color: "transparent"
                            border.color: "#ff8c42"
                            border.width: 2
                            radius: 12
                            
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onEntered: parent.color = Qt.rgba(255, 140, 66, 0.1)
                                onExited: parent.color = "transparent"
                                onClicked: {
                                    createDialog.distroImage = modelData.image
                                    // Extract names from containerModel
                                    var names = []
                                    for (var i = 0; i < containerModel.length; i++) {
                                        names.push(containerModel[i].name)
                                    }
                                    createDialog.existingNames = names
                                    createDialog.open()
                                }
                            }
                            
                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 5
                                
                                Image {
                                    source: modelData.icon
                                    sourceSize.width: 48
                                    sourceSize.height: 48
                                    Layout.alignment: Qt.AlignHCenter
                                }
                                
                                Text {
                                    text: modelData.name
                                    color: textPrimary
                                    font.pixelSize: 12
                                    Layout.alignment: Qt.AlignHCenter
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Data Models
    property var containerModel: []
    property var distroModel: []
    
    function updateDistroList() {
        var arch = manager.getSystemArch()
        console.log("System Architecture: " + arch)
        var list = []
        
        // Add Custom option first
        list.push({name: "自定义", image: "", icon: "qrc:/assets/icons/linux.svg"})
        
        if (arch === "riscv64") {
            // RISC-V specific list
            list.push({name: "Ubuntu", image: "ubuntu:latest", icon: "qrc:/assets/icons/ubuntu.svg"})
            list.push({name: "Fedora", image: "fedorariscv/base:latest", icon: "qrc:/assets/icons/fedora.svg"}) // Special image
            list.push({name: "Arch", image: "riscfive/archlinux:latest", icon: "qrc:/assets/icons/arch.svg"}) // Added RISC-V specific Arch
            list.push({name: "Debian", image: "debian:stable", icon: "qrc:/assets/icons/debian.svg"})
            list.push({name: "openKylin", image: "openkylin/openkylin:2.0", icon: "qrc:/assets/icons/openkylin.svg"})
            list.push({name: "Alpine", image: "alpine:latest", icon: "qrc:/assets/icons/alpine.svg"})
            // Removed Kali, Ubuntu 22.04
            list.push({name: "Ubuntu 24.04", image: "ubuntu:24.04", icon: "qrc:/assets/icons/ubuntu.svg"}) // Added 24.04
        } else {
            // Standard list
            list.push({name: "Ubuntu", image: "ubuntu:latest", icon: "qrc:/assets/icons/ubuntu.svg"})
            list.push({name: "Fedora", image: "fedora:latest", icon: "qrc:/assets/icons/fedora.svg"})
            list.push({name: "Arch", image: "archlinux:latest", icon: "qrc:/assets/icons/arch.svg"})
            list.push({name: "Debian", image: "debian:stable", icon: "qrc:/assets/icons/debian.svg"})
            list.push({name: "openKylin", image: "openkylin/openkylin:2.0", icon: "qrc:/assets/icons/openkylin.svg"})
            list.push({name: "Alpine", image: "alpine:latest", icon: "qrc:/assets/icons/alpine.svg"})
            list.push({name: "Kali", image: "kalilinux/kali-rolling", icon: "qrc:/assets/icons/kali.svg"})
            list.push({name: "Ubuntu 22.04", image: "ubuntu:22.04", icon: "qrc:/assets/icons/ubuntu.svg"})
        }
        distroModel = list
    }
    
    function refreshContainers() {
        if (manager.isDistroboxInstalled()) {
            containerModel = manager.getContainersVariant()
        } else {
            installDialog.open()
        }
    }
    
    Component.onCompleted: {
        updateDistroList()
        refreshContainers()
    }
    
    // Dialogs
    CloneDialog {
        id: cloneDialog
        onAccepted: {
            busyIndicator.running = true
            manager.cloneBox(sourceContainer, newName)
        }
    }
    
    ExportDialog {
        id: exportDialog
        onAccepted: {
            busyIndicator.running = true
            manager.exportApp(containerName, appName, isBinary)
        }
    }

    MessageDialog {
        id: messageDialog
        title: ""
        message: ""
    }

    CreateDialog {
        id: createDialog
        onAccepted: {
             busyIndicator.running = true
             manager.createBox(containerName, finalImage, homePath, volumePath, useRoot, getOptions())
        }
    }
    
    MessageDialog {
        id: confirmDeleteDialog
        title: "确认删除"
        property string containerName
        message: "确定要删除容器 " + containerName + " 吗？\n此操作不可撤销！"
        accentColor: "#e06c75"
        onAccepted: {
            if (manager.deleteBox(containerName)) refreshContainers()
        }
    }
    
    MessageDialog {
        id: installDialog
        title: "安装 Distrobox"
        message: "未检测到 Distrobox。是否安装到 ~/.local？"
        onAccepted: manager.installDistrobox()
    }
    
    BusyIndicator {
        id: busyIndicator
        anchors.centerIn: parent
        running: false
    }
    
    Timer {
        id: refreshTimer
        interval: 2000
        repeat: false
        onTriggered: refreshContainers()
    }
}
