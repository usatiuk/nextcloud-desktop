import QtQml 2.12
import QtQml.Models 2.1
import QtQuick 2.15
import QtQuick.Window 2.3
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import "../"

// Custom qml modules are in /theme (and included by resources.qrc)
import Style 1.0

import com.nextcloud.desktopclient 1.0

Item {
    id: trayWindowMainItem

    signal showUserStatusSelector(int index)
    signal openFileActivityDialog(string objectName, int objectId)

    property bool isUnifiedSearchActive: unifiedSearchResultsListViewSkeleton.visible
                                         || unifiedSearchResultNothingFound.visible
                                         || unifiedSearchResultsErrorLabel.visible
                                         || unifiedSearchResultsListView.visible

    anchors.fill:   parent
    clip: true
    Accessible.role: Accessible.Grouping
    Accessible.name: qsTr("Nextcloud desktop main dialog")

    function reload() {
        // HACK: reload account Instantiator immediately by restting it - could be done better I guess
        // see also id:accountMenu below
        userLineInstantiator.active = false;
        userLineInstantiator.active = true;
        accountMenu.close();
        appsMenu.close();
        syncStatus.model.load();
    }

    Rectangle {
        id: trayWindowHeaderBackground

        anchors.left:   trayWindowMainItem.left
        anchors.right:  trayWindowMainItem.right
        anchors.top:    trayWindowMainItem.top
        height:         Style.trayWindowHeaderHeight
        color:          UserModel.currentUser.headerColor

        RowLayout {
            id: trayWindowHeaderLayout

            spacing:        0
            anchors.fill:   parent

            Button {
                id: currentAccountButton

                Layout.preferredWidth:  Style.currentAccountButtonWidth
                Layout.preferredHeight: Style.trayWindowHeaderHeight
                display:                AbstractButton.IconOnly
                flat:                   true
                palette: Style.systemPalette

                Accessible.role: Accessible.ButtonMenu
                Accessible.name: qsTr("Current account")
                Accessible.onPressAction: currentAccountButton.clicked()

                // We call open() instead of popup() because we want to position it
                // exactly below the dropdown button, not the mouse
                onClicked: {
                    syncPauseButton.text = Systray.syncIsPaused ? qsTr("Resume sync for all") : qsTr("Pause sync for all")
                    if (accountMenu.visible) {
                        accountMenu.close()
                    } else {
                        accountMenu.open()
                    }
                }

                Menu {
                    id: accountMenu

                    // x coordinate grows towards the right
                    // y coordinate grows towards the bottom
                    x: (currentAccountButton.x + 2)
                    y: (currentAccountButton.y + Style.trayWindowHeaderHeight + 2)

                    width: (Style.currentAccountButtonWidth - 2)
                    height: Math.min(implicitHeight, maxMenuHeight)
                    closePolicy: Menu.CloseOnPressOutsideParent | Menu.CloseOnEscape
                    palette: Style.palette

                    background: Rectangle {
                        border.color: Style.menuBorder
                        color: Style.backgroundColor
                        radius: Style.currentAccountButtonRadius
                    }

                    contentItem: ScrollView {
                        id: accMenuScrollView
                        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

                        data: WheelHandler {
                            target: accMenuScrollView.contentItem
                        }
                        ListView {
                            implicitHeight: contentHeight
                            model: accountMenu.contentModel
                            interactive: true
                            clip: true
                            currentIndex: accountMenu.currentIndex
                        }
                    }

                    onClosed: {
                        // HACK: reload account Instantiator immediately by restting it - could be done better I guess
                        // see also onVisibleChanged above
                        userLineInstantiator.active = false;
                        userLineInstantiator.active = true;
                    }

                    Instantiator {
                        id: userLineInstantiator
                        model: UserModel
                        delegate: UserLine {
                            onShowUserStatusSelector: {
                                trayWindowMainItem.showUserStatusSelector(model.index);
                                accountMenu.close();
                            }
                        }
                        onObjectAdded: accountMenu.insertItem(index, object)
                        onObjectRemoved: accountMenu.removeItem(object)
                    }

                    MenuItem {
                        id: addAccountButton
                        height: Style.addAccountButtonHeight
                        hoverEnabled: true
                        palette: Theme.systemPalette

                        background: Item {
                            height: parent.height
                            width: parent.menu.width
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 1
                                color: parent.parent.hovered || parent.parent.visualFocus ? Style.lightHover : "transparent"
                            }
                        }

                        RowLayout {
                            anchors.fill: parent
                            spacing: 0

                            Image {
                                Layout.leftMargin: 12
                                verticalAlignment: Qt.AlignCenter
                                source: Theme.darkMode ? "qrc:///client/theme/white/add.svg" : "qrc:///client/theme/black/add.svg"
                                sourceSize.width: Style.headerButtonIconSize
                                sourceSize.height: Style.headerButtonIconSize
                            }
                            Label {
                                Layout.leftMargin: 14
                                text: qsTr("Add account")
                                color: Style.ncTextColor
                                font.pixelSize: Style.topLinePixelSize
                            }
                            // Filler on the right
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }
                        }
                        onClicked: UserModel.addAccount()

                        Accessible.role: Accessible.MenuItem
                        Accessible.name: qsTr("Add new account")
                        Accessible.onPressAction: addAccountButton.clicked()
                    }

                    MenuSeparator {
                        contentItem: Rectangle {
                            implicitHeight: 1
                            color: Style.menuBorder
                        }
                    }

                    MenuItem {
                        id: syncPauseButton
                        font.pixelSize: Style.topLinePixelSize
                        palette.windowText: Style.ncTextColor
                        hoverEnabled: true
                        onClicked: Systray.syncIsPaused = !Systray.syncIsPaused

                        background: Item {
                            height: parent.height
                            width: parent.menu.width
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 1
                                color: parent.parent.hovered || parent.parent.visualFocus ? Style.lightHover : "transparent"
                            }
                        }

                        Accessible.role: Accessible.MenuItem
                        Accessible.name: Systray.syncIsPaused ? qsTr("Resume sync for all") : qsTr("Pause sync for all")
                        Accessible.onPressAction: syncPauseButton.clicked()
                    }

                    MenuItem {
                        id: settingsButton
                        text: qsTr("Settings")
                        font.pixelSize: Style.topLinePixelSize
                        palette.windowText: Style.ncTextColor
                        hoverEnabled: true
                        onClicked: Systray.openSettings()

                        background: Item {
                            height: parent.height
                            width: parent.menu.width
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 1
                                color: parent.parent.hovered || parent.parent.visualFocus ? Style.lightHover : "transparent"
                            }
                        }

                        Accessible.role: Accessible.MenuItem
                        Accessible.name: text
                        Accessible.onPressAction: settingsButton.clicked()
                    }

                    MenuItem {
                        id: exitButton
                        text: qsTr("Exit");
                        font.pixelSize: Style.topLinePixelSize
                        palette.windowText: Style.ncTextColor
                        hoverEnabled: true
                        onClicked: Systray.shutdown()

                        background: Item {
                            height: parent.height
                            width: parent.menu.width
                            Rectangle {
                                anchors.fill: parent
                                anchors.margins: 1
                                color: parent.parent.hovered || parent.parent.visualFocus ? Style.lightHover : "transparent"
                            }
                        }

                        Accessible.role: Accessible.MenuItem
                        Accessible.name: text
                        Accessible.onPressAction: exitButton.clicked()
                    }
                }

                background: Rectangle {
                    color: parent.hovered || parent.visualFocus ? UserModel.currentUser.headerTextColor : "transparent"
                    opacity: 0.2
                }

                RowLayout {
                    id: accountControlRowLayout

                    height: Style.trayWindowHeaderHeight
                    width:  Style.currentAccountButtonWidth
                    spacing: 0

                    Image {
                        id: currentAccountAvatar

                        Layout.leftMargin: Style.trayHorizontalMargin
                        verticalAlignment: Qt.AlignCenter
                        cache: false
                        source: UserModel.currentUser.avatar != "" ? UserModel.currentUser.avatar : "image://avatars/fallbackWhite"
                        Layout.preferredHeight: Style.accountAvatarSize
                        Layout.preferredWidth: Style.accountAvatarSize

                        Accessible.role: Accessible.Graphic
                        Accessible.name: qsTr("Current account avatar")

                        Rectangle {
                            id: currentAccountStatusIndicatorBackground
                            visible: UserModel.currentUser.isConnected
                                     && UserModel.currentUser.serverHasUserStatus
                            width: Style.accountAvatarStateIndicatorSize + 2
                            height: width
                            anchors.bottom: currentAccountAvatar.bottom
                            anchors.right: currentAccountAvatar.right
                            color: UserModel.currentUser.headerColor
                            radius: width*0.5
                        }

                        Rectangle {
                            id: currentAccountStatusIndicatorMouseHover
                            visible: UserModel.currentUser.isConnected
                                     && UserModel.currentUser.serverHasUserStatus
                            width: Style.accountAvatarStateIndicatorSize + 2
                            height: width
                            anchors.bottom: currentAccountAvatar.bottom
                            anchors.right: currentAccountAvatar.right
                            color: currentAccountButton.hovered ? UserModel.currentUser.headerTextColor : "transparent"
                            opacity: 0.2
                            radius: width*0.5
                        }

                        Image {
                            id: currentAccountStatusIndicator
                            visible: UserModel.currentUser.isConnected
                                     && UserModel.currentUser.serverHasUserStatus
                            source: UserModel.currentUser.statusIcon
                            cache: false
                            x: currentAccountStatusIndicatorBackground.x + 1
                            y: currentAccountStatusIndicatorBackground.y + 1
                            sourceSize.width: Style.accountAvatarStateIndicatorSize
                            sourceSize.height: Style.accountAvatarStateIndicatorSize

                            Accessible.role: Accessible.Indicator
                            Accessible.name: UserModel.desktopNotificationsAllowed ? qsTr("Current account status is online") : qsTr("Current account status is do not disturb")
                        }
                    }

                    Column {
                        id: accountLabels
                        spacing: 0
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.leftMargin: Style.userStatusSpacing
                        Layout.fillWidth: true
                        Layout.maximumWidth: parent.width

                        Label {
                            id: currentAccountUser
                            Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                            width: Style.currentAccountLabelWidth
                            text: UserModel.currentUser.name
                            elide: Text.ElideRight
                            color: UserModel.currentUser.headerTextColor

                            font.pixelSize: Style.topLinePixelSize
                            font.bold: true
                        }

                        RowLayout {
                            id: currentUserStatus
                            visible: UserModel.currentUser.isConnected &&
                                     UserModel.currentUser.serverHasUserStatus
                            spacing: Style.accountLabelsSpacing
                            width: parent.width

                            Label {
                                id: emoji
                                visible: UserModel.currentUser.statusEmoji !== ""
                                width: Style.userStatusEmojiSize
                                text: UserModel.currentUser.statusEmoji
                            }
                            Label {
                                id: message
                                Layout.alignment: Qt.AlignLeft | Qt.AlignBottom
                                Layout.fillWidth: true
                                visible: UserModel.currentUser.statusMessage !== ""
                                width: Style.currentAccountLabelWidth
                                text: UserModel.currentUser.statusMessage !== ""
                                      ? UserModel.currentUser.statusMessage
                                      : UserModel.currentUser.server
                                elide: Text.ElideRight
                                color: UserModel.currentUser.headerTextColor
                                font.pixelSize: Style.subLinePixelSize
                            }
                        }
                    }

                    ColorOverlay {
                        cached: true
                        color: UserModel.currentUser.headerTextColor
                        width: source.width
                        height: source.height
                        source: Image {
                            Layout.alignment: Qt.AlignRight
                            verticalAlignment: Qt.AlignCenter
                            Layout.margins: Style.accountDropDownCaretMargin
                            source: "qrc:///client/theme/white/caret-down.svg"
                            sourceSize.width: Style.accountDropDownCaretSize
                            sourceSize.height: Style.accountDropDownCaretSize
                            Accessible.role: Accessible.PopupMenu
                            Accessible.name: qsTr("Account switcher and settings menu")
                        }
                    }
                }
            }

            // Add space between items
            Item {
                Layout.fillWidth: true
            }

            RowLayout {
                id: openLocalFolderRowLayout
                spacing: 0
                Layout.preferredWidth:  Style.trayWindowHeaderHeight
                Layout.preferredHeight: Style.trayWindowHeaderHeight
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

                Accessible.role: Accessible.Button
                Accessible.name: qsTr("Open local folder of current account")

                HeaderButton {
                    id: openLocalFolderButton
                    visible: UserModel.currentUser.hasLocalFolder
                    icon.source: "qrc:///client/theme/white/folder.svg"
                    icon.color: UserModel.currentUser.headerTextColor
                    onClicked: UserModel.openCurrentAccountLocalFolder()

                    Image {
                        id: folderStateIndicator
                        visible: UserModel.currentUser.hasLocalFolder
                        source: UserModel.currentUser.isConnected
                                ? Style.stateOnlineImageSource
                                : Style.stateOfflineImageSource
                        cache: false

                        anchors.top: openLocalFolderButton.verticalCenter
                        anchors.left: openLocalFolderButton.horizontalCenter
                        sourceSize.width: Style.folderStateIndicatorSize
                        sourceSize.height: Style.folderStateIndicatorSize

                        Accessible.role: Accessible.Indicator
                        Accessible.name: UserModel.currentUser.isConnected ? qsTr("Connected") : qsTr("Disconnected")
                        z: 1

                        Rectangle {
                            id: folderStateIndicatorBackground
                            width: Style.folderStateIndicatorSize + 2
                            height: width
                            anchors.centerIn: parent
                            color: UserModel.currentUser.headerColor
                            radius: width*0.5
                            z: -2
                        }

                        Rectangle {
                            id: folderStateIndicatorBackgroundMouseHover
                            width: Style.folderStateIndicatorSize + 2
                            height: width
                            anchors.centerIn: parent
                            color: openLocalFolderButton.hovered ? UserModel.currentUser.headerTextColor : "transparent"
                            opacity: 0.2
                            radius: width*0.5
                            z: -1
                        }
                    }
                }
            }

            HeaderButton {
                id: trayWindowTalkButton

                visible: UserModel.currentUser.serverHasTalk
                icon.source: "qrc:///client/theme/white/talk-app.svg"
                icon.color: UserModel.currentUser.headerTextColor
                onClicked: UserModel.openCurrentAccountTalk()

                Accessible.role: Accessible.Button
                Accessible.name: qsTr("Open Nextcloud Talk in browser")
                Accessible.onPressAction: trayWindowTalkButton.clicked()
            }

            HeaderButton {
                id: trayWindowAppsButton
                icon.source: "qrc:///client/theme/white/more-apps.svg"
                icon.color: UserModel.currentUser.headerTextColor

                onClicked: {
                    if(appsMenu.count <= 0) {
                        UserModel.openCurrentAccountServer()
                    } else if (appsMenu.visible) {
                        appsMenu.close()
                    } else {
                        appsMenu.open()
                    }
                }

                Accessible.role: Accessible.ButtonMenu
                Accessible.name: qsTr("More apps")
                Accessible.onPressAction: trayWindowAppsButton.clicked()

                AutoSizingMenu {
                    id: appsMenu
                    y: (trayWindowAppsButton.y + trayWindowAppsButton.height + 2)
                    readonly property Item listContentItem: contentItem.contentItem
                    closePolicy: Menu.CloseOnPressOutsideParent | Menu.CloseOnEscape

                    background: Rectangle {
                        border.color: Style.menuBorder
                        color: Style.backgroundColor
                        radius: 2
                    }

                    Instantiator {
                        id: appsMenuInstantiator
                        model: UserAppsModel
                        onObjectAdded: appsMenu.insertItem(index, object)
                        onObjectRemoved: appsMenu.removeItem(object)
                        delegate: MenuItem {
                            id: appEntry
                            text: appName
                            font.pixelSize: Style.topLinePixelSize
                            palette.windowText: Style.ncTextColor
                            icon.source: appIconUrl
                            icon.color: Style.ncTextColor
                            onTriggered: UserAppsModel.openAppUrl(appUrl)
                            hoverEnabled: true

                            background: Item {
                                height: parent.height
                                width: parent.width
                                Rectangle {
                                    anchors.fill: parent
                                    anchors.margins: 1
                                    color: parent.parent.hovered || parent.parent.visualFocus ? Style.lightHover : "transparent"
                                }
                            }

                            Accessible.role: Accessible.MenuItem
                            Accessible.name: qsTr("Open %1 in browser").arg(appName)
                            Accessible.onPressAction: appEntry.triggered()
                        }
                    }
                }
            }
        }
    }   // Rectangle trayWindowHeaderBackground

    UnifiedSearchInputContainer {
        id: trayWindowUnifiedSearchInputContainer
        height: Style.trayWindowHeaderHeight * 0.65

        anchors {
            top: trayWindowHeaderBackground.bottom
            left: trayWindowMainItem.left
            right: trayWindowMainItem.right

            topMargin: Style.trayHorizontalMargin + controlRoot.padding
            leftMargin: Style.trayHorizontalMargin + controlRoot.padding
            rightMargin: Style.trayHorizontalMargin + controlRoot.padding
        }

        text: UserModel.currentUser.unifiedSearchResultsListModel.searchTerm
        readOnly: !UserModel.currentUser.isConnected || UserModel.currentUser.unifiedSearchResultsListModel.currentFetchMoreInProgressProviderId
        isSearchInProgress: UserModel.currentUser.unifiedSearchResultsListModel.isSearchInProgress
        onTextEdited: { UserModel.currentUser.unifiedSearchResultsListModel.searchTerm = trayWindowUnifiedSearchInputContainer.text }
        onClearText: { UserModel.currentUser.unifiedSearchResultsListModel.searchTerm = "" }
    }

    ErrorBox {
        id: unifiedSearchResultsErrorLabel
        visible:  UserModel.currentUser.unifiedSearchResultsListModel.errorString && !unifiedSearchResultsListView.visible && ! UserModel.currentUser.unifiedSearchResultsListModel.isSearchInProgress && ! UserModel.currentUser.unifiedSearchResultsListModel.currentFetchMoreInProgressProviderId
        text:  UserModel.currentUser.unifiedSearchResultsListModel.errorString
        anchors.top: trayWindowUnifiedSearchInputContainer.bottom
        anchors.left: trayWindowMainItem.left
        anchors.right: trayWindowMainItem.right
        anchors.margins: Style.trayHorizontalMargin
    }

    UnifiedSearchResultNothingFound {
        id: unifiedSearchResultNothingFound
        visible: false
        anchors.top: trayWindowUnifiedSearchInputContainer.bottom
        anchors.left: trayWindowMainItem.left
        anchors.right: trayWindowMainItem.right
        anchors.topMargin: Style.trayHorizontalMargin

        text: UserModel.currentUser.unifiedSearchResultsListModel.searchTerm

        property bool isSearchRunning: UserModel.currentUser.unifiedSearchResultsListModel.isSearchInProgress
        property bool isSearchResultsEmpty: unifiedSearchResultsListView.count === 0
        property bool nothingFound: text && isSearchResultsEmpty && !UserModel.currentUser.unifiedSearchResultsListModel.errorString

        onIsSearchRunningChanged: {
            if (unifiedSearchResultNothingFound.isSearchRunning) {
                visible = false;
            } else {
                if (nothingFound) {
                    visible = true;
                }
            }
        }

        onTextChanged: {
            visible = false;
        }

        onIsSearchResultsEmptyChanged: {
            if (!unifiedSearchResultNothingFound.isSearchResultsEmpty) {
                visible = false;
            }
        }
    }

    UnifiedSearchResultItemSkeletonContainer {
        id: unifiedSearchResultsListViewSkeleton
        visible: !unifiedSearchResultNothingFound.visible && !unifiedSearchResultsListView.visible && ! UserModel.currentUser.unifiedSearchResultsListModel.errorString &&  UserModel.currentUser.unifiedSearchResultsListModel.searchTerm
        anchors.top: trayWindowUnifiedSearchInputContainer.bottom
        anchors.left: trayWindowMainItem.left
        anchors.right: trayWindowMainItem.right
        anchors.bottom: trayWindowMainItem.bottom
    }

    ScrollView {
        id: controlRoot
        padding: 1
        contentWidth: availableWidth

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

        data: WheelHandler {
            target: controlRoot.contentItem
        }
        visible: unifiedSearchResultsListView.count > 0

        anchors.top: trayWindowUnifiedSearchInputContainer.bottom
        anchors.left: trayWindowMainItem.left
        anchors.right: trayWindowMainItem.right
        anchors.bottom: trayWindowMainItem.bottom

        ListView {
            id: unifiedSearchResultsListView
            spacing: 4
            clip: true

            keyNavigationEnabled: true

            reuseItems: true

            Accessible.role: Accessible.List
            Accessible.name: qsTr("Unified search results list")

            model: UserModel.currentUser.unifiedSearchResultsListModel

            delegate: UnifiedSearchResultListItem {
                width: unifiedSearchResultsListView.width
                height: Style.unifiedSearchItemHeight
                isSearchInProgress:  unifiedSearchResultsListView.model.isSearchInProgress
                currentFetchMoreInProgressProviderId: unifiedSearchResultsListView.model.currentFetchMoreInProgressProviderId
                fetchMoreTriggerClicked: unifiedSearchResultsListView.model.fetchMoreTriggerClicked
                resultClicked: unifiedSearchResultsListView.model.resultClicked
                ListView.onPooled: isPooled = true
                ListView.onReused: isPooled = false
            }

            section.property: "providerName"
            section.criteria: ViewSection.FullString
            section.delegate: UnifiedSearchResultSectionItem {
                width: unifiedSearchResultsListView.width
            }
        }
    }

    SyncStatus {
        id: syncStatus

        visible: !trayWindowMainItem.isUnifiedSearchActive

        anchors.top: trayWindowUnifiedSearchInputContainer.bottom
        anchors.left: trayWindowMainItem.left
        anchors.right: trayWindowMainItem.right
    }

    ActivityList {
        visible: !trayWindowMainItem.isUnifiedSearchActive
        anchors.top: syncStatus.bottom
        anchors.left: trayWindowMainItem.left
        anchors.right: trayWindowMainItem.right
        anchors.bottom: trayWindowMainItem.bottom

        activeFocusOnTab: true
        model: activityModel
        onShowFileActivity: {
            openFileActivityDialog(objectName, objectId)
        }
        onActivityItemClicked: {
            model.slotTriggerDefaultAction(index)
        }
    }
} // trayWindowMainItem
