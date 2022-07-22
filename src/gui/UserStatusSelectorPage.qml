/*
 * Copyright (C) 2022 by Claudio Cambra <claudio.cambra@nextcloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

import QtQuick 2.15
import QtQuick.Controls 2.15
import Style 1.0

import com.nextcloud.desktopclient 1.0 as NC

Page {
    id: page
    
    signal finished

    property NC.UserStatusSelectorModel model: NC.UserStatusSelectorModel {
        onFinished: page.finished()
    }

    property int userIndex
    onUserIndexChanged: model.load(userIndex)

    padding: Style.standardSpacing * 2

    background: Rectangle {
        color: Style.backgroundColor
        radius: Style.trayWindowRadius
    }
    
    contentItem: UserStatusSelector {
        id: userStatusSelector
        userStatusSelectorModel: model
    }
}
