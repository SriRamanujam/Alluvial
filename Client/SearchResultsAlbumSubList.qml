import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    width: parent.width

    Flickable {
        id: albumsSearchResultGrid
        anchors.bottom: parent.bottom
        height: parent.height
        width: parent.width
        x: parent.width % albumGrid.cellWidth / 2

        GridView{
            id:albumGrid
            cellHeight: 150
            cellWidth: 125
            cacheBuffer: 0
            anchors.fill: parent
            flow: GridView.LeftToRight
            flickableDirection: Flickable.VerticalFlick
            model: searchResults
            delegate: albumSearchResult
            boundsBehavior: Flickable.StopAtBounds
        }

        Scrollbar {
            flickable: albumGrid;
        }
    }

    ListModel {
        id: searchResults
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }
    }

    Component {
        id: albumSearchResult

        Rectangle {
            width: 125
            height: 150
            MouseArea {
                anchors.fill:parent
                onDoubleClicked:
                {
                    console.log("Opening: " + albumTitle)
                    mainWindow.state = "showItemDetailView"
                }
                z: 1
            }
            Image {
                id: albumSearchResultCoverArt
                width: 100
                height: 100
                //color: "#FF0000"
                asynchronous: true
                source: image
                fillMode: Image.PreserveAspectFit
            }
            Text {
                id: albumSearchResultArtist
                anchors.top: albumSearchResultCoverArt.bottom
                text: artist
            }
            Text {
                id: albumSearchResultTitle
                anchors.top: albumSearchResultArtist.bottom
                text: albumTitle
            }
        }

    }
}

