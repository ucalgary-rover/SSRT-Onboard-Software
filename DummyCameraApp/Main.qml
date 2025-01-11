import QtQuick
import QtMultimedia
import QtQuick.Controls

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Dummy Camera App")

    //  CaptureSession {
    //     id: captureSession
    //     camera: Camera {}
    //     videoOutput: output
    // }

    MediaPlayer {
        id: captureSession
        source: "http://127.0.0.1:5000/video_feed/0"
        videoOutput: output
        autoPlay: true
    }

   VideoOutput {
       id: output
       anchors.fill: parent
       rotation: 0
       fillMode: VideoOutput.Stretch
   }

   // Component.onCompleted: captureSession.camera.start()
    Component.onCompleted: captureSession.play()

    onClosing: {
        captureSession.stop();
    }

   Button {
    text: "Rotate camera"
    anchors.bottom: parent.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    onClicked: {
        output.rotation = (output.rotation + 90) % 360
    }

   }
}
