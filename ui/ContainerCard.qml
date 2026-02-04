import QtQuick 2.15
import QtQuick.Controls 2.15 as Controls
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import org.ukui.quick.items 1.0
import org.ukui.quick.platform 1.0

Item {
    id: root
    height: 120
    
    property var containerData: ({}) 
    property var rootWindow: null
    signal enterClicked()
    signal stopClicked()
    signal deleteClicked()
    signal cloneClicked()
    signal upgradeClicked()
    signal exportClicked()

    DtThemeBackground {
        anchors.fill: parent
        radius: 10
        backgroundColor: cardMouseArea.containsMouse ? GlobalTheme.kComponentHover : GlobalTheme.kComponentNormal
        borderColor: cardMouseArea.containsMouse ? GlobalTheme.kLineComponentHover : GlobalTheme.kLineComponentNormal
        border.width: 1
    }
    
    MouseArea {
        id: cardMouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: Qt.NoButton
    }
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15
        
        // Icon Wrapper
        Item {
            width: 56
            height: 56
            
            DtThemeBackground {
                anchors.fill: parent
                radius: 8
                backgroundColor: GlobalTheme.kComponentAlphaNormal
            }
            
            Image {
                anchors.centerIn: parent
                source: getIconSource(containerData ? containerData.image : "")
                sourceSize.width: 40
                sourceSize.height: 40
            }
        }
        
        ColumnLayout {
            Layout.fillWidth: true
            Layout.minimumWidth: 100
            spacing: 2
            
            DtThemeText {
                text: containerData ? containerData.name : ""
                textColor: GlobalTheme.kFontPrimary
                font.pixelSize: 17
                font.bold: true
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
            
            DtThemeText {
                text: containerData ? containerData.image : ""
                textColor: GlobalTheme.kFontSecondary
                font.pixelSize: 12
                elide: Text.ElideRight
                Layout.fillWidth: true
            }
            
            Item { Layout.preferredHeight: 4 }

            Item {
                width: statusText.width + 12
                height: 20
                
                DtThemeBackground {
                    anchors.fill: parent
                    radius: 4
                    backgroundColor: (containerData && containerData.isRunning) ? GlobalTheme.kSuccessNormal : GlobalTheme.kErrorNormal
                    opacity: 0.15
                }
                
                DtThemeText {
                    id: statusText
                    anchors.centerIn: parent
                    text: (containerData && containerData.isRunning) ? "● 运行中" : "○ 已停止"
                    textColor: (containerData && containerData.isRunning) ? GlobalTheme.kSuccessNormal : GlobalTheme.kErrorNormal
                    font.bold: true
                    font.pixelSize: 11
                }
            }
        }
        
        ColumnLayout {
            id: actionsCol
            spacing: 8
            Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
            Layout.minimumWidth: 140
            Layout.maximumWidth: 140
            Layout.preferredWidth: 140
            
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 34
                UKUIButton {
                    id: enterBtn
                    anchors.fill: parent
                    text: "进入终端"
                    highlighted: true
                    onClicked: root.enterClicked()
                }
            }
            
            RowLayout {
                spacing: 8
                Layout.fillWidth: true
                
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 32
                    UKUIButton {
                        id: stopBtn
                        anchors.fill: parent
                        text: "⏹ 停止"
                        enabled: containerData && containerData.isRunning
                        customTextColor: enabled ? GlobalTheme.kErrorNormal.pureColor : "transparent"
                        onClicked: root.stopClicked()
                        
                        Rectangle {
                            anchors.fill: parent
                            color: "transparent"
                            border.color: GlobalTheme.kErrorNormal.pureColor
                            border.width: 1
                            radius: 6
                            visible: stopBtn.enabled
                        }
                    }
                }
                
                Item {
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    UKUIButton {
                        id: menuBtn
                        anchors.fill: parent
                        text: "⋮" 
                        onClicked: optionsMenu.open()
                        
                        Rectangle {
                            anchors.fill: parent
                            color: "transparent"
                            border.color: GlobalTheme.kLineNormal.pureColor
                            border.width: 1
                            radius: 6
                        }

                        Controls.Menu {
                            id: optionsMenu
                            y: parent.height
                            x: parent.width - width
                            Controls.MenuItem { text: "克隆环境"; onTriggered: root.cloneClicked() }
                            Controls.MenuItem { text: "导出应用"; onTriggered: root.exportClicked() }
                            Controls.MenuItem { text: "升级包"; onTriggered: root.upgradeClicked() }
                            Controls.MenuItem { 
                                text: "删除环境"
                                onTriggered: root.deleteClicked()
                            }
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
