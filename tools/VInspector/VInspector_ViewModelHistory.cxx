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

#include <inspector/VInspector_ViewModelHistory.hxx>

#include <inspector/VInspector_ItemHistoryElement.hxx>
#include <inspector/VInspector_ItemHistoryRoot.hxx>
#include <inspector/VInspector_ItemHistoryType.hxx>

#include <QStringList>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ViewModelHistory::VInspector_ViewModelHistory (QObject* theParent, const int theHistoryTypesMaxAmount)
: TreeModel_ModelBase (theParent)
{
  for (int aColumnId = 0, aNbColumns = columnCount(); aColumnId < aNbColumns; aColumnId++)
  {
    myRootItems.insert(aColumnId, VInspector_ItemHistoryRoot::CreateItem(TreeModel_ItemBasePtr(), 0, aColumnId));
    VInspector_ItemHistoryRootPtr aRootItem = itemDynamicCast<VInspector_ItemHistoryRoot>(myRootItems[aColumnId]);
    aRootItem->SetHistoryTypesMaxAmount(theHistoryTypesMaxAmount);
  }
  m_pRootItem = myRootItems[0];
}

// =======================================================================
// function : AddElement
// purpose :
// =======================================================================
void VInspector_ViewModelHistory::AddElement (const VInspector_CallBackMode& theMode, const QList<QVariant>& theInfo)
{
  // fill root item by the application
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
  {
    VInspector_ItemHistoryRootPtr aRootItem = itemDynamicCast<VInspector_ItemHistoryRoot>(myRootItems[aColId]);
    aRootItem->AddElement(theMode, theInfo);
  }
  Reset();
  EmitLayoutChanged();
}

// =======================================================================
// function : GetSelectedPointers
// purpose :
// =======================================================================
QStringList VInspector_ViewModelHistory::GetSelectedPointers (const QModelIndex& theIndex)
{
  QStringList aPointers;
  TreeModel_ItemBasePtr anItem = TreeModel_ModelBase::GetItemByIndex (theIndex);
  if (!anItem)
    return aPointers;

  VInspector_ItemHistoryTypePtr aHistoryItem = itemDynamicCast<VInspector_ItemHistoryType>(anItem);
  if (aHistoryItem)
  {
    QString aPointerInfo = aHistoryItem->PointerInfo();
    QString anOwnerInfo = aHistoryItem->OwnerInfo();

    if (!aPointerInfo.isEmpty())
      aPointers.append (aPointerInfo);
    if (!anOwnerInfo.isEmpty())
      aPointers.append (anOwnerInfo.split (", "));
  }
  else
  {
    VInspector_ItemHistoryElementPtr anElementItem = itemDynamicCast<VInspector_ItemHistoryElement>(anItem);
    if (anElementItem)
    {
      QString aPointerInfo = anElementItem->PointerInfo();
      if (!aPointerInfo.isEmpty())
        aPointers.append (aPointerInfo);
    }
  }
  return aPointers;
}

// =======================================================================
// function : headerData
// purpose :
// =======================================================================
QVariant VInspector_ViewModelHistory::headerData (int theSection, Qt::Orientation theOrientation, int theRole) const
{
  if (theOrientation != Qt::Horizontal || theRole != Qt::DisplayRole)
    return QVariant();

  switch (theSection)
  {
    case 0: return "Name";
    case 1: return "Size";
    case 2: return "Pointer";
    case 3: return "Shape type";
    case 4: return "AIS Name";
    case 5: return "Selected/Highlighted";
    default: break;
  }
  return QVariant();
}
