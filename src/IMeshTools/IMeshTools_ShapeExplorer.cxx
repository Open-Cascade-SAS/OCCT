// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <IMeshTools_ShapeExplorer.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepLib.hxx>
#include <BRep_Tool.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Geom_Surface.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IMeshTools_ShapeExplorer, IMeshData_Shape)

namespace
{
  //=======================================================================
  // Function: visitEdges
  // Purpose : Explodes the given shape on edges according to the specified
  //           criteria and visits each one in order to add it to data model.
  //=======================================================================
  void visitEdges (const Handle (IMeshTools_ShapeVisitor)& theVisitor,
                   const TopoDS_Shape&                     theShape,
                   const Standard_Boolean                  isResetLocation,
                   const TopAbs_ShapeEnum                  theToFind,
                   const TopAbs_ShapeEnum                  theToAvoid = TopAbs_SHAPE)
  {
    TopExp_Explorer aEdgesIt (theShape, theToFind, theToAvoid);
    for (; aEdgesIt.More (); aEdgesIt.Next ())
    {
      const TopoDS_Edge& aEdge = TopoDS::Edge (aEdgesIt.Current ());
      if (!BRep_Tool::IsGeometric (aEdge))
      {
        continue;
      }

      theVisitor->Visit (isResetLocation ?
        TopoDS::Edge (aEdge.Located (TopLoc_Location ())) :
        aEdge);
    }
  }
}

//=======================================================================
// Function: Constructor
// Purpose : 
//=======================================================================
IMeshTools_ShapeExplorer::IMeshTools_ShapeExplorer (
  const TopoDS_Shape& theShape)
  : IMeshData_Shape (theShape)
{
}

//=======================================================================
// Function: Destructor
// Purpose : 
//=======================================================================
IMeshTools_ShapeExplorer::~IMeshTools_ShapeExplorer ()
{
}

//=======================================================================
// Function: Accept
// Purpose : 
//=======================================================================
void IMeshTools_ShapeExplorer::Accept (
  const Handle (IMeshTools_ShapeVisitor)& theVisitor)
{
  // Explore all free edges in shape.
  visitEdges (theVisitor, GetShape (), Standard_True, TopAbs_EDGE, TopAbs_FACE);

  // Explore all related to some face edges in shape.
  // make array of faces suitable for processing (excluding faces without surface)
  TopTools_ListOfShape aFaceList;
  BRepLib::ReverseSortFaces (GetShape (), aFaceList);
  TopTools_MapOfShape aFaceMap;

  const TopLoc_Location aEmptyLoc;
  TopTools_ListIteratorOfListOfShape aFaceIter (aFaceList);
  for (; aFaceIter.More (); aFaceIter.Next ())
  {
    TopoDS_Shape aFaceNoLoc = aFaceIter.Value ();
    aFaceNoLoc.Location (aEmptyLoc);
    if (!aFaceMap.Add(aFaceNoLoc))
    {
      continue; // already processed
    }

    const TopoDS_Face& aFace = TopoDS::Face (aFaceIter.Value ());
    if (!BRep_Tool::IsGeometric (aFace))
    {
      continue;
    }

    // Explore all edges in face.
    visitEdges (theVisitor, aFace, Standard_False, TopAbs_EDGE);

    // Store only forward faces in order to prevent inverse issue.
    theVisitor->Visit (TopoDS::Face (aFace.Oriented (TopAbs_FORWARD)));
  }
}
