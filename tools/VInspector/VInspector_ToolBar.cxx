// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement. 

#include <inspector/VInspector_ToolBar.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ToolBar::VInspector_ToolBar (QWidget* theParent)
: QObject (theParent)
{
  myActionsMap[VInspector_ToolActionType_UpdateId] = new QPushButton (theParent);
  myActionsMap[VInspector_ToolActionType_UpdateId]->setIcon (QIcon (":/icons/treeview_update.png"));
  myActionsMap[VInspector_ToolActionType_UpdateId]->setText (tr ("Update Tree Model"));
  myActionsMap[VInspector_ToolActionType_UpdateId]->setToolTip (tr ("Update Tree Model"));

  myActionsMap[VInspector_ToolActionType_UpdateId]->setText ("Update");

  myMainWindow = new QWidget (theParent);

  QHBoxLayout* aLay = new QHBoxLayout (myMainWindow);
  aLay->setMargin(0);
  for (QMap<VInspector_ToolActionType, QPushButton*>::ConstIterator anActionsIt = myActionsMap.begin();
       anActionsIt != myActionsMap.end(); anActionsIt++)
  {
    QPushButton* aBtn = anActionsIt.value();
    connect (aBtn, SIGNAL (clicked()), this, SLOT (onActionClicked()));
    aLay->addWidget (aBtn);
  }
  aLay->addStretch(1);
}

// =======================================================================
// function : GetToolButton
// purpose :
// =======================================================================
QPushButton* VInspector_ToolBar::GetToolButton (const VInspector_ToolActionType& theActionId ) const
{
  return myActionsMap.contains (theActionId) ? myActionsMap[theActionId] : 0;
}

// =======================================================================
// function : onActionClicked
// purpose :
// =======================================================================
void VInspector_ToolBar::onActionClicked()
{
  int anId = -1;
  QPushButton* aSenderBtn = (QPushButton*)sender();

  for (QMap<VInspector_ToolActionType, QPushButton*>::ConstIterator anActionsIt = myActionsMap.begin();
       anActionsIt != myActionsMap.end(); anActionsIt++)
  {
    if (anActionsIt.value() != aSenderBtn)
      continue;
    anId = anActionsIt.key();
    break;
  }

  if (anId != -1)
    emit actionClicked (anId);
}
