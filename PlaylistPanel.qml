import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtMultimedia 5.0

ColumnLayout {

    id: playListPanel
    width: 150
    anchors.top: parent.top
    anchors.bottom: playBackBar.top
    anchors.left: parent.left
    state: "showPlaylist"

    states: [
        State {
            name: "hidePlaylist"
            PropertyChanges {
                target: playListPanel
                width:0
            }
            PropertyChanges {
                target: showPlaylistButton
                text: '>>'
            }
            PropertyChanges {
                target: trackListings
                model: hiderModel
            }
        },
        State {
            name: "showPlaylist"
            PropertyChanges {
                target: playListPanel
                width: 150
            }
            PropertyChanges {
                target: showPlaylistButton
                text: '<<'
            }
            PropertyChanges {
                target: trackListings
                model: cppModel
            }
        }
    ]

    transitions: Transition {
            NumberAnimation {
                properties: "width";
                easing.type: Easing.InOutQuad }
        }

    ToolButton{
        id: showPlaylistButton
        anchors.left: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        text: '<<'

        onClicked: {
            if(playListPanel.state=="showPlaylist"){
                playListPanel.state="hidePlaylist"
            }
            else {
                playListPanel.state="showPlaylist"
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        color: '#E0E0E0'


        ComboBox {
            objectName: "dropdownPlaylistOptions"
            id: dropdownPlaylistOptions
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            model: playlistModel

            signal activePlaylistChanged(int currentIndex);

            onCurrentIndexChanged: {
                dropdownPlaylistOptions.activePlaylistChanged(dropdownPlaylistOptions.currentIndex);
                console.log("Calling changeTrackListings " + currentIndex);
            }
        }

        Rectangle {
            id: playlistTrackListing
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: dropdownPlaylistOptions.bottom
            anchors.bottom: parent.bottom
            color: "transparent"

            Component {
                id: track
                Rectangle {
                    color: "red"
                    id: trackItem
                    height: 20
                    width: parent.width
                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            mainWindow.state = "showItemDetailView"
                            console.log("Song clicked from playlist pane: " + name)
                        }
                    }

                }
            }

            ListModel {
                id: hiderModel
            }

            ListModel {
                id: cppModel
                ListElement {
                    name: ""
                }
            }

            ListView {
                id: trackListings
                objectName: "trackListings"
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                model: cppModel

                function changeListings( newTracks )
                {
                    cppModel.clear();
                    console.log("changeListings called: " + newTracks);
                    for ( var i = 0 ; i < newTracks.length ; i++ )
                    {
                        cppModel.insert(i, {"name": newTracks[i]});
                        console.log(newTracks[i]);
                    }
                }

                delegate: Rectangle {
                    id: trackItem
                    height: 20
                    width: parent.width
                    color: "transparent"

                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            mainWindow.state = "showItemDetailView"
                            console.log("Song clicked from playlist pane: " + name)
                        }
                    }
                    Text {
                        text: '•' + name
                    }
                }

                function setTrackListings(songNames)
                {
                    console.log("setTrackListings has been called:" + songNames);
                    console.log("Track Model Count: " + trackListings.model.count);

                    for ( var i = trackListings.model.count-1 ; i >= 0 ; i-- )
                    {
                        console.log("Song " + (i+1) + " Pre Delete: " + trackListings.model.get(i).name);
                    }

                    console.log("Song names length: " + songNames.length);
                    for (var i = 0 ; i < songNames.length ; i++)
                    {
                        var data = {name: songNames[i]};
                        cppModel.append(data);
                        console.log("Appending data " + i + ":" + songNames[i] + ": " + data.name);
                    }

                    /*for (var i = 0 ; i < trks.count ; i++)
                    {
                        console.log("Song " + (i+1) + ": " + trks.get(i).name);
                    }*/
                    //console.log("Tracks Model length: " + trackListings.model.count)
                }

            }

        }
    }
}
