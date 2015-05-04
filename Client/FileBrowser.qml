import QtQuick 2.4
import QtQuick.Dialogs 1.2

FileDialog{
    id: fileBrowserDialog
    title: "Please Select a File or Directory"
    //when a file is selected
     onAccepted: {
         console.log("File/Directory Chosen: " + fileBrowserDialog.fileUrls);
         //TODO: store the file url with the other settings
         Qt.quit();

     }
     onRejected: {
         console.log("File Browser closed by cancel button");
         Qt.quit();
     }
}


