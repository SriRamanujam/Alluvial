import QtQuick 2.4
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2

Rectangle {
    id: login_info_screen
    width: parent.width
    height: parent.height
    color: "#04ACf4"  //THIS COLOR THOUGH!!!!!!!!!   HNNNNNNNGGGGGG!!!!!!!!!!!

    Component.onCompleted: {
        login_info_screen.state="slideToCenter"
    }

    signal loginRequest(string username, string password);

    states: State{
        name: "slideToCenter"
        AnchorChanges{
            target: alluvial_welcome_msg
            anchors.right: undefined
            anchors.horizontalCenter: parent.horizontalCenter
        }

    }

    transitions:Transition {
        AnchorAnimation{
            duration: 1000
        }

    }

    Text{
        id: alluvial_welcome_msg
        text: "Welcome to Alluvial!"
        anchors.right: parent.left
        //anchors.horizontalCenter: parent.horizontalCenter
        y: parent.height/2


    }
    Text{
        id: alluvial_login_header
        text: "Please login!"
        anchors.top: alluvial_welcome_msg.bottom
        anchors.right: parent.left
        anchors.horizontalCenter: login_info_screen.horizontalCenter
    }

    TextField{
        id:alluvial_username
        placeholderText: "Username"
        anchors.top:alluvial_login_header.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        onEditingFinished: {
            loginRequest(alluvial_username.getText(), alluvial_password.getText());
            login_screen.state = "loginScreenClose";
        }
    }
    TextField{
        id: alluvial_password
        placeholderText: "password"
        anchors.top:alluvial_username.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        echoMode: TextInput.Password
        onEditingFinished: {
            loginRequest(alluvial_username.getText(), alluvial_password.getText());
            login_screen.state = "loginScreenClose";
        }
    }

}

