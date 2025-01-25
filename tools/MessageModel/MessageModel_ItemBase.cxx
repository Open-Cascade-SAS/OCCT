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

#include <inspector/MessageModel_ItemBase.hxx>
#include <inspector/MessageModel_ItemRoot.hxx>

// =======================================================================
// function : GetRootItem
// purpose :
// =======================================================================
TreeModel_ItemBasePtr MessageModel_ItemBase::GetRootItem() const
{
  TreeModel_ItemBasePtr anItem = Parent();
  while (anItem)
  {
    if (MessageModel_ItemRootPtr aThisRootItem = itemDynamicCast<MessageModel_ItemRoot>(anItem))
    {
      return aThisRootItem;
    }
    anItem = anItem->Parent();
  }
  return TreeModel_ItemBasePtr();
}
