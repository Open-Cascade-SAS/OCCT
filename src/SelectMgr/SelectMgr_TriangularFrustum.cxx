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

#include <SelectMgr_TriangularFrustum.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_TriangularFrustum,Standard_Transient)

SelectMgr_TriangularFrustum::~SelectMgr_TriangularFrustum()
{
  Clear();
}

namespace
{
  void computeFrustumNormals (const gp_Vec* theEdges, gp_Vec* theNormals)
  {
    // V0V1
    theNormals[0] = theEdges[0].Crossed (theEdges[3]);
    // V1V2
    theNormals[1] = theEdges[1].Crossed (theEdges[4]);
    // V0V2
    theNormals[2] = theEdges[0].Crossed (theEdges[5]);
    // Near
    theNormals[3] = theEdges[3].Crossed (theEdges[4]);
    // Far
    theNormals[4] = -theNormals[3];
  }
}

// =======================================================================
// function : cacheVertexProjections
// purpose  : Caches projection of frustum's vertices onto its plane directions
//            and {i, j, k}
// =======================================================================
void SelectMgr_TriangularFrustum::cacheVertexProjections (SelectMgr_TriangularFrustum* theFrustum) const
{
  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 5; ++aPlaneIdx)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    const gp_XYZ& aPlane = theFrustum->myPlanes[aPlaneIdx].XYZ();
    for (Standard_Integer aVertIdx = 0; aVertIdx < 6; ++aVertIdx)
    {
      Standard_Real aProjection = aPlane.Dot (theFrustum->myVertices[aVertIdx].XYZ());
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    theFrustum->myMaxVertsProjections[aPlaneIdx] = aMax;
    theFrustum->myMinVertsProjections[aPlaneIdx] = aMin;
  }

  for (Standard_Integer aDim = 0; aDim < 3; ++aDim)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    for (Standard_Integer aVertIdx = 0; aVertIdx < 6; ++aVertIdx)
    {
      Standard_Real aProjection = theFrustum->myVertices[aVertIdx].XYZ().GetData()[aDim];
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    theFrustum->myMaxOrthoVertsProjections[aDim] = aMax;
    theFrustum->myMinOrthoVertsProjections[aDim] = aMin;
  }
}

//=======================================================================
// function : SelectMgr_TriangularFrustum
// purpose  : Creates new triangular frustum with bases of triangles with
//            vertices theP1, theP2 and theP3 projections onto near and
//            far view frustum planes
//=======================================================================
void SelectMgr_TriangularFrustum::Build (const gp_Pnt2d& theP1,
                                         const gp_Pnt2d& theP2,
                                         const gp_Pnt2d& theP3)
{
  // V0_Near
  myVertices[0] = myBuilder->ProjectPntOnViewPlane (theP1.X(), theP1.Y(), 0.0);
  // V1_Near
  myVertices[1] = myBuilder->ProjectPntOnViewPlane (theP2.X(), theP2.Y(), 0.0);
  // V2_Near
  myVertices[2] = myBuilder->ProjectPntOnViewPlane (theP3.X(), theP3.Y(), 0.0);
  // V0_Far
  myVertices[3] = myBuilder->ProjectPntOnViewPlane (theP1.X(), theP1.Y(), 1.0);
  // V1_Far
  myVertices[4] = myBuilder->ProjectPntOnViewPlane (theP2.X(), theP2.Y(), 1.0);
  // V2_Far
  myVertices[5] = myBuilder->ProjectPntOnViewPlane (theP3.X(), theP3.Y(), 1.0);

  // V0_Near - V0_Far
  myEdgeDirs[0] = myVertices[0].XYZ() - myVertices[3].XYZ();
  // V1_Near - V1_Far
  myEdgeDirs[1] = myVertices[1].XYZ() - myVertices[4].XYZ();
  // V2_Near - V1_Far
  myEdgeDirs[2] = myVertices[2].XYZ() - myVertices[5].XYZ();
  // V1_Near - V0_Near
  myEdgeDirs[3] = myVertices[1].XYZ() - myVertices[0].XYZ();
  // V2_Near - V1_Near
  myEdgeDirs[4] = myVertices[2].XYZ() - myVertices[1].XYZ();
  // V1_Near - V0_Near
  myEdgeDirs[5] = myVertices[2].XYZ() - myVertices[0].XYZ();

  computeFrustumNormals (myEdgeDirs, myPlanes);

  cacheVertexProjections (this);
}

//=======================================================================
// function : ScaleAndTransform
// purpose  : IMPORTANT: Scaling makes sense only for frustum built on a single point!
//            Note that this method does not perform any checks on type of the frustum.
//            Returns a copy of the frustum resized according to the scale factor given
//            and transforms it using the matrix given.
//            There are no default parameters, but in case if:
//                - transformation only is needed: @theScaleFactor must be initialized
//                  as any negative value;
//                - scale only is needed: @theTrsf must be set to gp_Identity.
//=======================================================================
Handle(SelectMgr_BaseFrustum) SelectMgr_TriangularFrustum::ScaleAndTransform (const Standard_Integer /*theScale*/,
                                                                              const gp_GTrsf& theTrsf) const
{
  Handle(SelectMgr_TriangularFrustum) aRes = new SelectMgr_TriangularFrustum();

  for (Standard_Integer anIt = 0; anIt < 6; anIt++)
  {
    gp_Pnt aPoint = myVertices[anIt];
    theTrsf.Transforms (aPoint.ChangeCoord());
    aRes->myVertices[anIt] = aPoint;
  }

  aRes->myIsOrthographic = myIsOrthographic;

  // V0_Near - V0_Far
  aRes->myEdgeDirs[0] = aRes->myVertices[0].XYZ() - aRes->myVertices[3].XYZ();
  // V1_Near - V1_Far
  aRes->myEdgeDirs[1] = aRes->myVertices[1].XYZ() - aRes->myVertices[4].XYZ();
  // V2_Near - V1_Far
  aRes->myEdgeDirs[2] = aRes->myVertices[2].XYZ() - aRes->myVertices[5].XYZ();
  // V1_Near - V0_Near
  aRes->myEdgeDirs[3] = aRes->myVertices[1].XYZ() - aRes->myVertices[0].XYZ();
  // V2_Near - V1_Near
  aRes->myEdgeDirs[4] = aRes->myVertices[2].XYZ() - aRes->myVertices[1].XYZ();
  // V1_Near - V0_Near
  aRes->myEdgeDirs[5] = aRes->myVertices[2].XYZ() - aRes->myVertices[0].XYZ();

  computeFrustumNormals (aRes->myEdgeDirs, aRes->myPlanes);

  cacheVertexProjections (aRes.get());

  return aRes;
}

//=======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and
//            given axis-aligned box
//=======================================================================
Standard_Boolean SelectMgr_TriangularFrustum::Overlaps (const SelectMgr_Vec3& theMinPt,
                                                        const SelectMgr_Vec3& theMaxPt,
                                                        Standard_Real& /*theDepth*/)
{
  return hasOverlap (theMinPt, theMaxPt);
}

// =======================================================================
// function : Overlaps
// purpose  : Returns true if selecting volume is overlapped by
//            axis-aligned bounding box with minimum corner at point
//            theMinPt and maximum at point theMaxPt
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustum::Overlaps (const SelectMgr_Vec3& theMinPt,
                                                        const SelectMgr_Vec3& theMaxPt,
                                                        Standard_Boolean* /*theInside*/)
{
  return hasOverlap (theMinPt, theMaxPt, NULL);
}

// =======================================================================
// function : Overlaps
// purpose  : Intersection test between defined volume and given point
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustum::Overlaps (const gp_Pnt& thePnt,
                                                        Standard_Real& /*theDepth*/)
{
  return hasOverlap (thePnt);
}

// =======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and given
//            ordered set of points, representing line segments. The test
//            may be considered of interior part or boundary line defined
//            by segments depending on given sensitivity type
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustum::Overlaps (const TColgp_Array1OfPnt& theArrayOfPnts,
                                                        Select3D_TypeOfSensitivity theSensType,
                                                        Standard_Real& /*theDepth*/)
{
  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    const Standard_Integer aLower  = theArrayOfPnts.Lower();
    const Standard_Integer anUpper = theArrayOfPnts.Upper();
    for (Standard_Integer aPtIdx = aLower; aPtIdx <= anUpper; ++aPtIdx)
    {
      const gp_Pnt& aStartPt = theArrayOfPnts.Value (aPtIdx);
      const gp_Pnt& aEndPt   = theArrayOfPnts.Value (aPtIdx == anUpper ? aLower : (aPtIdx + 1));
      if (!hasOverlap (aStartPt, aEndPt))
      {
        return Standard_False;
      }
    }
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    gp_Vec aNorm (gp_XYZ (RealLast(), RealLast(), RealLast()));
    return hasOverlap (theArrayOfPnts, aNorm);
  }

  return Standard_False;
}

// =======================================================================
// function : Overlaps
// purpose  : Checks if line segment overlaps selecting frustum
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustum::Overlaps (const gp_Pnt& thePnt1,
                                                        const gp_Pnt& thePnt2,
                                                        Standard_Real& /*theDepth*/)
{
  return hasOverlap (thePnt1, thePnt2);
}

// =======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and given
//            triangle. The test may be considered of interior part or
//            boundary line defined by triangle vertices depending on
//            given sensitivity type
// =======================================================================
Standard_Boolean SelectMgr_TriangularFrustum::Overlaps (const gp_Pnt& thePnt1,
                                                        const gp_Pnt& thePnt2,
                                                        const gp_Pnt& thePnt3,
                                                        Select3D_TypeOfSensitivity theSensType,
                                                        Standard_Real& theDepth)
{
  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    const gp_Pnt aPntsArrayBuf[3] = { thePnt1, thePnt2, thePnt3 };
    const TColgp_Array1OfPnt aPntsArray (aPntsArrayBuf[0], 1, 3);
    return Overlaps (aPntsArray, Select3D_TOS_BOUNDARY, theDepth);
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    gp_Vec aNorm (gp_XYZ (RealLast(), RealLast(), RealLast()));
    return hasOverlap (thePnt1, thePnt2, thePnt3, aNorm);
  }

  return Standard_True;
}

// =======================================================================
// function : Clear
// purpose  : Nullifies the handle for corresponding builder instance to prevent
//            memory leaks
// =======================================================================
void SelectMgr_TriangularFrustum::Clear()
{
  myBuilder.Nullify();
}

// =======================================================================
// function : GetPlanes
// purpose  :
// =======================================================================
void SelectMgr_TriangularFrustum::GetPlanes (NCollection_Vector<SelectMgr_Vec4>& thePlaneEquations) const
{
  SelectMgr_Vec4 aPlaneEquation;
  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 5; ++aPlaneIdx)
  {
    const gp_Vec& aNorm = myPlanes[aPlaneIdx];
    aPlaneEquation.x() = aNorm.X();
    aPlaneEquation.y() = aNorm.Y();
    aPlaneEquation.z() = aNorm.Z();
    aPlaneEquation.w() = - (aNorm.XYZ().Dot (myVertices[aPlaneIdx % 2 == 0 ? aPlaneIdx : 1].XYZ()));
    thePlaneEquations.Append (aPlaneEquation);
  }
}
