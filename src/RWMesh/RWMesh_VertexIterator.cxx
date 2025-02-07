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

#include <RWMesh_VertexIterator.hxx>

#include <BRep_Tool.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFPrs.hxx>

//=================================================================================================

RWMesh_VertexIterator::RWMesh_VertexIterator(const TDF_Label&       theLabel,
                                             const TopLoc_Location& theLocation,
                                             const Standard_Boolean theToMapColors,
                                             const XCAFPrs_Style&   theStyle)
    : RWMesh_ShapeIterator(theLabel,
                           theLocation,
                           TopAbs_VERTEX,
                           TopAbs_EDGE,
                           theToMapColors,
                           theStyle)
{
  Next();
}

//=================================================================================================

RWMesh_VertexIterator::RWMesh_VertexIterator(const TopoDS_Shape&  theShape,
                                             const XCAFPrs_Style& theStyle)
    : RWMesh_ShapeIterator(theShape, TopAbs_VERTEX, TopAbs_EDGE, theStyle)
{
  Next();
}

//=================================================================================================

void RWMesh_VertexIterator::Next()
{
  for (; myIter.More(); myIter.Next())
  {
    myVertex = TopoDS::Vertex(myIter.Current());
    myPoint  = BRep_Tool::Pnt(myVertex);
    myTrsf   = myLocation.Transformation();
    if (myVertex.IsNull())
    {
      resetVertex();
      continue;
    }

    initVertex();
    myIter.Next();
    return;
  }

  resetVertex();
}

//=================================================================================================

void RWMesh_VertexIterator::initVertex()
{
  initShape();
}
