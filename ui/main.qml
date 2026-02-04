import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import Distrobox 1.0
import org.ukui.quick.items 1.0
import org.ukui.quick.platform 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 1000
    height: 600
    title: "Kylin 开发环境管理器"
    
    color: GlobalTheme.windowActive.pureColor
    
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
        background: DtThemeBackground {
            backgroundColor: GlobalTheme.baseActive
            border.width: 0
        }
        RowLayout {
            anchors.fill: parent
            spacing: 15
            
            ToolButton {
                text: "📦"
                font.pixelSize: 24
                background: null
            }
            
            DtThemeText {
                text: "开发环境管理器"
                font.pixelSize: 20
                font.bold: true
                textColor: GlobalTheme.textActive
            }
            
            Item { Layout.fillWidth: true }
            
            UKUIButton {
                text: "🔄 刷新"
                onClicked: refreshContainers()
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
                
                DtThemeText {
                    text: "正在运行的环境"
                    font.pixelSize: 16
                    font.bold: true
                    textColor: GlobalTheme.textActive
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
                
                DtThemeText {
                    text: "新建开发环境"
                    font.pixelSize: 16
                    font.bold: true
                    textColor: GlobalTheme.kBrandNormal
                }
                
                ScrollView {
                    id: scroll
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    clip: true
                    
                    Column {
                        width: scroll.availableWidth
                        spacing: 20
                        padding: 5
                        
                        Repeater {
                            model: distroModel
                            delegate: Column {
                                width: scroll.availableWidth
                                spacing: 10
                                
                                DtThemeText {
                                    text: modelData.category
                                    font.pixelSize: 14
                                    font.bold: true
                                    textColor: GlobalTheme.textActive
                                    opacity: 0.8
                                    width: parent.width
                                }
                                
                                Flow {
                                    width: parent.width
                                    spacing: 12
                                    
                                    Repeater {
                                        model: modelData.items
                                        delegate: Item {
                                            width: 120
                                            height: 140
                                            
                                            MouseArea {
                                                id: gridItemMouseArea
                                                anchors.fill: parent
                                                hoverEnabled: true
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

                                            DtThemeBackground {
                                                anchors.fill: parent
                                                anchors.margins: 5
                                                radius: 12
                                                border.width: 2
                                                backgroundColor: gridItemMouseArea.containsMouse ? GlobalTheme.kBrandHover : GlobalTheme.kComponentNormal
                                                borderColor: gridItemMouseArea.containsMouse ? GlobalTheme.kBrandNormal : GlobalTheme.kLineNormal
                                                
                                                ColumnLayout {
                                                    anchors.centerIn: parent
                                                    spacing: 5
                                                    
                                                    Image {
                                                        source: modelData.icon
                                                        sourceSize.width: 48
                                                        sourceSize.height: 48
                                                        Layout.alignment: Qt.AlignHCenter
                                                    }
                                                    
                                                    DtThemeText {
                                                        text: modelData.name
                                                        textColor: GlobalTheme.textActive
                                                        font.pixelSize: 12
                                                        Layout.alignment: Qt.AlignHCenter
                                                        elide: Text.ElideRight
                                                        Layout.maximumWidth: 100
                                                    }
                                                }
                                            }
                                        }
                                    }
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
        
        var basicItems = []
        // Add Custom option first
        basicItems.push({name: "自定义", image: "", icon: "qrc:/assets/icons/linux.svg"})
        
        if (arch === "riscv64") {
            basicItems.push({name: "Ubuntu", image: "ubuntu:latest", icon: "qrc:/assets/icons/ubuntu.svg"})
            basicItems.push({name: "Fedora", image: "fedorariscv/base:latest", icon: "qrc:/assets/icons/fedora.svg"})
            basicItems.push({name: "Arch", image: "riscfive/archlinux:latest", icon: "qrc:/assets/icons/arch.svg"})
            basicItems.push({name: "Debian", image: "debian:stable", icon: "qrc:/assets/icons/debian.svg"})
            basicItems.push({name: "openKylin", image: "openkylin/openkylin:2.0", icon: "qrc:/assets/icons/openkylin.svg"})
        } else {
            basicItems.push({name: "Ubuntu", image: "ubuntu:latest", icon: "qrc:/assets/icons/ubuntu.svg"})
            basicItems.push({name: "Fedora", image: "fedora:latest", icon: "qrc:/assets/icons/fedora.svg"})
            basicItems.push({name: "Arch", image: "archlinux:latest", icon: "qrc:/assets/icons/arch.svg"})
            basicItems.push({name: "Debian", image: "debian:stable", icon: "qrc:/assets/icons/debian.svg"})
            basicItems.push({name: "openKylin", image: "openkylin/openkylin:2.0", icon: "qrc:/assets/icons/openkylin.svg"})
            basicItems.push({name: "Kali", image: "kalilinux/kali-rolling", icon: "qrc:/assets/icons/kali.svg"})
            basicItems.push({name: "Ubuntu 22.04", image: "ubuntu:22.04", icon: "qrc:/assets/icons/ubuntu.svg"})
        }
        
        var cloudItems = []
        cloudItems.push({name: "Alpine", image: "alpine:latest", icon: "qrc:/assets/icons/alpine.svg"})
        cloudItems.push({name: "Rocky Linux", image: "rockylinux:9", icon: "qrc:/assets/icons/linux.svg"})
        cloudItems.push({name: "AlmaLinux", image: "almalinux:9", icon: "qrc:/assets/icons/linux.svg"})
        cloudItems.push({name: "CentOS Stream", image: "quay.io/centos/centos:stream9", icon: "qrc:/assets/icons/linux.svg"})

        var aiItems = []
        aiItems.push({name: "PyTorch", image: "pytorch/pytorch:latest", icon: "qrc:/assets/icons/linux.svg"})
        aiItems.push({name: "TensorFlow", image: "tensorflow/tensorflow:latest", icon: "qrc:/assets/icons/linux.svg"})
        
        var list = []
        list.push({category: "基础发行版", items: basicItems})
        list.push({category: "云原生 & 服务器", items: cloudItems})
        list.push({category: "AI & 深度学习", items: aiItems})
        
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
