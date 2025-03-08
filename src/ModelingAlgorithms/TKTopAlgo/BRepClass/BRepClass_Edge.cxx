// Created on: 1992-11-19
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#include <BRepClass_Edge.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Precision.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopExp.hxx>

//=================================================================================================

BRepClass_Edge::BRepClass_Edge()
    : myMaxTolerance(Precision::Infinite()),
      myUseBndBox(Standard_False)
{
}

//=================================================================================================

void BRepClass_Edge::SetNextEdge(const TopTools_IndexedDataMapOfShapeListOfShape& theMapVE)
{
  if (theMapVE.IsEmpty() || myEdge.IsNull())
  {
    return;
  }
  TopoDS_Vertex aVF, aVL;
  TopExp::Vertices(myEdge, aVF, aVL, Standard_True);

  if (aVL.IsNull() || aVL.IsSame(aVF))
  {
    return;
  }
  const TopTools_ListOfShape* aListE = theMapVE.Seek(aVL);
  if (aListE->Extent() == 2)
  {
    for (TopTools_ListIteratorOfListOfShape anIt(*aListE); anIt.More(); anIt.Next())
    {
      if ((!anIt.Value().IsNull()) && (!anIt.Value().IsSame(myEdge)))
      {
        myNextEdge = TopoDS::Edge(anIt.Value());
      }
    }
  }
}

//=================================================================================================

BRepClass_Edge::BRepClass_Edge(const TopoDS_Edge& E, const TopoDS_Face& F)
    : myEdge(E),
      myFace(F),
      myMaxTolerance(Precision::Infinite()),
      myUseBndBox(Standard_False)
{
}
