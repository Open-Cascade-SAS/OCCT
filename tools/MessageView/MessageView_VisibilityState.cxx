// Created on: 2021-04-27
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <inspector/MessageView_VisibilityState.hxx>
#include <inspector/MessageModel_ItemAlert.hxx>

#include <Message_AlertExtended.hxx>
#include <Message_AttributeStream.hxx>

#include <TopoDS_AlertAttribute.hxx>

// =======================================================================
// function : CanBeVisible
// purpose :
// =======================================================================
bool MessageView_VisibilityState::CanBeVisible (const QModelIndex& theIndex) const
{
  MessageModel_ItemAlertPtr anAlertItem = getAlertItem (theIndex);
  if (anAlertItem)
  {
    NCollection_List<Handle(Standard_Transient)> aPresentations;
    anAlertItem->Presentations (aPresentations);
    if (!aPresentations.IsEmpty())
      return true;
  }
  return false;
}

// =======================================================================
// function : SetVisible
// purpose :
// =======================================================================
bool MessageView_VisibilityState::SetVisible (const QModelIndex&, const bool, const bool)
{
  return false;
}

// =======================================================================
// function : IsVisible
// purpose :
// =======================================================================
bool MessageView_VisibilityState::IsVisible (const QModelIndex&) const
{
  return false;
}

// =======================================================================
// function : OnClicked
// purpose :
// =======================================================================
void MessageView_VisibilityState::OnClicked (const QModelIndex& theIndex)
{
  processClicked (theIndex);
  emit itemClicked (theIndex);
}

// =======================================================================
// function : getAlertItem
// purpose :
// =======================================================================
MessageModel_ItemAlertPtr MessageView_VisibilityState::getAlertItem (const QModelIndex& theIndex) const
{
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (theIndex);
  if (!anItemBase)
    return MessageModel_ItemAlertPtr();

  MessageModel_ItemAlertPtr anAlertItem = itemDynamicCast<MessageModel_ItemAlert>(anItemBase);
  return anAlertItem;
}
