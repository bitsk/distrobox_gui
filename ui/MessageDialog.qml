import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Dialog {
    id: root
    property string message: ""
    property string acceptText: "确定"
    property string rejectText: "取消"
    property color accentColor: "#61afef"

    width: 400
    modal: true
    dim: true
    
    // Position in center of application
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

    contentItem: Item {
        implicitHeight: msgText.implicitHeight + 60
        Text {
            id: msgText
            anchors.centerIn: parent
            width: parent.width - 40
            text: root.message
            color: "#abb2bf"
            font.pixelSize: 15
            lineHeight: 1.2
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
        }
    }

    footer: DialogButtonBox {
        alignment: Qt.AlignRight
        padding: 20
        spacing: 10
        background: Rectangle { color: "#2c313c"; radius: 8 }
        
        UKUIButton {
            text: root.rejectText
            DialogButtonBox.buttonRole: DialogButtonBox.RejectRole
            visible: root.rejectText !== ""
        }
        
        UKUIButton {
            id: acceptBtn
            text: root.acceptText
            DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
        }
    }
}