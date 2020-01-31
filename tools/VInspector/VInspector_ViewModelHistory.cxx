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

#include <Standard_WarningsDisable.hxx>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

const int COLUMN_NAME_WIDTH = 230;
const int COLUMN_SIZE_WIDTH = 30;
const int COLUMN_POINTER_WIDTH = 70;
const int COLUMN_SHAPE_TYPE_WIDTH = 75;
const int COLUMN_AIS_NAME_WIDTH = 75;
const int COLUMN_SELECTED_WIDTH = 75;

const int COLUMN_5_WIDTH = 120;
const int COLUMN_6_WIDTH = 65;
const int COLUMN_7_WIDTH = 70;

const int HISTORY_AIS_NAME_COLUMN_WIDTH = 140;


// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ViewModelHistory::VInspector_ViewModelHistory (QObject* theParent, const int theHistoryTypesMaxAmount)
: TreeModel_ModelBase (theParent), myHistoryTypesMaxAmount (theHistoryTypesMaxAmount)
{
}

// =======================================================================
// function : InitColumns
// purpose :
// =======================================================================
void VInspector_ViewModelHistory::InitColumns()
{
  TreeModel_ModelBase::InitColumns();

  SetHeaderItem (0, TreeModel_HeaderSection ("Name", COLUMN_NAME_WIDTH));
  SetHeaderItem (1, TreeModel_HeaderSection ("Visibility", COLUMN_SIZE_WIDTH)); // visualization item
  SetHeaderItem (2, TreeModel_HeaderSection ("Size", COLUMN_SIZE_WIDTH));
  SetHeaderItem (3, TreeModel_HeaderSection ("Pointer", COLUMN_POINTER_WIDTH));
  SetHeaderItem (4, TreeModel_HeaderSection ("Shape type", COLUMN_SHAPE_TYPE_WIDTH));
  SetHeaderItem (5, TreeModel_HeaderSection ("AIS Name", COLUMN_AIS_NAME_WIDTH));
  SetHeaderItem (6, TreeModel_HeaderSection ("Selected/Highlighted", -1));

  for (int aColumnId = 0, aNbColumns = columnCount(); aColumnId < aNbColumns; aColumnId++)
  {
    VInspector_ItemHistoryRootPtr aRootItem = itemDynamicCast<VInspector_ItemHistoryRoot> (myRootItems[aColumnId]);
    aRootItem->SetHistoryTypesMaxAmount (myHistoryTypesMaxAmount);
  }
}

// =======================================================================
// function : createRootItem
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ViewModelHistory::createRootItem (const int theColumnId)
{
  return VInspector_ItemHistoryRoot::CreateItem (TreeModel_ItemBasePtr(), 0, theColumnId);
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
