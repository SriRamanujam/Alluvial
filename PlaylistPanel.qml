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
                target: showPlaylistButton
                text: '>>'
            }
            PropertyChanges {
                target: trackListings
                model: hiderModel
            }
            PropertyChanges {
                target: playListPanel
                width:0
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
            PropertyChanges {
                target: trackListings
                currentIndex: trackListings.index
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

            ListModel {
                id: playlistModel
                ListElement {
                    text: ""
                }
            }

            objectName: "dropdownPlaylistOptions"
            id: dropdownPlaylistOptions
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            model: playlistModel

            function playlistChanged(newPlaylist)
            {
                dropdownPlaylistOptions.currentIndex = dropdownPlaylistOptions.find(newPlaylist);
            }

            function changePlaylistListings(newPlaylists)
            {
                playlistModel.clear();

                for ( var i = 0 ; i < newPlaylists.length ; i++ )
                {
                    playlistModel.insert(i, {"text": newPlaylists[i]});
                }
            }

            signal activePlaylistChanged(int currentIndex);

            onCurrentIndexChanged: {
                trackListings.index = 0;
                dropdownPlaylistOptions.activePlaylistChanged(dropdownPlaylistOptions.currentIndex);
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
                id: highlight
                Rectangle {
                    id: highlightRectangle
                    color: "lightsteelblue"; radius: 5
                    y: trackListings.model.currentItem.y
                    Behavior on y {
                        SpringAnimation {
                            spring: 3
                            damping: 0.2
                        }
                    }
                }
            }

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
                highlight: highlight
                property int index: 0

                signal activeSongChanged(int newIndex);
                signal deleteSongFromPlaylist(int indexToDelete)

                function changeActiveSong( newIndex )
                {
                    trackListings.index = newIndex
                    trackListings.currentIndex = newIndex;
                }

                function changeListings( newTracks )
                {
                    cppModel.clear();
                    hiderModel.clear();
                    for ( var i = 0 ; i < newTracks.length ; i++ )
                    {
                        cppModel.insert(i, {"name": newTracks[i]});
                        hiderModel.insert(i, {"text": ""});
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
                            trackListings.changeActiveSong(index);
                            trackListings.activeSongChanged(index);
                            mainWindow.state = "showItemDetailView";
                            console.log("Song clicked from playlist pane: " + name);
                        }
                    }

                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width * 0.05
                        text: '•' + name
                        elide: Text.ElideRight
                    }

                    MouseArea {
                        anchors.right: parent.right
                        anchors.rightMargin: parent.width * 0.05
                        anchors.top: parent.top
                        anchors.topMargin: parent.height * 0.05
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: parent.height * 0.05

                        width: this.height
                        height: this.height

                        Image {
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            anchors.left: parent.left
                            anchors.fill: parent

                            source: 'icons/close.png'
                        }

                        onClicked: {
                            trackListings.deleteSongFromPlaylist(index)
                        }
                    }
                }
            }
        }
    }
}
