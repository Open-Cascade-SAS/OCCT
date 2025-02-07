// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <RWMesh_EdgeIterator.hxx>

#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>

//=================================================================================================

RWMesh_EdgeIterator::RWMesh_EdgeIterator(const TDF_Label&       theLabel,
                                         const TopLoc_Location& theLocation,
                                         const Standard_Boolean theToMapColors,
                                         const XCAFPrs_Style&   theStyle)
    : RWMesh_ShapeIterator(theLabel,
                           theLocation,
                           TopAbs_EDGE,
                           TopAbs_FACE,
                           theToMapColors,
                           theStyle)
{
  Next();
}

//=================================================================================================

RWMesh_EdgeIterator::RWMesh_EdgeIterator(const TopoDS_Shape&  theShape,
                                         const XCAFPrs_Style& theStyle)
    : RWMesh_ShapeIterator(theShape, TopAbs_EDGE, TopAbs_FACE, theStyle)
{
  Next();
}

//=================================================================================================

void RWMesh_EdgeIterator::Next()
{
  for (; myIter.More(); myIter.Next())
  {
    myEdge      = TopoDS::Edge(myIter.Current());
    myPolygon3D = BRep_Tool::Polygon3D(myEdge, myLocation);
    myTrsf      = myLocation.Transformation();
    if (myPolygon3D.IsNull() || myPolygon3D->NbNodes() == 0)
    {
      resetEdge();
      continue;
    }

    initEdge();
    myIter.Next();
    return;
  }

  resetEdge();
}

//=================================================================================================

void RWMesh_EdgeIterator::initEdge()
{
  initShape();
}
