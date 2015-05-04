import QtQuick 2.4
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.2

Rectangle {
    id:searchTextRoot
    height: 30
    width: parent.width
    color: "#5599ff"

    TextField{
        objectName: "searchBar"
        id: searchBarTextField

        signal searchQuery(var input);

        anchors.fill:parent
        onAccepted: {
            //console.log("Search text input: " + searchBarTextField.text);
            searchBarTextField.searchQuery(searchBarTextField.text);
            mainWindow.state="showSearchResultsPane"
            searchBarTextField.selectAll();
            searchBarTextField.focus=false;

        }
    }
/*Jefferson Holbrook
A lot of the following code with opacity is a bit buggy and could be done better
For now, though, it works.*/
    MouseArea{
        anchors.fill: parent
        opacity: 1
        hoverEnabled: true
        onClicked: {
            searchBarTextField.forceActiveFocus();
        }
    }

}

