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

const int COLUMN_NAME_WIDTH = 190;
const int COLUMN_SIZE_WIDTH = 30;
const int COLUMN_POINTER_WIDTH = 70;
const int COLUMN_SHAPE_TYPE_WIDTH = 75;

const int COLUMN_ORIENTATION_WIDTH = 70;
const int COLUMN_LOCATION_WIDTH = 120;

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
ShapeView_TreeModel::ShapeView_TreeModel (QObject* theParent)
: TreeModel_ModelBase (theParent)
{
  SetHeaderItem (0, TreeModel_HeaderSection ("Name", COLUMN_NAME_WIDTH));
  // column 1 is reserved for visiblity state
  SetHeaderItem (2, TreeModel_HeaderSection ("Size", COLUMN_SIZE_WIDTH));
  SetHeaderItem (3, TreeModel_HeaderSection ("Pointer", COLUMN_POINTER_WIDTH));
  SetHeaderItem (4, TreeModel_HeaderSection ("Orientation", COLUMN_ORIENTATION_WIDTH));
  SetHeaderItem (5, TreeModel_HeaderSection ("Location", COLUMN_LOCATION_WIDTH));

  SetHeaderItem (6, TreeModel_HeaderSection ("Checked", -1, true));
  SetHeaderItem (7, TreeModel_HeaderSection ("Closed", -1, true));
  SetHeaderItem (8, TreeModel_HeaderSection ("Infinite", -1, true));
  SetHeaderItem (9, TreeModel_HeaderSection ("Locked", -1, true));
  SetHeaderItem (10, TreeModel_HeaderSection ("Modified", -1, true));
  SetHeaderItem (11, TreeModel_HeaderSection ("Orientable", -1, true));

  SetHeaderItem (12, TreeModel_HeaderSection ("VERTEX: (X, Y, Z)", -1, true));

  SetHeaderItem (13, TreeModel_HeaderSection ("EDGE: Length", -1, true));
  SetHeaderItem (14, TreeModel_HeaderSection ("DynamicType", -1, true));
  SetHeaderItem (15, TreeModel_HeaderSection ("First", -1, true));
  SetHeaderItem (16, TreeModel_HeaderSection ("Last", -1, true));
  SetHeaderItem (17, TreeModel_HeaderSection ("Continuity", -1, true));
  SetHeaderItem (18, TreeModel_HeaderSection ("IsClosed", -1, true));
  SetHeaderItem (19, TreeModel_HeaderSection ("IsPeriodic", -1, true));
}

// =======================================================================
// function : createRootItem
// purpose :
// =======================================================================
void ShapeView_TreeModel::createRootItem (const int theColumnId)
{
  myRootItems.insert (theColumnId, ShapeView_ItemRoot::CreateItem (TreeModel_ItemBasePtr(), 0, theColumnId));
  if (theColumnId == 0)
      m_pRootItem = myRootItems[0];
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
