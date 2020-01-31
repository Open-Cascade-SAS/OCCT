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

#include <inspector/ShapeView_TreeModel.hxx>
#include <inspector/ShapeView_ItemRoot.hxx>
#include <inspector/ShapeView_TreeModel.hxx>
#include <inspector/ShapeView_ItemRoot.hxx>
#include <inspector/ShapeView_ItemShape.hxx>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ShapeView_TreeModel::ShapeView_TreeModel (QObject* theParent)
: TreeModel_ModelBase (theParent)
{
}

// =======================================================================
// function : createRootItem
// purpose :
// =======================================================================
TreeModel_ItemBasePtr ShapeView_TreeModel::createRootItem (const int theColumnId)
{
  return ShapeView_ItemRoot::CreateItem (TreeModel_ItemBasePtr(), 0, theColumnId);
}

// =======================================================================
// function : AddShape
// purpose :
// =======================================================================
void ShapeView_TreeModel::AddShape(const TopoDS_Shape& theShape)
{
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
  {
    ShapeView_ItemRootPtr aRootItem = itemDynamicCast<ShapeView_ItemRoot>(RootItem (aColId));
    aRootItem->AddShape(theShape);
  }

  Reset();
  EmitLayoutChanged();
}

// =======================================================================
// function : RemoveAllShapes
// purpose :
// =======================================================================
void ShapeView_TreeModel::RemoveAllShapes()
{
  for (int aColId = 0, aNbColumns = columnCount(); aColId < aNbColumns; aColId++)
  {
    ShapeView_ItemRootPtr aRootItem = itemDynamicCast<ShapeView_ItemRoot>(RootItem (aColId));
    aRootItem->RemoveAllShapes();
  }
  Reset();
  EmitLayoutChanged();
}

// =======================================================================
// function : FindIndex
// purpose :
// =======================================================================
QModelIndex ShapeView_TreeModel::FindIndex (const TopoDS_Shape& theShape) const
{
  QModelIndex aParentIndex = index (0, 0);
  TreeModel_ItemBasePtr aParentItem = TreeModel_ModelBase::GetItemByIndex (aParentIndex); // application item
  for (int aChildId = 0, aCount = aParentItem->rowCount(); aChildId < aCount; aChildId++)
  {
    QModelIndex anIndex = index (aChildId, 0, aParentIndex);
    ShapeView_ItemShapePtr anItem = itemDynamicCast<ShapeView_ItemShape> (TreeModel_ModelBase::GetItemByIndex (anIndex));
    if (anItem && anItem->GetItemShape() == theShape)
      return anIndex;
  }
  return QModelIndex();
}
