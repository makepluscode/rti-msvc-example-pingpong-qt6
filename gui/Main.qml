import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    width: 400
    height: 500
    visible: true
    title: "RTI Ping-Pong"
    color: "#121212"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 16

        // Status Card
        Rectangle {
            Layout.fillWidth: true
            height: 60
            radius: 12
            color: "#1e1e1e"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 12

                Rectangle {
                    width: 12; height: 12
                    radius: 6
                    color: ddsManager.status.indexOf("Error") >= 0 ? "#ef4444" : "#22c55e"
                }

                Text {
                    text: ddsManager.status
                    color: "#ffffff"
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }
            }
        }

        // Messages Card
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 12
            color: "#1e1e1e"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 16
                spacing: 8

                Text {
                    text: "Messages"
                    color: "#888888"
                    font.pixelSize: 12
                }

                ListView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    model: ddsManager.messages
                    clip: true
                    spacing: 4

                    delegate: Rectangle {
                        width: parent ? parent.width : 0
                        height: msgText.height + 12
                        radius: 6
                        color: "#2a2a2a"

                        Text {
                            id: msgText
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.margins: 8
                            text: modelData
                            color: "#e0e0e0"
                            font.pixelSize: 11
                            wrapMode: Text.Wrap
                        }
                    }
                }
            }
        }
    }
}
