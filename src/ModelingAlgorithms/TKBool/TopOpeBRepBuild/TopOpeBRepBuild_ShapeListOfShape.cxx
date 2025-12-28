// Created on: 1997-06-26
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#include <TopOpeBRepBuild_ShapeListOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopAbs_State.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_List.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>
#include <TCollection_AsciiString.hxx>

//=================================================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape() {}

//=================================================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape(const TopoDS_Shape& S)
{
  myShape = S;
}

//=================================================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape(
  const TopoDS_Shape&                   S,
  const NCollection_List<TopoDS_Shape>& L)
{
  myShape = S;
  myList  = L;
}

//=================================================================================================

const NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_ShapeListOfShape::List() const
{
  return myList;
}

//=================================================================================================

NCollection_List<TopoDS_Shape>& TopOpeBRepBuild_ShapeListOfShape::ChangeList()
{
  return myList;
}

//=================================================================================================

const TopoDS_Shape& TopOpeBRepBuild_ShapeListOfShape::Shape() const
{
  return myShape;
}

//=================================================================================================

TopoDS_Shape& TopOpeBRepBuild_ShapeListOfShape::ChangeShape()
{
  return myShape;
}
