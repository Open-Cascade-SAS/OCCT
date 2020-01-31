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

#include <inspector/ShapeView_ItemShape.hxx>

#include <inspector/ShapeView_ItemRoot.hxx>
#include <inspector/ShapeView_ItemShape.hxx>

#include <inspector/ViewControl_Tools.hxx>

#include <TopAbs.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Iterator.hxx>

#include <TopExp.hxx>
#include <TopTools_IndexedMapOfShape.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QStringList>
#include <Standard_WarningsRestore.hxx>

// =======================================================================
// function : Shape
// purpose :
// =======================================================================
TopoDS_Shape ShapeView_ItemShape::Shape (const int theRowId) const
{
  if (myChildShapes.IsEmpty())
  {
    ShapeView_ItemShape* aThis = (ShapeView_ItemShape*)this;

    if (myExplodeType != TopAbs_SHAPE)
    {
      TopExp::MapShapes(myShape, myExplodeType, aThis->myChildShapes);
    }
    else
    {
      TopoDS_Iterator aSubShapeIt (myShape);
      for (int aCurrentIndex = 0; aSubShapeIt.More(); aSubShapeIt.Next(), aCurrentIndex++)
      {
        aThis->myChildShapes.Add (aSubShapeIt.Value());
      }
    }
  }
  if (myChildShapes.Extent() >= theRowId + 1)
    return myChildShapes(theRowId + 1);

  return TopoDS_Shape();
}

// =======================================================================
// function : initValue
// purpose :
// =======================================================================
QVariant ShapeView_ItemShape::initValue(const int theRole) const
{
  QVariant aParentValue = TreeModel_ItemBase::initValue (theRole);
  if (aParentValue.isValid())
    return aParentValue;

  TopoDS_Shape aShape = getShape();
  if (aShape.IsNull())
    return QVariant();

  if (theRole != Qt::DisplayRole && theRole != Qt::ToolTipRole)
    return QVariant();

  switch (Column())
  {
    case 0: return TopAbs::ShapeTypeToString (aShape.ShapeType());
    default: break;
  }
  return QVariant();
}

// =======================================================================
// function : initRowCount
// purpose :
// =======================================================================
int ShapeView_ItemShape::initRowCount() const
{
  TopoDS_Shape aShape = getShape();
  if (aShape.IsNull())
    return 0;

  int aRowsCount = 0;
  if (myExplodeType != TopAbs_SHAPE)
  {
    TopTools_IndexedMapOfShape aSubShapes;
    TopExp::MapShapes(aShape, myExplodeType, aSubShapes);
    aRowsCount = aSubShapes.Extent();
  }
  else
  {
    for (TopoDS_Iterator aSubShapeIt(aShape); aSubShapeIt.More(); aSubShapeIt.Next())
      aRowsCount++;
  }
  return aRowsCount;
}

// =======================================================================
// function : initStream
// purpose :
// =======================================================================
void ShapeView_ItemShape::initStream (Standard_OStream& theOStream) const
{
  TopoDS_Shape aShape = getShape();
  if (aShape.IsNull())
    return;

  aShape.DumpJson (theOStream);
}

// =======================================================================
// function : createChild
// purpose :
// =======================================================================
TreeModel_ItemBasePtr ShapeView_ItemShape::createChild (int theRow, int theColumn)
{
  return ShapeView_ItemShape::CreateItem (currentItem(), theRow, theColumn);
}

// =======================================================================
// function : Init
// purpose :
// =======================================================================
void ShapeView_ItemShape::Init()
{
  ShapeView_ItemRootPtr aRootItem = itemDynamicCast<ShapeView_ItemRoot> (Parent());
  ShapeView_ItemShapePtr aShapeItem = itemDynamicCast<ShapeView_ItemShape> (Parent());
  myShape = aRootItem ? aRootItem->Shape (Row()) : aShapeItem->Shape (Row());

  TreeModel_ItemBase::Init();
}

// =======================================================================
// function : getShape
// purpose :
// =======================================================================
TopoDS_Shape ShapeView_ItemShape::getShape() const
{
  initItem();
  return myShape;
}

// =======================================================================
// function : Reset
// purpose :
// =======================================================================
void ShapeView_ItemShape::Reset()
{
  myFileName = QString();
  myChildShapes.Clear();
  myShape = TopoDS_Shape();

  TreeModel_ItemBase::Reset();
}

// =======================================================================
// function : initItem
// purpose :
// =======================================================================
void ShapeView_ItemShape::initItem() const
{
  if (IsInitialized())
    return;
  const_cast<ShapeView_ItemShape*>(this)->Init();
}
