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

#define DOT(A, B) (A.x() * B.x() + A.y() * B.y() + A.z() * B.z())
#define DOTp(A, B) (A.x() * B.X() + A.y() * B.Y() + A.z() * B.Z())
#define LENGTH(A) (std::sqrt (A.x() * A.x() + A.y() * A.y() + A.z() * A.z()))

SelectMgr_TriangularFrustum::~SelectMgr_TriangularFrustum()
{
  Clear();
}

namespace
{
  void computeFrustumNormals (const SelectMgr_Vec3* theVertices, SelectMgr_Vec3* theNormals)
  {
    // V0V1
    theNormals[0] = SelectMgr_Vec3::Cross (theVertices[3] - theVertices[0],
                                           theVertices[4] - theVertices[0]);
    // V1V2
    theNormals[1] = SelectMgr_Vec3::Cross (theVertices[4] - theVertices[1],
                                           theVertices[5] - theVertices[1]);
    // V0V2
    theNormals[2] = SelectMgr_Vec3::Cross (theVertices[3] - theVertices[0],
                                           theVertices[5] - theVertices[0]);
    // Near
    theNormals[3] = SelectMgr_Vec3::Cross (theVertices[1] - theVertices[0],
                                           theVertices[2] - theVertices[0]);
    // Far
    theNormals[4] = SelectMgr_Vec3::Cross (theVertices[4] - theVertices[3],
                                           theVertices[5] - theVertices[3]);
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

  computeFrustumNormals (myVertices, myPlanes);

  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 5; ++aPlaneIdx)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    const SelectMgr_Vec3 aPlane = myPlanes[aPlaneIdx];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 6; ++aVertIdx)
    {
      Standard_Real aProjection = DOT (aPlane, myVertices[aVertIdx]);
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    myMaxVertsProjections[aPlaneIdx] = aMax;
    myMinVertsProjections[aPlaneIdx] = aMin;
  }

  SelectMgr_Vec3 aDimensions[3] =
  {
    SelectMgr_Vec3 (1.0, 0.0, 0.0),
    SelectMgr_Vec3 (0.0, 1.0, 0.0),
    SelectMgr_Vec3 (0.0, 0.0, 1.0)
  };

  for (Standard_Integer aDim = 0; aDim < 3; ++aDim)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    for (Standard_Integer aVertIdx = 0; aVertIdx < 6; ++aVertIdx)
    {
      Standard_Real aProjection = DOT (aDimensions[aDim], myVertices[aVertIdx]);
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    myMaxOrthoVertsProjections[aDim] = aMax;
    myMinOrthoVertsProjections[aDim] = aMin;
  }

  // V0_Near - V0_Far
  myEdgeDirs[0] = myVertices[0] - myVertices[3];
  // V1_Near - V1_Far
  myEdgeDirs[1] = myVertices[1] - myVertices[4];
  // V2_Near - V1_Far
  myEdgeDirs[2] = myVertices[2] - myVertices[5];
  // V1_Near - V0_Near
  myEdgeDirs[3] = myVertices[1] - myVertices[0];
  // V2_Near - V1_Near
  myEdgeDirs[4] = myVertices[2] - myVertices[1];
  // V1_Near - V0_Near
  myEdgeDirs[5] = myVertices[2] - myVertices[0];
}

//=======================================================================
// function : Transform
// purpose  : Returns a copy of the frustum transformed according to the matrix given
//=======================================================================
NCollection_Handle<SelectMgr_BaseFrustum> SelectMgr_TriangularFrustum::Transform (const gp_Trsf& theTrsf)
{
  SelectMgr_TriangularFrustum* aRes = new SelectMgr_TriangularFrustum();

  // V0_Near
  aRes->myVertices[0] = SelectMgr_MatOp::Transform (theTrsf, myVertices[0]);
  // V1_Near
  aRes->myVertices[1] = SelectMgr_MatOp::Transform (theTrsf, myVertices[1]);
  // V2_Near
  aRes->myVertices[2] = SelectMgr_MatOp::Transform (theTrsf, myVertices[2]);
  // V0_Far
  aRes->myVertices[3] = SelectMgr_MatOp::Transform (theTrsf, myVertices[3]);
  // V1_Far
  aRes->myVertices[4] = SelectMgr_MatOp::Transform (theTrsf, myVertices[4]);
  // V2_Far
  aRes->myVertices[5] = SelectMgr_MatOp::Transform (theTrsf, myVertices[5]);

  aRes->myIsOrthographic = myIsOrthographic;

  computeFrustumNormals (aRes->myVertices, aRes->myPlanes);

  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 5; ++aPlaneIdx)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    const SelectMgr_Vec3 aPlane = aRes->myPlanes[aPlaneIdx];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 6; ++aVertIdx)
    {
      Standard_Real aProjection = DOT (aPlane, aRes->myVertices[aVertIdx]);
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    aRes->myMaxVertsProjections[aPlaneIdx] = aMax;
    aRes->myMinVertsProjections[aPlaneIdx] = aMin;
  }

  SelectMgr_Vec3 aDimensions[3] =
  {
    SelectMgr_Vec3 (1.0, 0.0, 0.0),
    SelectMgr_Vec3 (0.0, 1.0, 0.0),
    SelectMgr_Vec3 (0.0, 0.0, 1.0)
  };

  for (Standard_Integer aDim = 0; aDim < 3; ++aDim)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    for (Standard_Integer aVertIdx = 0; aVertIdx < 6; ++aVertIdx)
    {
      Standard_Real aProjection = DOT (aDimensions[aDim], aRes->myVertices[aVertIdx]);
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    aRes->myMaxOrthoVertsProjections[aDim] = aMax;
    aRes->myMinOrthoVertsProjections[aDim] = aMin;
  }

  // V0_Near - V0_Far
  aRes->myEdgeDirs[0] = aRes->myVertices[0] - aRes->myVertices[3];
  // V1_Near - V1_Far
  aRes->myEdgeDirs[1] = aRes->myVertices[1] - aRes->myVertices[4];
  // V2_Near - V1_Far
  aRes->myEdgeDirs[2] = aRes->myVertices[2] - aRes->myVertices[5];
  // V1_Near - V0_Near
  aRes->myEdgeDirs[3] = aRes->myVertices[1] - aRes->myVertices[0];
  // V2_Near - V1_Near
  aRes->myEdgeDirs[4] = aRes->myVertices[2] - aRes->myVertices[1];
  // V1_Near - V0_Near
  aRes->myEdgeDirs[5] = aRes->myVertices[2] - aRes->myVertices[0];

  return NCollection_Handle<SelectMgr_BaseFrustum> (aRes);
}

//=======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and
//            given axis-aligned box
//=======================================================================
Standard_Boolean SelectMgr_TriangularFrustum::Overlaps (const BVH_Box<Standard_Real, 3>& theBox,
                                                        Standard_Real& /*theDepth*/)
{
  return hasOverlap (theBox.CornerMin(), theBox.CornerMax());
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
Standard_Boolean SelectMgr_TriangularFrustum::Overlaps (const Handle(TColgp_HArray1OfPnt)& theArrayOfPnts,
                                                        Select3D_TypeOfSensitivity theSensType,
                                                        Standard_Real& /*theDepth*/)
{
  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    Standard_Integer aLower = theArrayOfPnts->Lower();
    Standard_Integer anUpper = theArrayOfPnts->Upper();

    for (Standard_Integer aPtIdx = aLower; aPtIdx <= anUpper; ++aPtIdx)
    {
      const gp_Pnt& aStartPt = theArrayOfPnts->Value (aPtIdx);
      const gp_Pnt& aEndPt = aPtIdx == anUpper ? theArrayOfPnts->Value (aLower) : theArrayOfPnts->Value (aPtIdx + 1);

      if (!hasOverlap (aStartPt, aEndPt))
      {
        return Standard_False;
      }
    }
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    SelectMgr_Vec3 aNorm (RealLast());
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
    Handle(TColgp_HArray1OfPnt) aPtsArray = new TColgp_HArray1OfPnt(1, 4);
    aPtsArray->SetValue (1, thePnt1);
    aPtsArray->SetValue (2, thePnt2);
    aPtsArray->SetValue (3, thePnt3);
    return Overlaps (aPtsArray, Select3D_TOS_BOUNDARY, theDepth);
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    SelectMgr_Vec3 aNorm (RealLast());
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
