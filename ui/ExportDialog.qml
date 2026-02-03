import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    title: "导出应用/二进制"
    width: 400
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
    }
    
    property string containerName: ""
    property alias appName: appInput.text
    property alias isBinary: binCheck.checked
    
    contentItem: ColumnLayout {
        spacing: 20
        
        Label {
            text: "从 " + root.containerName + " 导出"
            color: "#61afef"
            font.bold: true
        }
        
        TextField {
            id: appInput
            Layout.fillWidth: true
            placeholderText: "应用名称 (如: code) 或 路径"
            placeholderTextColor: "#5c6370"
            color: "white"
            background: Rectangle {
                color: "#21252b"
                border.color: parent.activeFocus ? "#61afef" : "#3e4451"
                radius: 6
            }
            selectByMouse: true
        }
        
        CheckBox {
            id: binCheck
            text: "导出为二进制文件 (而非桌面快捷方式)"
            contentItem: Text {
                text: parent.text
                color: "#abb2bf"
                leftPadding: parent.indicator.width + parent.spacing
                verticalAlignment: Text.AlignVCenter
            }
        }
        
        Label {
            text: "注意: 导出二进制文件将放置在 ~/.local/bin"
            color: "#98c379"
            font.pixelSize: 12
            visible: binCheck.checked
        }
    }
    
    footer: DialogButtonBox {
        alignment: Qt.AlignRight
        padding: 20
        spacing: 10
        background: Rectangle { color: "#2c313c"; radius: 8 }
        
        Button {
            text: "取消"
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            background: Rectangle { color: "transparent"; radius: 6 }
            contentItem: Text { text: parent.text; color: "#abb2bf"; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        }
        
        Button {
            text: "导出"
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            enabled: appInput.text !== ""
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#528bca" : "#61afef") : "#3e4451"
                radius: 6
            }
            contentItem: Text { text: parent.text; color: parent.enabled ? "white" : "#5c6370"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        }
    }
}
