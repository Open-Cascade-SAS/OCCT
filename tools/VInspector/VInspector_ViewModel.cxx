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

#include <inspector/VInspector_ViewModel.hxx>

#include <inspector/TreeModel_Tools.hxx>
#include <inspector/VInspector_ItemContext.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QItemSelectionModel>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

const int COLUMN_POINTER_WIDTH = 70;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ViewModel::VInspector_ViewModel(QObject* theParent)
    : TreeModel_ModelBase(theParent)
{
}

// =======================================================================
// function : InitColumns
// purpose :
// =======================================================================
void VInspector_ViewModel::InitColumns()
{
  TreeModel_ModelBase::InitColumns();

  setHeaderItem(3, TreeModel_HeaderSection("Pointer", COLUMN_POINTER_WIDTH));
  setHeaderItem(4, TreeModel_HeaderSection("SelectedOwners", -1));
}

// =======================================================================
// function : createRootItem
// purpose :
// =======================================================================
TreeModel_ItemBasePtr VInspector_ViewModel::createRootItem(const int theColumnId)
{
  return VInspector_ItemContext::CreateItem(TreeModel_ItemBasePtr(), 0, theColumnId);
}

// =======================================================================
// function : GetContext
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) VInspector_ViewModel::GetContext() const
{
  return itemDynamicCast<VInspector_ItemContext>(RootItem(0))->GetContext();
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void VInspector_ViewModel::SetContext(const Handle(AIS_InteractiveContext)& theContext)
{
  // fill root item by the application
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
    itemDynamicCast<VInspector_ItemContext>(myRootItems[aColId])->SetContext(theContext);

  UpdateTreeModel();
}

// =======================================================================
// function : FindPointers
// purpose :
// =======================================================================
void VInspector_ViewModel::FindPointers(const QStringList& thePointers,
                                        const QModelIndex& theParent,
                                        QModelIndexList&   theFoundIndices)
{
  (void)thePointers;
  (void)theParent;
  (void)theFoundIndices;
  // should be used after Object of items is improved, as it takes a lot of time on BVH item
  /*
  if (thePointers.isEmpty())
    return;

  QModelIndex aParentIndex = theParent.isValid() ? theParent : index (0, 0);
  TreeModel_ItemBasePtr aParentItem = TreeModel_ModelBase::GetItemByIndex (aParentIndex); // context
  item for (int aRowId = 0, aCount = aParentItem->rowCount(); aRowId < aCount; aRowId++)
  {
    QModelIndex anIndex = index (aRowId, 0, aParentIndex);
    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    VInspector_ItemBasePtr aVItem = itemDynamicCast<VInspector_ItemBase>(anItemBase);
    if (!aVItem)
      continue;
    const Handle(Standard_Transient)& anObject = aVItem->Object();
    TCollection_AsciiString aPointerInfo = Standard_Dump::GetPointerInfo (anObject);
    if (thePointers.contains (aPointerInfo.ToCString()))
      theFoundIndices.append (anIndex);

    FindPointers (thePointers, anIndex, theFoundIndices);
  }*/
}

// =======================================================================
// function : FindIndex
// purpose :
// =======================================================================
QModelIndex VInspector_ViewModel::FindIndex(
  const Handle(AIS_InteractiveObject)& thePresentation) const
{
  QModelIndex           aParentIndex = index(0, 0);
  TreeModel_ItemBasePtr aParentItem =
    TreeModel_ModelBase::GetItemByIndex(aParentIndex); // context item
  for (int aRowId = 0, aCount = aParentItem->rowCount(); aRowId < aCount; aRowId++)
  {
    QModelIndex                         anIndex    = index(aRowId, 0, aParentIndex);
    TreeModel_ItemBasePtr               anItemBase = TreeModel_ModelBase::GetItemByIndex(anIndex);
    VInspector_ItemPresentableObjectPtr anItemPrs =
      itemDynamicCast<VInspector_ItemPresentableObject>(anItemBase);
    if (!anItemPrs)
      continue;
    if (anItemPrs->GetInteractiveObject() == thePresentation)
      return anIndex;
  }
  return QModelIndex();
}

// =======================================================================
// function : UpdateTreeModel
// purpose :
// =======================================================================
void VInspector_ViewModel::UpdateTreeModel()
{
  Reset();
  EmitLayoutChanged();
}
