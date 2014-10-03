// Created: 2014-08-28
//
// Copyright (c) 2014 OPEN CASCADE SAS
//
// This file is part of commercial software by OPEN CASCADE SAS.
//
// This software is furnished in accordance with the terms and conditions
// of the contract and with the inclusion of this copyright notice.
// This software or any other copy thereof may not be provided or otherwise
// be made available to any third party.
// No ownership title to the software is transferred hereby.
//
// OPEN CASCADE SAS makes no representation or warranties with respect to the
// performance of this software, and specifically disclaims any responsibility
// for any damages, special or consequential, connected with its use.

import QtQuick 2.2
import QtQuick.Window 2.1

import QtQuick.Dialogs 1.2

import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

import AndroidQt 1.0

Window {
  id: root_window
  visible: true

  Item {
    id: root_item
    anchors.fill: parent

    AndroidQt {
      id: viewer
    }

    MouseArea {
      anchors.fill: parent

      onPressed: viewer.InitTouch(mouseX, mouseY)
      onPositionChanged: viewer.UpdateTouch (mouseX, mouseY)
    }

    // open button
    Rectangle {
      id: open_button

      // align
      anchors.top: parent.top
      anchors.left: parent.left

      // size
      width:  200
      height: 200

      color: "white"

      // image
      Image {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        source: "qrc:///icons/res/icons/ic_action_collection.png"
      }

      MouseArea {
        anchors.fill: parent
        onClicked: file_dialog.open()
      }
    }
  }

  FileDialog {
    id: file_dialog
    title: "Please choose a file"
    selectMultiple: false
    nameFilters: [ "BRep files (*.brep)", "All files (*)" ]
    onAccepted: viewer.ReadShapeFromFile(file_dialog.fileUrl)
  }
}
