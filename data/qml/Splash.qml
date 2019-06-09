/*
 * Stellarium
 * Copyright (C) 2013 Fabien Chereau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.
 */

// https://doc.qt.io/qt-5/qtquickcontrols2-customize.html#customizing-busyindicator

import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.2

Rectangle {
    color: "black"
    anchors.fill: parent
    Image {
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        source: "images/about-logo.png"
    }
    BusyIndicator {
        id: busyIndication
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 100
        width: 200
        height: 200
        // 'running' defaults to 'true'
        RotationAnimator {
            target: busyIndication
            running: true
            from: 0
            to: 360
            loops: Animation.Infinite
            duration: 1250
        }
        /*
        ColorAnimation {
            target: busyIndication
            from: "white"
            to: "black"
            duration: 200
        }*/
    }
    focus: true
    Keys.onReleased: {
        if (event.key === Qt.Key_Back) {
            event.accepted = true
            Qt.quit()
        }
    }
}

























































/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
