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
#include <inspector/VInspector_ItemEntityOwner.hxx>
#include <inspector/VInspector_ItemPresentableObject.hxx>
#include <inspector/VInspector_ItemSensitiveEntity.hxx>
#include <SelectBasics_EntityOwner.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QItemSelectionModel>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

const int COLUMN_NAME_WIDTH = 260;
const int COLUMN_SIZE_WIDTH = 30;
const int COLUMN_POINTER_WIDTH = 70;
const int COLUMN_SHAPE_TYPE_WIDTH = 75;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
VInspector_ViewModel::VInspector_ViewModel (QObject* theParent)
  : TreeModel_ModelBase (theParent)
{
  SetHeaderItem (0, TreeModel_HeaderSection ("Name", COLUMN_NAME_WIDTH));
  SetHeaderItem (1, TreeModel_HeaderSection ("Size", COLUMN_SIZE_WIDTH));
  SetHeaderItem (2, TreeModel_HeaderSection ("Pointer", COLUMN_POINTER_WIDTH));
  SetHeaderItem (3, TreeModel_HeaderSection ("ShapeType", COLUMN_SHAPE_TYPE_WIDTH)); // ItemPresentableObject, ItemSelection
  SetHeaderItem (4, TreeModel_HeaderSection ("SelectedOwners", -1)); // ItemContext, ItemPresentableObject, ItemSelection
  SetHeaderItem (5, TreeModel_HeaderSection ("ActivatedModes", -1)); // ItemPresentableObject
  SetHeaderItem (6, TreeModel_HeaderSection ("DeviationCoefficient", -1, true)); // ItemContext, ItemPresentableObject
  SetHeaderItem (7, TreeModel_HeaderSection ("Deflection", -1, true)); // ItemPresentableObject
  SetHeaderItem (8, TreeModel_HeaderSection ("IsAutoTriangulation", -1, true)); // ItemPresentableObject

  SetHeaderItem (9, TreeModel_HeaderSection ("SelectionState", -1)); // ItemSelection
  SetHeaderItem (10, TreeModel_HeaderSection ("Sensitivity", -1, true)); // ItemSelection
  SetHeaderItem (11, TreeModel_HeaderSection ("UpdateStatus", -1, true)); // ItemSelection
  SetHeaderItem (12, TreeModel_HeaderSection ("BVHUpdateStatus", -1, true)); // ItemSelection

  SetHeaderItem (13, TreeModel_HeaderSection ("IsActiveForSelection", -1, true)); // ItemSensitiveEntity
  SetHeaderItem (14, TreeModel_HeaderSection ("SensitivityFactor", -1, true)); // ItemSensitiveEntity
  SetHeaderItem (15, TreeModel_HeaderSection ("NbSubElements", -1, true)); // ItemSensitiveEntity
  SetHeaderItem (16, TreeModel_HeaderSection ("Priority", -1, true)); // ItemSensitiveEntity

  SetHeaderItem (17, TreeModel_HeaderSection ("TShape", COLUMN_POINTER_WIDTH, true)); // ItemEntityOwner
  SetHeaderItem (18, TreeModel_HeaderSection ("Orientation", -1, true)); // ItemEntityOwner
  SetHeaderItem (19, TreeModel_HeaderSection ("Location", -1, true)); // ItemEntityOwner

  SetHeaderItem (20, TreeModel_HeaderSection ("Color", -1)); // ItemPresentableObject
}

// =======================================================================
// function : createRootItem
// purpose :
// =======================================================================
void VInspector_ViewModel::createRootItem (const int theColumnId)
{
  myRootItems.insert (theColumnId, VInspector_ItemContext::CreateItem (TreeModel_ItemBasePtr(), 0, theColumnId));
  if (theColumnId == 0)
      m_pRootItem = myRootItems[0];
}

// =======================================================================
// function : GetContext
// purpose :
// =======================================================================
Handle(AIS_InteractiveContext) VInspector_ViewModel::GetContext() const
{
  return itemDynamicCast<VInspector_ItemContext> (RootItem (0))->GetContext();
}

// =======================================================================
// function : SetContext
// purpose :
// =======================================================================
void VInspector_ViewModel::SetContext (const Handle(AIS_InteractiveContext)& theContext)
{
  // fill root item by the application
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
    itemDynamicCast<VInspector_ItemContext>(myRootItems[aColId])->SetContext (theContext);
  EmitLayoutChanged();
}

// =======================================================================
// function : FindPointers
// purpose :
// =======================================================================
QModelIndexList VInspector_ViewModel::FindPointers (const QStringList& thePointers)
{
  QModelIndexList anIndices;
  QModelIndex aParentIndex = index (0, 0);
  TreeModel_ItemBasePtr aParentItem = TreeModel_ModelBase::GetItemByIndex (aParentIndex); // context item
  for (int aRowId = 0, aCount = aParentItem->rowCount(); aRowId < aCount; aRowId++)
  {
    QModelIndex anIndex = index (aRowId, 0, aParentIndex);
    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    VInspector_ItemPresentableObjectPtr anItemPrs = itemDynamicCast<VInspector_ItemPresentableObject>(anItemBase);
    if (!anItemPrs)
      continue;
    if (thePointers.contains (anItemPrs->PointerInfo()))
      anIndices.append (anIndex);
  }
  return anIndices;
}

// =======================================================================
// function : FindIndex
// purpose :
// =======================================================================
QModelIndex VInspector_ViewModel::FindIndex (const Handle(AIS_InteractiveObject)& thePresentation) const
{
  QModelIndex aParentIndex = index (0, 0);
  TreeModel_ItemBasePtr aParentItem = TreeModel_ModelBase::GetItemByIndex (aParentIndex); // context item
  for (int aRowId = 0, aCount = aParentItem->rowCount(); aRowId < aCount; aRowId++)
  {
    QModelIndex anIndex = index (aRowId, 0, aParentIndex);
    TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex (anIndex);
    VInspector_ItemPresentableObjectPtr anItemPrs = itemDynamicCast<VInspector_ItemPresentableObject>(anItemBase);
    if (!anItemPrs)
      continue;
    if (anItemPrs->GetInteractiveObject() == thePresentation)
      return anIndex;
  }
  return QModelIndex();
}

// =======================================================================
// function : GetSelectedOwners
// purpose :
// =======================================================================
void VInspector_ViewModel::GetSelectedOwners (QItemSelectionModel* theSelectionModel,
                                              NCollection_List<Handle(SelectBasics_EntityOwner)>& theOwners)
{
  if (!theSelectionModel)
    return;
  QList<TreeModel_ItemBasePtr> anItems;
  
  QModelIndexList anIndices = theSelectionModel->selectedIndexes();
  for (QModelIndexList::const_iterator anIndicesIt = anIndices.begin(); anIndicesIt != anIndices.end(); anIndicesIt++)
  {
    TreeModel_ItemBasePtr anItem = TreeModel_ModelBase::GetItemByIndex (*anIndicesIt);
    if (!anItem || anItems.contains (anItem))
      continue;
    anItems.append (anItem);
  }

  QList<size_t> aSelectedIds; // Remember of selected address in order to avoid duplicates
  for (QList<TreeModel_ItemBasePtr>::const_iterator anItemIt = anItems.begin(); anItemIt != anItems.end(); anItemIt++)
  {
    TreeModel_ItemBasePtr anItem = *anItemIt;
    Handle(SelectBasics_EntityOwner) anEntityOwner;
    if (VInspector_ItemEntityOwnerPtr anOwnerItem = itemDynamicCast<VInspector_ItemEntityOwner>(anItem))
    {
      anEntityOwner = anOwnerItem->EntityOwner();
    }
    else if (VInspector_ItemSensitiveEntityPtr aSensItem = itemDynamicCast<VInspector_ItemSensitiveEntity>(anItem))
    {
      anEntityOwner = aSensItem->GetSensitiveEntity()->BaseSensitive()->OwnerId();
    }
    if (anEntityOwner.IsNull())
      continue;
    if (aSelectedIds.contains ((size_t)anEntityOwner.operator->()))
      continue;
    aSelectedIds.append ((size_t)anEntityOwner.operator->());
    if (!anEntityOwner.IsNull())
      theOwners.Append (anEntityOwner);
  }
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
