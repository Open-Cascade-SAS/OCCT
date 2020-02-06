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
// function : SelectMgr_TriangularFrustumSet
// purpose  :
// =======================================================================
SelectMgr_TriangularFrustumSet::SelectMgr_TriangularFrustumSet()
:  myToAllowOverlap (Standard_False)
{}

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
  Handle(BRepMesh_DataStructureOfDelaun) aMeshStructure = new BRepMesh_DataStructureOfDelaun (anAllocator);
  Standard_Integer aPtsLower = thePoints.Lower();
  Standard_Integer aPtsUpper = thePoints.Upper();
  IMeshData::VectorOfInteger anIndexes (thePoints.Size(), anAllocator);
  myBoundaryPoints.Resize (aPtsLower, aPtsLower + 2 * (thePoints.Size()) - 1, Standard_False);

  for (Standard_Integer aPtIdx = aPtsLower; aPtIdx <= aPtsUpper; ++aPtIdx)
  {
    BRepMesh_Vertex aVertex (thePoints.Value (aPtIdx).XY(), aPtIdx, BRepMesh_Frontier);
    anIndexes.Append (aMeshStructure->AddNode (aVertex));
    const gp_Pnt aNearPnt = myBuilder->ProjectPntOnViewPlane (aVertex.Coord().X(), aVertex.Coord().Y(), 0.0);
    const gp_Pnt aFarPnt  = myBuilder->ProjectPntOnViewPlane (aVertex.Coord().X(), aVertex.Coord().Y(), 1.0);
    myBoundaryPoints.SetValue (aPtIdx, aNearPnt);
    myBoundaryPoints.SetValue (aPtIdx + thePoints.Size(), aFarPnt);
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
  const IMeshData::MapOfInteger& aTriangles = aMeshStructure->ElementsOfDomain();
  if (aTriangles.Extent() < 1)
    return;

  IMeshData::IteratorOfMapOfInteger aTriangleIt (aTriangles);
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

  aRes->myBoundaryPoints.Resize (myBoundaryPoints.Lower(), myBoundaryPoints.Upper(), Standard_False);
  for (Standard_Integer anIdx = myBoundaryPoints.Lower(); anIdx <= myBoundaryPoints.Upper(); anIdx++)
  {
    gp_Pnt aPoint = myBoundaryPoints.Value (anIdx);
    theTrsf.Transforms (aPoint.ChangeCoord());
    aRes->myBoundaryPoints.SetValue (anIdx, aPoint);
  }

  return aRes;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const SelectMgr_Vec3& theMinPnt,
                                                           const SelectMgr_Vec3& theMaxPnt,
                                                           const SelectMgr_ViewClipRange& theClipRange,
                                                           SelectBasics_PickResult& thePickResult) const
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (theMinPnt, theMaxPnt, theClipRange, thePickResult))
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
                                                           Standard_Boolean* theInside) const
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (theMinPnt, theMaxPnt, NULL))
    {
      if (myToAllowOverlap || theInside == NULL)
      {
        return Standard_True;
      }
      else
      {
        gp_Pnt aMinMaxPnts[2] = { gp_Pnt (theMinPnt.x(), theMinPnt.y(), theMinPnt.z()),
                                  gp_Pnt (theMaxPnt.x(), theMaxPnt.y(), theMaxPnt.z())};

        gp_Pnt anOffset[3] = { gp_Pnt (aMinMaxPnts[1].X() - aMinMaxPnts[0].X(), 0.0, 0.0),
                               gp_Pnt (0.0, aMinMaxPnts[1].Y() - aMinMaxPnts[0].Y(), 0.0),
                               gp_Pnt (0.0, 0.0, aMinMaxPnts[1].Z() - aMinMaxPnts[0].Z()) };

        Standard_Integer aSign = 1;
        for (Standard_Integer aPntsIdx = 0; aPntsIdx < 2; aPntsIdx++)
        {
          for (Standard_Integer aCoordIdx = 0; aCoordIdx < 3; aCoordIdx++)
          {
            gp_Pnt anOffsetPnt = aMinMaxPnts [aPntsIdx].XYZ() + aSign * anOffset [aCoordIdx].XYZ();
            if (isIntersectBoundary (aMinMaxPnts [aPntsIdx], anOffsetPnt)
             || isIntersectBoundary (anOffsetPnt, anOffsetPnt.XYZ() + aSign * anOffset [(aCoordIdx + 1) % 3].XYZ()))
            {
              *theInside &= Standard_False;
              return Standard_True;
            }
          }
          aSign = -aSign;
        }
        return Standard_True;
      }
    }
  }

  return Standard_False;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const gp_Pnt& thePnt,
                                                           const SelectMgr_ViewClipRange& theClipRange,
                                                           SelectBasics_PickResult& thePickResult) const
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (thePnt, theClipRange, thePickResult))
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
                                                           const SelectMgr_ViewClipRange& theClipRange,
                                                           SelectBasics_PickResult& thePickResult) const
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (theArrayOfPts, theSensType, theClipRange, thePickResult))
    {
      if (myToAllowOverlap)
      {
        return Standard_True;
      }
      else
      {
        Standard_Integer aPtsLower = theArrayOfPts.Lower();
        Standard_Integer aPtsUpper = theArrayOfPts.Upper();
        for (Standard_Integer anIdx = aPtsLower; anIdx <= aPtsUpper; anIdx++)
        {
          if (isIntersectBoundary (theArrayOfPts.Value (anIdx), theArrayOfPts.Value (anIdx < aPtsUpper ? anIdx + 1 : aPtsLower)))
          {
            return Standard_False;
          }
        }
        return Standard_True;
      }
    }
  }

  return Standard_False;
}

// =======================================================================
// function : Overlaps
// purpose  :
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::Overlaps (const gp_Pnt& thePnt1,
                                                           const gp_Pnt& thePnt2,
                                                           const SelectMgr_ViewClipRange& theClipRange,
                                                           SelectBasics_PickResult& thePickResult) const
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (thePnt1, thePnt2, theClipRange, thePickResult))
    {
      if (myToAllowOverlap)
      {
        return Standard_True;
      }
      else
      {
        if (isIntersectBoundary (thePnt1, thePnt2))
        {
          return Standard_False;
        }
        return Standard_True;
      }
    }
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
                                                           const SelectMgr_ViewClipRange& theClipRange,
                                                           SelectBasics_PickResult& thePickResult) const
{
  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    if (anIter.Value()->Overlaps (thePnt1, thePnt2, thePnt3, theSensType, theClipRange, thePickResult))
    {
      if (myToAllowOverlap)
      {
        return Standard_True;
      }
      else
      {
        if (isIntersectBoundary (thePnt1, thePnt2)
         || isIntersectBoundary (thePnt2, thePnt3)
         || isIntersectBoundary (thePnt3, thePnt1))
        {
          return Standard_False;
        }
        return Standard_True;
      }
    }
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

//=======================================================================
// function : SetAllowOverlapDetection
// purpose  :
//=======================================================================
void SelectMgr_TriangularFrustumSet::SetAllowOverlapDetection (const Standard_Boolean theIsToAllow)
{
  myToAllowOverlap = theIsToAllow;
}

//=======================================================================
// function : isIntersectBoundary
// purpose  :
//=======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::isIntersectBoundary (const gp_Pnt& thePnt1, const gp_Pnt& thePnt2) const
{
  Standard_Integer aFacesNb = myBoundaryPoints.Size() / 2;
  gp_Vec aDir = thePnt2.XYZ() - thePnt1.XYZ();
  gp_Pnt anOrig = thePnt1;

  for (Standard_Integer anIdx = myBoundaryPoints.Lower(); anIdx < aFacesNb + myBoundaryPoints.Lower(); anIdx++)
  {
    gp_Pnt aFace[4] = { myBoundaryPoints.Value (anIdx),
                        myBoundaryPoints.Value (anIdx + aFacesNb),
                        myBoundaryPoints.Value (anIdx % aFacesNb + 1 + aFacesNb),
                        myBoundaryPoints.Value (anIdx % aFacesNb + 1) };

    if (segmentTriangleIntersection (anOrig, aDir, aFace[0], aFace[1], aFace[2])
     || segmentTriangleIntersection (anOrig, aDir, aFace[0], aFace[2], aFace[3]))
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
// function : segmentTriangleIntersection
// purpose  : Moller-Trumbore ray-triangle intersection test
//=======================================================================
Standard_Boolean SelectMgr_TriangularFrustumSet::segmentTriangleIntersection (const gp_Pnt& theOrig, const gp_Vec& theDir,
                                                                              const gp_Pnt& theV1, const gp_Pnt& theV2, const gp_Pnt& theV3) const
{
  gp_Vec        aPVec, aTVec, aQVec;
  Standard_Real aD, aInvD, anU, aV, aT;

  gp_Vec anEdge1 = theV2.XYZ() - theV1.XYZ();
  gp_Vec anEdge2 = theV3.XYZ() - theV1.XYZ();

  aPVec = theDir.Crossed (anEdge2);
  aD = anEdge1.Dot (aPVec);
  if (fabs (aD) < gp::Resolution())
  {
    return Standard_False;
  }

  aInvD = 1.0 / aD;
  aTVec = theOrig.XYZ() - theV1.XYZ();
  anU = aInvD * aTVec.Dot (aPVec);
  if (anU < 0.0 || anU > 1.0)
  {
    return Standard_False;
  }

  aQVec = aTVec.Crossed (anEdge1);
  aV = aInvD * theDir.Dot (aQVec);
  if (aV < 0.0 || anU + aV > 1.0)
  {
    return Standard_False;
  }

  aT = aInvD * anEdge2.Dot (aQVec);
  if (aT < 0 || aT > 1)
  {
    return Standard_False;
  }

  return Standard_True;
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void SelectMgr_TriangularFrustumSet::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, SelectMgr_TriangularFrustumSet)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, SelectMgr_BaseFrustum)

  for (SelectMgr_TriangFrustumsIter anIter (myFrustums); anIter.More(); anIter.Next())
  {
    const Handle(SelectMgr_TriangularFrustum)& aFrustum = anIter.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, aFrustum.get())
  }
}

#undef MEMORY_BLOCK_SIZE
