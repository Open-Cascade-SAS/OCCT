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
  for (int aColumnId = 0, aNbColumns = columnCount(); aColumnId < aNbColumns; aColumnId++)
    myRootItems.insert(aColumnId, ShapeView_ItemRoot::CreateItem(TreeModel_ItemBasePtr(), 0, aColumnId));

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
// function : headerData
// purpose :
// =======================================================================
QVariant ShapeView_TreeModel::headerData (int theSection, Qt::Orientation theOrientation, int theRole) const
{
  if (theOrientation != Qt::Horizontal || theRole != Qt::DisplayRole)
    return QVariant();
  {
    switch (theSection)
    {
      case 0: return "Name";
      case 1: return "Size";
      case 2: return "Pointer";
      case 3: return "Orientation";
      case 4: return "Location";
      case 5: return "Flags"; // Checked/Closed/Infinite/Locked/Modified/Orientable
      case 7: return "Other";
        //Auto Triangulation
      default: break;
    }
  }
  return QVariant();
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

// =======================================================================
// function : SingleSelected
// purpose :
// =======================================================================
QModelIndex ShapeView_TreeModel::SingleSelected (const QModelIndexList& theIndices, const int theCellId,
                                                 const Qt::Orientation theOrientation)
{
  QModelIndexList aFirstColumnSelectedIndices;
  for (QModelIndexList::const_iterator anIndicesIt = theIndices.begin(); anIndicesIt != theIndices.end(); anIndicesIt++)
  {
    QModelIndex anIndex = *anIndicesIt;
    if ((theOrientation == Qt::Horizontal && anIndex.column() == theCellId) ||
        (theOrientation == Qt::Vertical && anIndex.row() == theCellId))
      aFirstColumnSelectedIndices.append (anIndex);
  }
  return aFirstColumnSelectedIndices.size() == 1 ? aFirstColumnSelectedIndices.first() : QModelIndex();
}
