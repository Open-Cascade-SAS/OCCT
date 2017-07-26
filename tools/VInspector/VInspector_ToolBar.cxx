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

#include <VInspector_ToolBar.hxx>

#include <QHBoxLayout>
#include <QToolButton>
#include <QWidget>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ToolBar::VInspector_ToolBar (QWidget* theParent)
: QObject (theParent)
{
  myActionsMap[VInspector_ToolActionType_UpdateId] = new QToolButton (theParent);
  myActionsMap[VInspector_ToolActionType_UpdateId]->setText ("Update");

  myActionsMap[VInspector_ToolActionType_SelectPresentationsId] = new QToolButton (theParent);
  myActionsMap[VInspector_ToolActionType_SelectPresentationsId]->setText ("Select Presentations");
  myActionsMap[VInspector_ToolActionType_SelectPresentationsId]->setCheckable (true);

  myActionsMap[VInspector_ToolActionType_SelectOwnersId] = new QToolButton (theParent);
  myActionsMap[VInspector_ToolActionType_SelectOwnersId]->setText ("Select Owners");
  myActionsMap[VInspector_ToolActionType_SelectOwnersId]->setCheckable (true);

  myMainWindow = new QWidget (theParent);

  QHBoxLayout* aLay = new QHBoxLayout (myMainWindow);
  for (QMap<VInspector_ToolActionType, QToolButton*>::ConstIterator anActionsIt = myActionsMap.begin();
       anActionsIt != myActionsMap.end(); anActionsIt++)
  {
    QToolButton* aBtn = anActionsIt.value();
    connect (aBtn, SIGNAL (clicked()), this, SLOT (onActionClicked()));
    aLay->addWidget (aBtn);
  }
}

// =======================================================================
// function : GetToolButton
// purpose :
// =======================================================================
QToolButton* VInspector_ToolBar::GetToolButton (const VInspector_ToolActionType& theActionId ) const
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
  QToolButton* aSenderBtn = (QToolButton*)sender();

  for (QMap<VInspector_ToolActionType, QToolButton*>::ConstIterator anActionsIt = myActionsMap.begin();
       anActionsIt != myActionsMap.end(); anActionsIt++)
  {
    if (anActionsIt.value() != aSenderBtn)
      continue;
    anId = anActionsIt.key();
    break;
  }

  if (anId == VInspector_ToolActionType_SelectPresentationsId && myActionsMap[VInspector_ToolActionType_SelectOwnersId]->isChecked())
    myActionsMap[VInspector_ToolActionType_SelectOwnersId]->setChecked(false);
  else if (anId == VInspector_ToolActionType_SelectOwnersId && myActionsMap[VInspector_ToolActionType_SelectPresentationsId]->isChecked())
    myActionsMap[VInspector_ToolActionType_SelectPresentationsId]->setChecked(false);

  if (anId != -1)
    emit actionClicked (anId);
}
