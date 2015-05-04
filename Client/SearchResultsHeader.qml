import QtQuick 2.0
import QtQuick.Controls 1.2

Rectangle {
    id: searchResultsButtons
    width: parent.width
    height: 40

    state: "songSearchResultsUpFront"

    Rectangle {
        anchors.fill: parent
        height: parent.height
        color: "#bbbbbb"
    }

    Rectangle {
        id: showSongSearchResultsButton
        color: "#ffffff"
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: showAlbumSearchResultsButton.left
        anchors.left: parent.left
        width: songSearchTitle.width + 20

        Text {
            anchors.top: parent.top
            anchors.centerIn: parent
            id: songSearchTitle
            text:"Songs"
            font.bold: true
            font.pixelSize: 30
            color: "#000000"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                searchResultsButtons.state = "songSearchResultsUpFront"
            }
        }
    }

    Rectangle {
        id: showAlbumSearchResultsButton
        color: "#dddddd"
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: showSongSearchResultsButton.right
        width: albumSearchTitle.width + 20

        Text {
            anchors.top: parent.top
            anchors.centerIn: parent
            id: albumSearchTitle
            text: "Albums"
            font.pixelSize: 30
            font.bold: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                searchResultsButtons.state = "albumSearchResultsUpFront"
            }
        }
    }

    ToolButton {
        width: parent.height
        height: parent.height
        anchors.right: parent.right
        anchors.rightMargin: 10
        iconSource: "icons/back.png"
        onClicked: {
            mainWindow.state="showItemDetailView";
            searchBarTextField.focus=true;
        }
    }

    states: [
        State {
            name: "songSearchResultsUpFront"
            PropertyChanges {
                target: showSongSearchResultsButton
                color: "#ffffff"
                z: 2
            }
            PropertyChanges {
                target: showAlbumSearchResultsButton
                color: "#dddddd"
                z: 1
            }
            PropertyChanges {
                target: searchResultsRoot
                state: "showSongSearchResults"
            }
        },
        State {
            name: "albumSearchResultsUpFront"
            PropertyChanges {
                target: showAlbumSearchResultsButton
                color: "#ffffff"
                z: 2
            }
            PropertyChanges {
                target: showSongSearchResultsButton
                color: "#dddddd"
                z: 1
            }
            PropertyChanges {
                target: searchResultsRoot
                state: "showAlbumSearchResults"
            }
        }

    ]
}

