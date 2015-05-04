import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    height: parent.height
    width: parent.width

    Flickable {
        id: albumsSearchResultGrid
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.05
        anchors.bottom: parent.bottom
        anchors.bottomMargin: parent.height * 0.05
        anchors.left: parent.left
        anchors.leftMargin: parent.height * 0.05
        anchors.right: parent.right
        anchors.rightMargin: parent.height * 0.05
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
        objectName: "searchResults"
        id: searchResults
        ListElement {
            image: "container-background.jpg"
            artist: "Taylor Swift"
            albumTitle: "1989"
        }

        signal playAlbum(var albumTitle)

        function getAlbumsFromSearchResults(artists, albums)
        {
            console.log("getAlbumsFromSearchResults called")
            searchResults.clear();

            for ( var i = 0 ; i < artists.length ; i++ )
            {
                var album = {
                    "image": "container-background.jpg",
                    "artist": artists[i],
                    "albumTitle": albums[i]
                };
                searchResults.insert(i, album);
            }
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
                    mainWindow.state = "showItemDetailView"
                    searchResults.playAlbum(albumTitle)
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
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: albumSearchResultArtist
                anchors.top: albumSearchResultCoverArt.bottom
                width: parent.width
                text: artist
                elide: Text.ElideRight
                anchors.horizontalCenter: parent.horizontalCenter

            }
            Text {
                id: albumSearchResultTitle
                anchors.top: albumSearchResultArtist.bottom
                width: parent.width
                text: albumTitle
                elide: Text.ElideRight
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

    }
}

