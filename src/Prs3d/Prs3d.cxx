// Created on: 1993-08-27
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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

#include <Prs3d.hxx>

#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Poly_Connect.hxx>
#include <Poly_Triangulation.hxx>
#include <Prs3d_LineAspect.hxx>

// =========================================================================
// function : AddFreeEdges
// purpose  :
// =========================================================================
void Prs3d::AddFreeEdges(TColgp_SequenceOfPnt&             theSegments,
                         const Handle(Poly_Triangulation)& thePolyTri,
                         const gp_Trsf&                    theLocation)
{
  if (thePolyTri.IsNull() || !thePolyTri->HasGeometry())
  {
    return;
  }

  // Build the connect tool.
  Poly_Connect     aPolyConnect(thePolyTri);
  Standard_Integer aNbTriangles = thePolyTri->NbTriangles();
  Standard_Integer aT[3];
  Standard_Integer aN[3];

  // Count the free edges.
  Standard_Integer aNbFree = 0;
  for (Standard_Integer anI = 1; anI <= aNbTriangles; ++anI)
  {
    aPolyConnect.Triangles(anI, aT[0], aT[1], aT[2]);
    for (Standard_Integer aJ = 0; aJ < 3; ++aJ)
    {
      if (aT[aJ] == 0)
      {
        ++aNbFree;
      }
    }
  }
  if (aNbFree == 0)
  {
    return;
  }

  TColStd_Array1OfInteger aFree(1, 2 * aNbFree);

  Standard_Integer aFreeIndex = 1;
  for (Standard_Integer anI = 1; anI <= aNbTriangles; ++anI)
  {
    aPolyConnect.Triangles(anI, aT[0], aT[1], aT[2]);
    thePolyTri->Triangle(anI).Get(aN[0], aN[1], aN[2]);
    for (Standard_Integer aJ = 0; aJ < 3; aJ++)
    {
      Standard_Integer k = (aJ + 1) % 3;
      if (aT[aJ] == 0)
      {
        aFree(aFreeIndex)     = aN[aJ];
        aFree(aFreeIndex + 1) = aN[k];
        aFreeIndex += 2;
      }
    }
  }

  // free edges
  Standard_Integer aFreeHalfNb = aFree.Length() / 2;
  for (Standard_Integer anI = 1; anI <= aFreeHalfNb; ++anI)
  {
    const gp_Pnt aPoint1 = thePolyTri->Node(aFree(2 * anI - 1)).Transformed(theLocation);
    const gp_Pnt aPoint2 = thePolyTri->Node(aFree(2 * anI)).Transformed(theLocation);
    theSegments.Append(aPoint1);
    theSegments.Append(aPoint2);
  }
}

//=================================================================================================

Standard_Boolean Prs3d::MatchSegment(const Standard_Real X,
                                     const Standard_Real Y,
                                     const Standard_Real Z,
                                     const Standard_Real aDistance,
                                     const gp_Pnt&       P1,
                                     const gp_Pnt&       P2,
                                     Standard_Real&      dist)
{
  Standard_Real X1, Y1, Z1, X2, Y2, Z2;
  P1.Coord(X1, Y1, Z1);
  P2.Coord(X2, Y2, Z2);
  Standard_Real DX   = X2 - X1;
  Standard_Real DY   = Y2 - Y1;
  Standard_Real DZ   = Z2 - Z1;
  Standard_Real Dist = DX * DX + DY * DY + DZ * DZ;
  if (Dist == 0.)
    return Standard_False;

  Standard_Real Lambda = ((X - X1) * DX + (Y - Y1) * DY + (Z - Z1) * DZ) / Dist;
  if (Lambda < 0. || Lambda > 1.)
    return Standard_False;
  dist = Abs(X - X1 - Lambda * DX) + Abs(Y - Y1 - Lambda * DY) + Abs(Z - Z1 - Lambda * DZ);
  return (dist < aDistance);
}

//=================================================================================================

Handle(Graphic3d_ArrayOfPrimitives) Prs3d::PrimitivesFromPolylines(
  const Prs3d_NListOfSequenceOfPnt& thePoints)
{
  if (thePoints.IsEmpty())
  {
    return Handle(Graphic3d_ArrayOfPrimitives)();
  }

  Standard_Integer aNbVertices = 0;
  for (Prs3d_NListOfSequenceOfPnt::Iterator anIt(thePoints); anIt.More(); anIt.Next())
  {
    aNbVertices += anIt.Value()->Length();
  }
  const Standard_Integer            aSegmentEdgeNb = (aNbVertices - thePoints.Size()) * 2;
  Handle(Graphic3d_ArrayOfSegments) aSegments =
    new Graphic3d_ArrayOfSegments(aNbVertices, aSegmentEdgeNb);
  for (Prs3d_NListOfSequenceOfPnt::Iterator anIt(thePoints); anIt.More(); anIt.Next())
  {
    const Handle(TColgp_HSequenceOfPnt)& aPoints = anIt.Value();

    Standard_Integer aSegmentEdge = aSegments->VertexNumber() + 1;
    aSegments->AddVertex(aPoints->First());
    for (Standard_Integer aPntIter = aPoints->Lower() + 1; aPntIter <= aPoints->Upper(); ++aPntIter)
    {
      aSegments->AddVertex(aPoints->Value(aPntIter));
      aSegments->AddEdge(aSegmentEdge);
      aSegments->AddEdge(++aSegmentEdge);
    }
  }

  return aSegments;
}

//=================================================================================================

void Prs3d::AddPrimitivesGroup(const Handle(Prs3d_Presentation)& thePrs,
                               const Handle(Prs3d_LineAspect)&   theAspect,
                               Prs3d_NListOfSequenceOfPnt&       thePolylines)
{
  Handle(Graphic3d_ArrayOfPrimitives) aPrims = Prs3d::PrimitivesFromPolylines(thePolylines);
  thePolylines.Clear();
  if (!aPrims.IsNull())
  {
    Handle(Graphic3d_Group) aGroup = thePrs->NewGroup();
    aGroup->SetPrimitivesAspect(theAspect->Aspect());
    aGroup->AddPrimitiveArray(aPrims);
  }
}
