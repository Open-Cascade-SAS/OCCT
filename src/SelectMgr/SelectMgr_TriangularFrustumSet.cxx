// Created on: 2014-11-21
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_Delaun.hxx>
#include <NCollection_IncAllocator.hxx>

#include <SelectMgr_TriangularFrustumSet.hxx>
#include <SelectMgr_TriangularFrustum.hxx>

#define MEMORY_BLOCK_SIZE 512 * 7

// =======================================================================
// function : BuildSelectingVolume
// purpose  : Meshes polygon bounded by polyline. Than organizes a set of
//            triangular frustums, where each triangle's projection onto
//            near and far view frustum planes is considered as a frustum
//            base
// =======================================================================
void SelectMgr_TriangularFrustumSet::Build (const TColgp_Array1OfPnt2d& thePoints)
{
  myFrustums.Clear();

  Handle(NCollection_IncAllocator) anAllocator = new NCollection_IncAllocator (MEMORY_BLOCK_SIZE);
  Handle(BRepMesh_DataStructureOfDelaun) aMeshStructure = new BRepMesh_DataStructureOfDelaun(anAllocator);
  Standard_Integer aPtsLower = thePoints.Lower();
  Standard_Integer aPtsUpper = thePoints.Upper();
  BRepMesh::Array1OfInteger anIndexes (0, thePoints.Length() - 1);
  for (Standard_Integer aPtIdx = aPtsLower; aPtIdx <= aPtsUpper; ++aPtIdx)
  {
    BRepMesh_Vertex aVertex (thePoints.Value (aPtIdx).XY(), aPtIdx, BRepMesh_Frontier);
    anIndexes.ChangeValue (aPtIdx - aPtsLower) = aMeshStructure->AddNode (aVertex);
  }

  Standard_Real aPtSum = 0;
  for (Standard_Integer aIdx = aPtsLower; aIdx <= aPtsUpper; ++aIdx)
  {
    Standard_Integer aNextIdx = (aIdx % thePoints.Length()) + 1;
    aPtSum += (thePoints.Value (aNextIdx).Coord().X() - thePoints.Value (aIdx).Coord().X())
              * (thePoints.Value (aNextIdx).Coord().Y() + thePoints.Value (aIdx).Coord().Y());
  }
  Standard_Boolean isClockwiseOrdered = aPtSum < 0;

  for (Standard_Integer aIdx = 0; aIdx < anIndexes.Length(); ++aIdx)
  {
    Standard_Integer aPtIdx = isClockwiseOrdered ? aIdx : (aIdx + 1) % anIndexes.Length();
    Standard_Integer aNextPtIdx = isClockwiseOrdered ? (aIdx + 1) % anIndexes.Length() : aIdx;
    BRepMesh_Edge anEdge (anIndexes.Value (aPtIdx),
                          anIndexes.Value (aNextPtIdx),
                          BRepMesh_Frontier);
    aMeshStructure->AddLink (anEdge);
  }

  BRepMesh_Delaun aTriangulation (aMeshStructure, anIndexes);
  const BRepMesh::MapOfInteger& aTriangles = aMeshStructure->ElementsOfDomain();
  if (aTriangles.Extent() < 1)
    return;

  BRepMesh::MapOfInteger::Iterator aTriangleIt (aTriangles);
  for (; aTriangleIt.More(); aTriangleIt.Next())
  {
    const Standard_Integer aTriangleId = aTriangleIt.Key();
    const BRepMesh_Triangle& aCurrentTriangle = aMeshStructure->GetElement (aTriangleId);

    if (aCurrentTriangle.Movability() == BRepMesh_Deleted)
      continue;

    Standard_Integer aTriangleVerts[3];
    aMeshStructure->ElementNodes (aCurrentTriangle, aTriangleVerts);

    gp_Pnt2d aPts[3];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 3; ++aVertIdx)
    {
      const BRepMesh_Vertex& aVertex = aMeshStructure->GetNode (aTriangleVerts[aVertIdx]);
      aPts[aVertIdx] = aVertex.Coord();
    }

    Handle(SelectMgr_TriangularFrustum) aTrFrustum = new SelectMgr_TriangularFrustum();
    aTrFrustum->SetBuilder (myBuilder);
    aTrFrustum->Build (aPts[0], aPts[1], aPts[2]);
    myFrustums.Append (aTrFrustum);
  }

  aMeshStructure.Nullify();
  anAllocator.Nullify();
}

// =======================================================================
// function : ScaleAndTransform
// purpose  : IMPORTANT: Scaling makes sense only for frustum built on a single point!
//            Note that this method does not perform any checks on type of the frustum.
//            Returns a copy of the frustum resized according to the scale factor given
//            and transforms it using the matrix given.
//            There are no default parameters, but in case if:
//                - transformation only is needed: @theScaleFactor must be initialized
//                  as any negative value;
//                - scale only is needed: @theTrsf must be set to gp_Identity.
// =======================================================================
Handle(SelectMgr_BaseFrustum) SelectMgr_TriangularFrustumSet::ScaleAndTransform (const Standard_Integer theScale,
                                                                                 const gp_GTrsf& theTrsf) const
{
  Handle(SelectMgr_TriangularFrustumSet) aRes = new SelectMgr_TriangularFrustumSet();

  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    aRes->myFrustums.Append (Handle(SelectMgr_TriangularFrustum)::DownCast (anIter.Value()->ScaleAndTransform (theScale, theTrsf)));
  }

  return aRes;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const SelectMgr_Vec3& theMinPnt,
                                                           const SelectMgr_Vec3& theMaxPnt,
                                                           Standard_Real& theDepth)
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (theMinPnt, theMaxPnt, theDepth))
      return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const SelectMgr_Vec3& theMinPnt,
                                                           const SelectMgr_Vec3& theMaxPnt,
                                                           Standard_Boolean* /*theInside*/)
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (theMinPnt, theMaxPnt, NULL))
      return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const gp_Pnt& thePnt,
                                                           Standard_Real& theDepth)
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (thePnt, theDepth))
      return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const TColgp_Array1OfPnt& theArrayOfPts,
                                                           Select3D_TypeOfSensitivity theSensType,
                                                           Standard_Real& theDepth)
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (theArrayOfPts, theSensType, theDepth))
      return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const gp_Pnt& thePnt1,
                                                           const gp_Pnt& thePnt2,
                                                           Standard_Real& theDepth)
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (thePnt1, thePnt2, theDepth))
      return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const gp_Pnt& thePnt1,
                                                           const gp_Pnt& thePnt2,
                                                           const gp_Pnt& thePnt3,
                                                           Select3D_TypeOfSensitivity theSensType,
                                                           Standard_Real& theDepth)
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (thePnt1, thePnt2, thePnt3, theSensType, theDepth))
      return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : GetPlanes
// purpose  :
// =======================================================================
void SelectMgr_TriangularFrustumSet::GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const
{
  thePlaneEquations.Clear();

  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    anIter.Value()->GetPlanes (thePlaneEquations);
  }
}

#undef MEMORY_BLOCK_SIZE
