import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
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

        ListModel {
            id: playlistModel
            ListElement {
                text: "Add New Playlist"
            }
        }

        ComboBox {
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
                var size = playlistModel.count;
                playlistModel.clear();

                for ( var i = 0 ; i < newPlaylists.length ; i++ )
                {
                    playlistModel.insert(i, {"text": newPlaylists[i]});
                }
                playlistModel.insert(i, {"text": "Add New Playlist"});

                if ( size > playlistModel.count )
                {
                    dropdownPlaylistOptions.__selectPrevItem()
                }
            }

            signal activePlaylistChanged(int currentIndex);

            onCurrentIndexChanged: {
                if (currentText == "Add New Playlist")
                {
                    prompt.visible = true
                }
                else
                {
                    trackListings.index = 0;
                    dropdownPlaylistOptions.activePlaylistChanged(dropdownPlaylistOptions.currentIndex);
                }
            }

            Dialog {
                id: prompt
                objectName: "prompt"
                visible: false
                title: "New Playlist"

                signal createNewPlaylist(var title);

                contentItem: Rectangle {
                    color: "lightskyblue"
                    implicitWidth: 200
                    implicitHeight: 100

                    Text {
                        id: promptMessage
                        text: "What should the new playlist be called?"
                        wrapMode: Text.Wrap
                        anchors.top: parent.top
                        anchors.topMargin: parent.height * 0.02
                        anchors.left: parent.left
                        anchors.leftMargin: parent.width * 0.1
                        anchors.right: parent.right
                        anchors.rightMargin: parent.width * 0.1
                        height: parent.height * 0.4
                    }

                    TextField {
                        id: promptField
                        anchors.top: promptMessage.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: parent.height * 0.4
                    }

                    Button {
                        id: promptSubmit
                        width: parent.width * 0.6
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: promptField.bottom
                        anchors.topMargin: parent.height * 0.02
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: parent.height * 0.02
                        text: "Submit"

                        onClicked: {
                            prompt.createNewPlaylist(promptField.text)
                            prompt.visible = false
                        }
                    }
                }
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

            ListModel {
                id: hiderModel
                ListElement {
                    name: ""
                }
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
                    if ( newTracks.length === 0 )
                    {
                        cppModel.insert(0, {"name": ""});
                    }

                    for ( var i = 0 ; i < newTracks.length ; i++ )
                    {
                        cppModel.insert(i, {"name": newTracks[i]});
                        hiderModel.insert(i, {"name": ""});
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
                        anchors.right: parent.right
                        anchors.rightMargin: parent.width * 0.05
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

                            if ( trackListings.currentIndex > index )
                            {
                                var ind = trackListings.currentIndex - 1;
                            }
                            else
                            {
                                var ind = trackListings.currentIndex;
                            }

                            trackListings.deleteSongFromPlaylist(index)

                            trackListings.changeActiveSong(ind)
                        }
                    }
                }
            }
        }
    }
}
