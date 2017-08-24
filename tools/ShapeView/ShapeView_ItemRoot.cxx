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


#include <inspector/ShapeView_ItemRoot.hxx>
#include <inspector/ShapeView_ItemShape.hxx>

// =======================================================================
// function : GetShape
// purpose :
// =======================================================================
const TopoDS_Shape& ShapeView_ItemRoot::GetShape (const int theRowId)
{
  NCollection_List<TopoDS_Shape>::Iterator aShapesIt (myShapes);
  for (int aRowId = 0; aShapesIt.More(); aShapesIt.Next(), aRowId++)
  {
    if (aRowId == theRowId)
      break;
  }
  return aShapesIt.Value();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant ShapeView_ItemRoot::initValue(const int theRole) const
{
  if (Column() != 0)
    return QVariant();

  if (theRole == Qt::DisplayRole || theRole == Qt::EditRole)
    return "TopoDS_Shapes";

  return QVariant();
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr ShapeView_ItemRoot::createChild (int theRow, int theColumn)
{
  return ShapeView_ItemShape::CreateItem (currentItem(), theRow, theColumn);
}

