/*
 * Stellarium
 * Copyright (C) 2014 Guillaume Chereau
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

import QtQuick 2.2

StelDialog {
	id: root
    title: qsTr("Plugins")
	width: rootStyle.niceMenuWidth


	// XXX: I guess we could move the flickable + column into a custom item.
	Flickable {
		width: root.width
		height: Math.min(column.height, rootStyle.maxMenuHeight)
		clip: true
		contentWidth: column.width
		contentHeight: column.height
		flickableDirection: Flickable.VerticalFlick

		Column {
			id: column
			width: root.width
			height: childrenRect.height

            Rectangle {
                color: "black"
                opacity: 0
                anchors {
                    fill: contentItem
                    topMargin: root.withMargins ? -rootStyle.margin : 0
                    bottomMargin: root.withMargins ? -rootStyle.margin : 0
                }

                Text {
                    anchors.centerIn: parent
                    text: "Quasars"
                    color: "white"
                    font.pixelSize: rootStyle.fontLargeSize
                }

            }

//			StelListItem {
//				checkbox: true
//                action: "actionDistribution_Quasars"
//                setting: "Quasars/distribution_enabled"
//                text: qsTr("Distribution")
//			}


            StelButton {
                text: qsTr("Update Comets (GVB)")
                action: "action_updatecomets1"
            }
            StelButton {
                text: qsTr("Update Comets (MPC)")
                action: "action_updatecomets2"
            }
		}
	}
}
