import QtQuick 2.15
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import Qt.labs.platform 1.1 as NativeDialogs
import "../"

// Custom qml modules are in /theme (and included by resources.qrc)
import Style 1.0

import com.nextcloud.desktopclient 1.0

ApplicationWindow {
    id:         trayWindow

    title:      Systray.windowTitle
    // If the main dialog is displayed as a regular window we want it to be quadratic
    width:      Systray.useNormalWindow ? Style.trayWindowHeight : Style.trayWindowWidth
    height:     Style.trayWindowHeight
    color:      "transparent"
    flags:      Systray.useNormalWindow ? Qt.Window : Qt.Dialog | Qt.FramelessWindowHint

    property int fileActivityDialogObjectId: -1

    readonly property int maxMenuHeight: Style.trayWindowHeight - Style.trayWindowHeaderHeight - 2 * Style.trayWindowBorderWidth

    function openFileActivityDialog(objectName, objectId) {
        fileActivityDialogLoader.objectName = objectName;
        fileActivityDialogLoader.objectId = objectId;
        fileActivityDialogLoader.refresh();
    }

    Component.onCompleted: Systray.forceWindowInit(trayWindow)
    onVisibleChanged: mainTrayView.reload()

    // Close tray window when focus is lost (e.g. click somewhere else on the screen)
    onActiveChanged: {
        if (!Systray.useNormalWindow && !active) {
            hide();
            Systray.isOpen = false;
        }
   }

    onClosing: Systray.isOpen = false

    background: Rectangle {
        id: trayWindowBackground
        anchors.fill:   parent
        radius: Systray.useNormalWindow ? 0.0 : Style.trayWindowRadius
        border.width:   Style.trayWindowBorderWidth
        border.color:   Style.menuBorder
        color: Style.backgroundColor
    }

    Connections {
        target: UserModel
        function onCurrentUserChanged() {
            mainTrayView.reload();
        }
    }

    Component {
        id: errorMessageDialog

        NativeDialogs.MessageDialog {
            id: dialog

            title: Systray.windowTitle

            onAccepted: destroy()
            onRejected: destroy()
        }
    }

    Connections {
        target: Systray

        function onIsOpenChanged() {
            if(Systray.isOpen) {
                mainTrayView.reload();
            }
        }

        function onShowFileActivityDialog(objectName, objectId) {
            openFileActivityDialog(objectName, objectId)
        }

        function onShowErrorMessageDialog(error) {
            var newErrorDialog = errorMessageDialog.createObject(trayWindow)
            newErrorDialog.text = error
            newErrorDialog.open()
        }
    }

    OpacityMask {
        anchors.fill: parent
        source: ShaderEffectSource {
            sourceItem: stackView
            hideSource: true
        }
        maskSource: Rectangle {
            width: trayWindow.width
            height: trayWindow.height
            radius: Systray.useNormalWindow ? 0.0 : Style.trayWindowRadius
        }
    }

    Loader {
        id: fileActivityDialogLoader

        property string objectName: ""
        property int objectId: -1

        function refresh() {
            active = true
            item.model.load(activityModel.accountState, objectId)
            item.show()
        }

        active: false
        sourceComponent: FileActivityDialog {
            title: qsTr("%1 - File activity").arg(fileActivityDialogLoader.objectName)
            onClosing: fileActivityDialogLoader.active = false
        }

        onLoaded: refresh()
    }

    Component {
        id: userStatusSelectorPageComponent
        UserStatusSelectorPage {
            onFinished: stackView.pop(mainTrayView)
        }
    }

    StackView {
        id: stackView
        anchors.fill: parent

        popEnter: Transition {
            YAnimator {
                from: 0
                to: 0
                duration: 500
            }
        }
        popExit: Transition {
            YAnimator {
                from: 0
                to: trayWindow.height
                duration: 500
                easing.type: Easing.OutCubic
            }
        }

        pushEnter: Transition {
            YAnimator {
                from: trayWindow.height
                to: 0
                duration: 500
                easing.type: Easing.OutCubic
            }
        }
        pushExit: Transition {
            YAnimator {
                from: 0
                to: 0
                duration: 500
            }
        }

        initialItem: MainTrayView {
            id: mainTrayView
            onShowUserStatusSelector: {
                const statusPage = stackView.push(userStatusSelectorPageComponent, {userIndex: index});
                statusPage.model.load(index);
            }
            onOpenFileActivityDialog: trayWindow.openFileActivityDialog(objectName, objectId)
        }
    }
}
