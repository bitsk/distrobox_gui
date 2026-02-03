import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    title: "克隆环境"
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
    
    property string sourceContainer: ""
    property alias newName: nameInput.text
    
    contentItem: ColumnLayout {
        spacing: 20
        
        Label {
            text: "正在克隆: " + root.sourceContainer
            color: "#61afef"
            font.bold: true
        }
        
        TextField {
            id: nameInput
            Layout.fillWidth: true
            placeholderText: "新容器名称"
            placeholderTextColor: "#5c6370"
            color: "white"
            background: Rectangle {
                color: "#21252b"
                border.color: parent.activeFocus ? "#61afef" : "#3e4451"
                radius: 6
            }
            selectByMouse: true
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
            text: "克隆"
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
            enabled: nameInput.text !== ""
            background: Rectangle {
                color: parent.enabled ? (parent.down ? "#528bca" : "#61afef") : "#3e4451"
                radius: 6
            }
            contentItem: Text { text: parent.text; color: parent.enabled ? "white" : "#5c6370"; font.bold: true; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
        }
    }
}
