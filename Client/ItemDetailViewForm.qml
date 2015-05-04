import QtQuick 2.4
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    id: listView
    transformOrigin: Item.Center

    property string hexcolor: "#5599FF"

    Rectangle {
        anchors.fill: parent
        color: "#FFFFFF"
    }

    QtObject {
        id: songMeta
        objectName: "songMeta"
        property string hash: ""
        property string songName: ""
        property string album: ""
        property string albumArt: "container-background.jpg"
        property string artist: ""
        property int length: 0

        function infoChanged(info)
        {
            songMeta.artist = info[0];
            songMeta.album = info[1];
            songMeta.songName = info[2];
            songMeta.albumArt = "container-background.jpg"
            albumArt.anchors.right = albumArt.parent.right
        }
        function loadingSong()
        {
            songMeta.artist = "Loading"
            songMeta.album = "Loading"
            songMeta.songName = "Loading"
            albumArt.anchors.right = albumArt.parent.left
        }
    }

    ColumnLayout {
        anchors.fill:parent

        Rectangle {
            id: albumDetail
            anchors.top: parent.top
            anchors.topMargin: parent.height * 0.05
            anchors.fill: parent
            anchors.bottomMargin: parent.height * 0.05
			color: "transparent"

            Image {
                z: 1
                id: albumArt
                source: songMeta.albumArt
                fillMode: Image.PreserveAspectFit
                height: parent.height - artistText.height - albumText.height - songText.height - spacer.height
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: parent.width * 0.05
                anchors.rightMargin: parent.width * 0.05
            }

            AnimatedImage {
                z: 0
                id: loadingGif
                source: "icons/loading.gif"
                fillMode: Image.PreserveAspectFit
                height: parent.height - artistText.height - albumText.height - songText.height - spacer.height
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.leftMargin: parent.width * 0.05
                anchors.rightMargin: parent.width * 0.05
            }

            Rectangle {
                id: spacer
                anchors.bottom: songText.top
                height: songText.height
            }

            Text {
                id: songText
                anchors.bottom: artistText.top
                x: parent.width * 0.5 - this.width * 0.5
                text: "Song: " + songMeta.songName
                font.bold: true
                font.pixelSize: 16
            }

            Text {
                id: artistText
                anchors.bottom: albumText.top
                x: parent.width * 0.5 - this.width * 0.5
                text: "Artist: " + songMeta.artist
                font.pixelSize: 16
            }

            Text {
                id: albumText
                anchors.bottom: parent.bottom
                x: parent.width * 0.5 - this.width * 0.5
                text: "Album: " + songMeta.album
                font.family: "Arial"
                font.bold: true
                font.pixelSize: 16
            }
        }
    }
}

