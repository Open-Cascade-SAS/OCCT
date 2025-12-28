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

//=================================================================================================

void Prs3d::AddFreeEdges(NCollection_Sequence<gp_Pnt>&          theSegments,
                         const occ::handle<Poly_Triangulation>& thePolyTri,
                         const gp_Trsf&                         theLocation)
{
  if (thePolyTri.IsNull() || !thePolyTri->HasGeometry())
  {
    return;
  }

  // Build the connect tool.
  Poly_Connect aPolyConnect(thePolyTri);
  int          aNbTriangles = thePolyTri->NbTriangles();
  int          aT[3];
  int          aN[3];

  // Count the free edges.
  int aNbFree = 0;
  for (int anI = 1; anI <= aNbTriangles; ++anI)
  {
    aPolyConnect.Triangles(anI, aT[0], aT[1], aT[2]);
    for (int aJ = 0; aJ < 3; ++aJ)
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

  NCollection_Array1<int> aFree(1, 2 * aNbFree);

  int aFreeIndex = 1;
  for (int anI = 1; anI <= aNbTriangles; ++anI)
  {
    aPolyConnect.Triangles(anI, aT[0], aT[1], aT[2]);
    thePolyTri->Triangle(anI).Get(aN[0], aN[1], aN[2]);
    for (int aJ = 0; aJ < 3; aJ++)
    {
      int k = (aJ + 1) % 3;
      if (aT[aJ] == 0)
      {
        aFree(aFreeIndex)     = aN[aJ];
        aFree(aFreeIndex + 1) = aN[k];
        aFreeIndex += 2;
      }
    }
  }

  // free edges
  int aFreeHalfNb = aFree.Length() / 2;
  for (int anI = 1; anI <= aFreeHalfNb; ++anI)
  {
    const gp_Pnt aPoint1 = thePolyTri->Node(aFree(2 * anI - 1)).Transformed(theLocation);
    const gp_Pnt aPoint2 = thePolyTri->Node(aFree(2 * anI)).Transformed(theLocation);
    theSegments.Append(aPoint1);
    theSegments.Append(aPoint2);
  }
}

//=================================================================================================

bool Prs3d::MatchSegment(const double  X,
                         const double  Y,
                         const double  Z,
                         const double  aDistance,
                         const gp_Pnt& P1,
                         const gp_Pnt& P2,
                         double&       dist)
{
  double X1, Y1, Z1, X2, Y2, Z2;
  P1.Coord(X1, Y1, Z1);
  P2.Coord(X2, Y2, Z2);
  double DX   = X2 - X1;
  double DY   = Y2 - Y1;
  double DZ   = Z2 - Z1;
  double Dist = DX * DX + DY * DY + DZ * DZ;
  if (Dist == 0.)
    return false;

  double Lambda = ((X - X1) * DX + (Y - Y1) * DY + (Z - Z1) * DZ) / Dist;
  if (Lambda < 0. || Lambda > 1.)
    return false;
  dist = std::abs(X - X1 - Lambda * DX) + std::abs(Y - Y1 - Lambda * DY)
         + std::abs(Z - Z1 - Lambda * DZ);
  return (dist < aDistance);
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfPrimitives> Prs3d::PrimitivesFromPolylines(
  const NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& thePoints)
{
  if (thePoints.IsEmpty())
  {
    return occ::handle<Graphic3d_ArrayOfPrimitives>();
  }

  int aNbVertices = 0;
  for (NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>::Iterator anIt(thePoints);
       anIt.More();
       anIt.Next())
  {
    aNbVertices += anIt.Value()->Length();
  }
  const int                              aSegmentEdgeNb = (aNbVertices - thePoints.Size()) * 2;
  occ::handle<Graphic3d_ArrayOfSegments> aSegments =
    new Graphic3d_ArrayOfSegments(aNbVertices, aSegmentEdgeNb);
  for (NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>::Iterator anIt(thePoints);
       anIt.More();
       anIt.Next())
  {
    const occ::handle<NCollection_HSequence<gp_Pnt>>& aPoints = anIt.Value();

    int aSegmentEdge = aSegments->VertexNumber() + 1;
    aSegments->AddVertex(aPoints->First());
    for (int aPntIter = aPoints->Lower() + 1; aPntIter <= aPoints->Upper(); ++aPntIter)
    {
      aSegments->AddVertex(aPoints->Value(aPntIter));
      aSegments->AddEdge(aSegmentEdge);
      aSegments->AddEdge(++aSegmentEdge);
    }
  }

  return aSegments;
}

//=================================================================================================

void Prs3d::AddPrimitivesGroup(
  const occ::handle<Prs3d_Presentation>&                        thePrs,
  const occ::handle<Prs3d_LineAspect>&                          theAspect,
  NCollection_List<occ::handle<NCollection_HSequence<gp_Pnt>>>& thePolylines)
{
  occ::handle<Graphic3d_ArrayOfPrimitives> aPrims = Prs3d::PrimitivesFromPolylines(thePolylines);
  thePolylines.Clear();
  if (!aPrims.IsNull())
  {
    occ::handle<Graphic3d_Group> aGroup = thePrs->NewGroup();
    aGroup->SetPrimitivesAspect(theAspect->Aspect());
    aGroup->AddPrimitiveArray(aPrims);
  }
}
