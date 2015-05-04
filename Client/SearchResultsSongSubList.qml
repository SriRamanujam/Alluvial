import QtQuick 2.0
import QtQuick.Layouts 1.1

Rectangle {
    width: searchBarTextField.width
    anchors.top: searchBarTextField.bottom
    anchors.left:parent.left

    RowLayout{
        id:songHeaderText
        height: nameHeaderBox.height
        width: parent.width

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            height: nameHeader.height
            color: "#FFFFFF"
        }

        Rectangle {
            id: nameHeaderBox
            anchors.left: parent.left
            width: parent.width * 0.4

            Text {
                id: nameHeader
                text:"Song Title"
                font.bold: true
                font.pixelSize: 16
                color: "#000000"
                width: parent.width
            }
        }

        Rectangle {
            id: artistHeaderBox
            anchors.left: nameHeaderBox.right
            width: parent.width * 0.2

            Text {
                id: artistHeader
                text:"Artist"
                font.bold: true
                font.pixelSize: 16
                color: "#000000"
                width: parent.width
            }
        }

        Rectangle {
            id: albumHeaderBox
            anchors.left: artistHeaderBox.right
            width: parent.width * 0.2

            Text {
                id: albumHeader
                text:"Album"
                font.bold: true
                font.pixelSize: 16
                color: "#000000"
                width: parent.width
            }
        }

        Rectangle {
            id: lengthHeaderBox
            anchors.left: albumHeaderBox.right
            width: parent.width * 0.2

            Text {
                id: lengthHeader
                text:"Length"
                font.bold: true
                font.pixelSize: 16
                color: "#000000"
                width: parent.width
            }
        }

    }
    Rectangle {
        anchors.bottom: parent.bottom
        height: parent.height - songHeaderText.height - searchResultsButtons.height
        width: parent.width

        ColumnLayout {
            id: songSearchResults
            height: parent.height
            width: parent.width

            ListModel {
                objectName: "songListModel"
                id: songListModel

                signal addSongToPlaylist(var name, var hash, var artist, var album, int length,
                                         var genre, int track_number)
                signal requestSong(var name, var hash, var artist, var album, int length,
                                   var genre, int track_number)

                function displaySearchResults(songNames, hashes, artists, albums, lengths, genres, trackNumbers, strLengths)
                {
                    songListModel.clear();

                    for ( var i = 0 ; i < songNames.length ; i++ )
                    {
                        var thing = {"name": songNames[i], "artist": artists[i],
                            "album": albums[i], "hash": hashes[i], "strLength": strLengths[i],
                            "length": lengths[i], "genre": genres[i], "track_number": trackNumbers[i]}
                        songListModel.insert(i, thing);
                    }
                }
            }

            Component {
                id: songDelegate
                Row {
                    width: parent.width
                    height: 20

                    MouseArea {
                        width: parent.width
                        height: 20
                        onDoubleClicked:
                        {
                            mainWindow.state = "showItemDetailView"
                            songListModel.requestSong(name, hash, artist, album, length,
                                genre, track_number)
                        }
                    }

                    Rectangle {

                        Text {
                            x: nameHeaderBox.x
                            width: nameHeaderBox.width
                            text: name
                            elide: Text.ElideRight

                            MouseArea {
                                anchors.right: parent.right
                                anchors.rightMargin: parent.width * 0.05
                                anchors.top: parent.top
                                anchors.bottom: parent.bottom

                                width: this.height
                                height: this.height

                                Image {
                                    anchors.top: parent.top
                                    anchors.bottom: parent.bottom
                                    anchors.right: parent.right
                                    anchors.left: parent.left
                                    anchors.fill: parent

                                    source: 'icons/add.png'
                                }

                                onClicked: {
                                    songListModel.addSongToPlaylist(name, hash, artist, album, length,
                                            genre, track_number)
                                }
                            }
                        }
                        Text {
                            x: artistHeaderBox.x
                            width: artistHeaderBox.width
                            text: artist
                            elide: Text.ElideRight
                        }
                        Text {
                            x: albumHeaderBox.x
                            width: albumHeaderBox.width
                            text: album
                            elide: Text.ElideRight
                        }
                        Text {
                            x: lengthHeaderBox.x
                            width: lengthHeaderBox.width
                            text: strLength
                            elide: Text.ElideRight
                        }

                    }

                }
            }

            ListView {
                id: searchResultsBySong
                anchors.fill: parent
                model: songListModel
                delegate:  songDelegate
                boundsBehavior: Flickable.StopAtBounds
            }

            Scrollbar {
                flickable: searchResultsBySong;
            }

        }
    }
}
