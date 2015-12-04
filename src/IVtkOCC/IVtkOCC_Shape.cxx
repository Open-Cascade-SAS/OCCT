// Created on: 2011-10-14 
// Created by: Roman KOZLOV
// Copyright (c) 2011-2014 OPEN CASCADE SAS 
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

#include <IVtkOCC_Shape.hxx>
#include <TopExp.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IVtkOCC_Shape,IVtk_IShape)

// Handle implementation


//============================================================================
// Method: Constructor
// Purpose:
//============================================================================
IVtkOCC_Shape::IVtkOCC_Shape (const TopoDS_Shape& theShape)
: myTopoDSShape (theShape)
{
  buildSubShapeIdMap();
}

//============================================================================
// Method: Destructor
// Purpose:
//============================================================================
IVtkOCC_Shape::~IVtkOCC_Shape() { }

//============================================================================
// Method: getSubShapeId
// Purpose: Returns unique ID of the given sub-shape within the top-level shape.
//============================================================================
IVtk_IdType IVtkOCC_Shape::GetSubShapeId (const TopoDS_Shape& theSubShape) const
{
  Standard_Integer anIndex = theSubShape.IsSame (myTopoDSShape) ?
                             -1 :
                             mySubShapeIds.FindIndex (theSubShape);

  if (!anIndex) // Not found in the map
  {
    anIndex = -1;
  }

  return (IVtk_IdType)anIndex;
}

//============================================================================
// Method: getSubIds
// Purpose: Get ids of sub-shapes composing a sub-shape with the given id.
//============================================================================
IVtk_ShapeIdList IVtkOCC_Shape::GetSubIds (const IVtk_IdType theId) const
{
  IVtk_ShapeIdList aRes;
  // Get the sub-shape by the given id.
  TopoDS_Shape aShape = mySubShapeIds.FindKey ((Standard_Integer) theId);
  TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
  if (aShapeType == TopAbs_VERTEX || aShapeType == TopAbs_EDGE ||
      aShapeType == TopAbs_FACE)
  {
    // If it is vertex, edge or face return just the input id.
    aRes.Append (theId);
  }
  else
  {
    // Find all composing vertices, edges and faces of the the found sub-shape
    // and append their ids to the result.
    TopTools_IndexedMapOfShape aSubShapes;
    if (aShape.IsSame (myTopoDSShape))
    {
      aSubShapes = mySubShapeIds;
    }
    else
    {
      TopExp::MapShapes (aShape, aSubShapes);
    }

    for (int anIt = 1; anIt <= aSubShapes.Extent(); anIt++)
    {
      aShape = aSubShapes.FindKey (anIt);
      aShapeType = aShape.ShapeType();
      if (aShapeType == TopAbs_VERTEX || aShapeType == TopAbs_EDGE ||
          aShapeType == TopAbs_FACE)
      {
        // If it is vertex, edge or face add its id to the result.
        aRes.Append (mySubShapeIds.FindIndex (aShape));
      }
    }
  }

  return aRes;
}

//============================================================================
// Method: GetSubShape
// Purpose:
//============================================================================
const TopoDS_Shape& IVtkOCC_Shape::GetSubShape (const IVtk_IdType theId) const
{
  return mySubShapeIds.FindKey ((Standard_Integer) theId);
}

//============================================================================
//  Method: buildShapeIdMap
// Purpose: Private method, assigns IDs to all sub-shapes of the top-level shape.
//============================================================================
void IVtkOCC_Shape::buildSubShapeIdMap()
{
  mySubShapeIds.Clear();
  TopExp::MapShapes (myTopoDSShape, mySubShapeIds);
}
