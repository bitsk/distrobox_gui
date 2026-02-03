import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    title: "新建开发环境"
    width: 450
    height: 500
    modal: true
    dim: true
    
    // Position in center of application
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    
    // Custom Background
    background: Rectangle {
        color: "#2c313c"
        border.color: "#3e4451"
        radius: 8
    }
    
    // Custom Header with Drag Support
    header: Rectangle {
        color: "#2c313c"
        height: 60
        radius: 8
        
        Label {
            text: root.title
            color: "#abb2bf"
            font.pixelSize: 18
            font.bold: true
            anchors.centerIn: parent
        }
        
        MouseArea {
            anchors.fill: parent
            property real lastMouseX: 0
            property real lastMouseY: 0
            onPressed: {
                lastMouseX = mouseX
                lastMouseY = mouseY
            }
            onPositionChanged: {
                if (pressed) {
                    root.x += mouseX - lastMouseX
                    root.y += mouseY - lastMouseY
                }
            }
        }
    }
    
    // Custom Footer (Buttons)
    footer: DialogButtonBox {
        alignment: Qt.AlignRight
        padding: 20
        spacing: 10
        background: Rectangle { color: "#2c313c"; radius: 8 }
        
        Button {
            text: "取消"
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            background: Rectangle {
                color: "transparent"
                border.color: "#3e4451"
                radius: 6
            }
            contentItem: Text {
                text: parent.text
                color: "#abb2bf"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        
        Button {
            text: "创建"
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            background: Rectangle {
                color: parent.down ? "#528bca" : "#61afef"
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
    
    property string distroImage: ""
    property alias containerName: nameInput.text
    property string finalImage: imageInput.text
    property var existingNames: []
    
    // New properties
    property bool useInit: initCheck.checked
    property bool useNvidia: nvidiaCheck.checked
    property string platform: platformInput.text
    property string packages: packagesInput.text
    property string extraFlags: flagsInput.text
    property string homePath: homeInput.text
    property string volumePath: volumeInput.text
    property bool useRoot: rootCheck.checked

    function getOptions() {
        return {
            "init": useInit,
            "nvidia": useNvidia,
            "platform": platform,
            "packages": packages,
            "flags": extraFlags
        }
    }
    
    function generateUniqueName(baseName) {
// ... (rest of function)
    }
    
    onOpened: {
        if (distroImage !== "") {
            imageInput.text = distroImage
            imageInput.enabled = false
            var parts = distroImage.split(":")
            var base = parts[0].split("/").pop()
            var baseName = "my-" + base.replace(" ", "-")
            nameInput.text = generateUniqueName(baseName)
        } else {
            imageInput.text = ""
            imageInput.enabled = true
            imageInput.placeholderText = "例如: ubuntu:22.04"
            var baseName = "my-custom-container"
            nameInput.text = generateUniqueName(baseName)
        }
        // Reset advanced fields
        initCheck.checked = false
        nvidiaCheck.checked = false
        platformInput.text = ""
        packagesInput.text = ""
        flagsInput.text = ""
        homeInput.text = ""
        volumeInput.text = ""
        rootCheck.checked = false
    }
    
    // Content with ScrollView to prevent overflow
    contentItem: ScrollView {
        id: scroll
        clip: true
        ScrollBar.vertical.policy: ScrollBar.AsNeeded
        
        ColumnLayout {
            width: scroll.availableWidth
            spacing: 15
            
            Label {
                text: root.distroImage === "" ? "自定义环境配置" : "正在创建: " + root.distroImage
                color: "#61afef"
                font.bold: true
                font.pixelSize: 14
                Layout.topMargin: 10
            }
            
            // Basic
            TextField { id: imageInput; Layout.fillWidth: true; placeholderText: "镜像名称"; placeholderTextColor: "#5c6370"; color: "white"; visible: true; background: Rectangle { color: "#21252b"; border.color: parent.activeFocus ? "#61afef" : "#3e4451"; radius: 6 } selectByMouse: true }
            TextField { id: nameInput; Layout.fillWidth: true; placeholderText: "容器名称"; placeholderTextColor: "#5c6370"; color: "white"; background: Rectangle { color: "#21252b"; border.color: parent.activeFocus ? "#61afef" : "#3e4451"; radius: 6 } selectByMouse: true }
            
            // Storage
            Label { text: "存储配置"; font.bold: true; color: "#e06c75"; font.pixelSize: 12 }
            TextField { id: homeInput; Layout.fillWidth: true; placeholderText: "自定义 Home (可选)"; placeholderTextColor: "#5c6370"; color: "white"; background: Rectangle { color: "#21252b"; border.color: parent.activeFocus ? "#61afef" : "#3e4451"; radius: 6 } selectByMouse: true }
            TextField { id: volumeInput; Layout.fillWidth: true; placeholderText: "挂载卷 (宿主:容器)"; placeholderTextColor: "#5c6370"; color: "white"; background: Rectangle { color: "#21252b"; border.color: parent.activeFocus ? "#61afef" : "#3e4451"; radius: 6 } selectByMouse: true }

            // Feature Toggles
            Label { text: "功能选项"; font.bold: true; color: "#e06c75"; font.pixelSize: 12 }
            Flow {
                Layout.fillWidth: true
                spacing: 10
                
                CheckBox {
                    id: rootCheck
                    text: "Root 权限"
                    contentItem: Text { text: parent.text; color: "#abb2bf"; leftPadding: parent.indicator.width + parent.spacing; verticalAlignment: Text.AlignVCenter }
                }
                CheckBox {
                    id: initCheck
                    text: "Init 系统 (Systemd)"
                    contentItem: Text { text: parent.text; color: "#abb2bf"; leftPadding: parent.indicator.width + parent.spacing; verticalAlignment: Text.AlignVCenter }
                }
                CheckBox {
                    id: nvidiaCheck
                    text: "Nvidia 支持"
                    contentItem: Text { text: parent.text; color: "#abb2bf"; leftPadding: parent.indicator.width + parent.spacing; verticalAlignment: Text.AlignVCenter }
                }
            }
            
            // Advanced Inputs
            Label { text: "高级参数"; font.bold: true; color: "#e06c75"; font.pixelSize: 12 }
            TextField { id: platformInput; Layout.fillWidth: true; placeholderText: "架构 (例如: linux/arm64)"; placeholderTextColor: "#5c6370"; color: "white"; background: Rectangle { color: "#21252b"; border.color: parent.activeFocus ? "#61afef" : "#3e4451"; radius: 6 } selectByMouse: true }
            TextField { id: packagesInput; Layout.fillWidth: true; placeholderText: "附加包 (空格分隔)"; placeholderTextColor: "#5c6370"; color: "white"; background: Rectangle { color: "#21252b"; border.color: parent.activeFocus ? "#61afef" : "#3e4451"; radius: 6 } selectByMouse: true }
            TextField { id: flagsInput; Layout.fillWidth: true; placeholderText: "额外 Flags"; placeholderTextColor: "#5c6370"; color: "white"; background: Rectangle { color: "#21252b"; border.color: parent.activeFocus ? "#61afef" : "#3e4451"; radius: 6 } selectByMouse: true }

            Item { Layout.fillHeight: true }
        }
    }
}
