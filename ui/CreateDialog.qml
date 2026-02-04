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
    
    parent: Overlay.overlay
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)
    
    background: Rectangle {
        color: "#2c313c"
        border.color: "#3e4451"
        radius: 8
    }
    
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
    
    footer: DialogButtonBox {
        alignment: Qt.AlignRight
        padding: 20
        spacing: 10
        background: Rectangle { color: "#2c313c"; radius: 8 }
        
        UKUIButton {
            text: "取消"
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
        }
        
        UKUIButton {
            text: "创建"
            enabled: nameInput.text.length > 0 && 
                     existingNames.indexOf(nameInput.text) === -1 &&
                     /^[a-zA-Z0-9][a-zA-Z0-9_.-]*$/.test(nameInput.text)
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
    }
    
    property string distroImage: ""
    property alias containerName: nameInput.text
    property string finalImage: imageInput.text
    property var existingNames: []
    
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
        var name = baseName
        var counter = 1
        while (existingNames.indexOf(name) !== -1) {
            name = baseName + "-" + counter
            counter++
        }
        return name
    }
    
    onOpened: {
        if (distroImage !== "") {
            imageInput.text = distroImage
            imageInput.enabled = false
            var parts = distroImage.split(":")
            var base = parts[0].split("/").pop()
            // Ensure base name is valid (replace non-allowed chars with -)
            var baseName = "my-" + base.replace(/[^a-zA-Z0-9_.-]/g, "-")
            nameInput.text = generateUniqueName(baseName)
        } else {
            imageInput.text = ""
            imageInput.enabled = true
            imageInput.placeholderText = "例如: ubuntu:22.04"
            var baseName = "my-custom-container"
            nameInput.text = generateUniqueName(baseName)
        }
        initCheck.checked = false
        nvidiaCheck.checked = false
        platformInput.text = ""
        packagesInput.text = ""
        flagsInput.text = ""
        homeInput.text = ""
        volumeInput.text = ""
        rootCheck.checked = false
    }
    
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
            
    UKUITextField { id: imageInput; Layout.fillWidth: true; placeholderText: "镜像名称"; visible: true; selectByMouse: true }
            UKUITextField { 
                id: nameInput
                Layout.fillWidth: true
                placeholderText: "容器名称"
                selectByMouse: true
                onTextChanged: {
                    if (text.length > 0) {
                        text = text.replace(/[^a-zA-Z0-9_.-]/g, "")
                    }
                }
            }
            
            ColumnLayout {
                spacing: 2
                Layout.fillWidth: true
                visible: nameInput.text.length === 0 || existingNames.indexOf(nameInput.text) !== -1 || !/^[a-zA-Z0-9]/.test(nameInput.text)
                
                Label {
                    visible: nameInput.text.length > 0 && existingNames.indexOf(nameInput.text) !== -1
                    text: "容器名称已存在！"
                    color: "#e06c75"
                    font.pixelSize: 11
                }

                Label {
                    visible: nameInput.text.length === 0
                    text: "容器名称不能为空"
                    color: "#e06c75"
                    font.pixelSize: 11
                }

                Label {
                    visible: nameInput.text.length > 0 && !/^[a-zA-Z0-9]/.test(nameInput.text)
                    text: "名称必须以字母或数字开头"
                    color: "#e06c75"
                    font.pixelSize: 11
                }
            }
            
            Label { text: "存储配置"; font.bold: true; color: "#e06c75"; font.pixelSize: 12 }
            UKUITextField { id: homeInput; Layout.fillWidth: true; placeholderText: "自定义 Home (可选)"; selectByMouse: true }
            UKUITextField { id: volumeInput; Layout.fillWidth: true; placeholderText: "挂载卷 (宿主:容器)"; selectByMouse: true }

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
            
            Label { text: "高级参数"; font.bold: true; color: "#e06c75"; font.pixelSize: 12 }
            UKUITextField { id: platformInput; Layout.fillWidth: true; placeholderText: "架构 (例如: linux/arm64)"; selectByMouse: true }
            UKUITextField { id: packagesInput; Layout.fillWidth: true; placeholderText: "附加包 (空格分隔)"; selectByMouse: true }
            UKUITextField { id: flagsInput; Layout.fillWidth: true; placeholderText: "额外 Flags"; selectByMouse: true }

            Item { Layout.fillHeight: true }
        }
    }
}