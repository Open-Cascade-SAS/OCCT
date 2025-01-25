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

#include <inspector/ShapeView_VisibilityState.hxx>

#include <inspector/ShapeView_ItemShape.hxx>

// =======================================================================
// function : OnClicked
// purpose :
// =======================================================================
void ShapeView_VisibilityState::OnClicked(const QModelIndex& theIndex)
{
  processClicked(theIndex);
  emit itemClicked(theIndex);
}

// =======================================================================
// function : SetVisible
// purpose :
// =======================================================================
bool ShapeView_VisibilityState::SetVisible(const QModelIndex& theIndex,
                                           const bool         theState,
                                           const bool         toEmitDataChanged)
{
  TopoDS_Shape aShape = Shape(theIndex);
  if (aShape.IsNull())
    return false;

  myDisplayer->SetVisible(aShape, theState, myPresentationType);

  if (toEmitDataChanged)
    getModel()->EmitDataChanged(theIndex, theIndex);
  return true;
}

// =======================================================================
// function : Shape
// purpose :
// =======================================================================
TopoDS_Shape ShapeView_VisibilityState::Shape(const QModelIndex& theIndex) const
{
  TreeModel_ItemBasePtr anItemBase = TreeModel_ModelBase::GetItemByIndex(theIndex);
  if (!anItemBase)
    return TopoDS_Shape();

  ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape>(anItemBase);
  if (!aShapeItem)
    return TopoDS_Shape();

  return aShapeItem->GetItemShape();
}
