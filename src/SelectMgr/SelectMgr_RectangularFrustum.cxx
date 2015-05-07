// Created on: 2014-05-22
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

#include <NCollection_Vector.hxx>
#include <Poly_Array1OfTriangle.hxx>

#include <SelectMgr_RectangularFrustum.hxx>

#define DOT(A, B) (A.x() * B.x() + A.y() * B.y() + A.z() * B.z())
#define DOTp(A, B) (A.x() * B.X() + A.y() * B.Y() + A.z() * B.Z())
#define DISTANCE(A, B) (std::sqrt ((A.x() - B.x()) * (A.x() - B.x()) + (A.y() - B.y()) * (A.y() - B.y()) + (A.z() - B.z()) * (A.z() - B.z())))
#define DISTANCEp(A, B) (std::sqrt ((A.x() - B.X()) * (A.x() - B.X()) + (A.y() - B.Y()) * (A.y() - B.Y()) + (A.z() - B.Z()) * (A.z() - B.Z())))

// =======================================================================
// function : segmentSegmentDistance
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::segmentSegmentDistance (const gp_Pnt& theSegPnt1,
                                                           const gp_Pnt& theSegPnt2,
                                                           Standard_Real& theDepth)
{
  SelectMgr_Vec3 anU = SelectMgr_Vec3 (theSegPnt2.X() - theSegPnt1.X(),
                                       theSegPnt2.Y() - theSegPnt1.Y(),
                                       theSegPnt2.Z() - theSegPnt1.Z());
  SelectMgr_Vec3 aV = myViewRayDir;
  SelectMgr_Vec3 aW = SelectMgr_Vec3 (theSegPnt1.X() - myNearPickedPnt.x(),
                                      theSegPnt1.Y() - myNearPickedPnt.y(),
                                      theSegPnt1.Z() - myNearPickedPnt.z());
  Standard_Real anA = DOT (anU, anU);
  Standard_Real aB = DOT (anU, aV);
  Standard_Real aC = DOT (aV, aV);
  Standard_Real aD = DOT (anU, aW);
  Standard_Real anE = DOT (aV, aW);
  Standard_Real aCoef = anA * aC - aB * aB;
  Standard_Real aSc, aSn, aSd = aCoef;
  Standard_Real aTc, aTn, aTd = aCoef;

  if (aCoef < Precision::Confusion())
  {
    aSn = 0.0;
    aSd = 1.0;
    aTn = anE;
    aTd = aC;
  }
  else
  {
    aSn = (aB * anE - aC * aD);
    aTn = (anA * anE - aB * aD);
    if (aSn < 0.0)
    {
      aSn = 0.0;
      aTn = anE;
      aTd = aC;
    }
    else if (aSn > aSd)
    {
      aSn = aSd;
      aTn = anE + aB;
      aTd = aC;
    }
  }

  if (aTn < 0.0)
  {
    aTn = 0.0;
    if (-aD < 0.0)
      aSn = 0.0;
    else if (-aD > anA)
      aSn = aSd;
    else {
      aSn = -aD;
      aSd = anA;
    }
  }
  else if (aTn > aTd)
  {
    aTn = aTd;
    if ((-aD + aB) < 0.0)
      aSn = 0;
    else if ((-aD + aB) > anA)
      aSn = aSd;
    else {
      aSn = (-aD +  aB);
      aSd = anA;
    }
  }
  aSc = (Abs (aSn) < Precision::Confusion() ? 0.0 : aSn / aSd);
  aTc = (Abs (aTn) < Precision::Confusion() ? 0.0 : aTn / aTd);

  SelectMgr_Vec3 aClosestPnt = myNearPickedPnt + myViewRayDir * aTc;
  theDepth = DISTANCE (myNearPickedPnt, aClosestPnt);
}

// =======================================================================
// function : segmentPlaneIntersection
// purpose  :
// =======================================================================
void SelectMgr_RectangularFrustum::segmentPlaneIntersection (const SelectMgr_Vec3& thePlane,
                                                             const gp_Pnt& thePntOnPlane,
                                                             Standard_Real& theDepth)
{
  SelectMgr_Vec3 anU = myViewRayDir;
  SelectMgr_Vec3 aW = SelectMgr_Vec3 (myNearPickedPnt.x() - thePntOnPlane.X(),
                                      myNearPickedPnt.y() - thePntOnPlane.Y(),
                                      myNearPickedPnt.z() - thePntOnPlane.Z());
  Standard_Real aD = DOT (thePlane, anU);
  Standard_Real aN = -DOT (thePlane, aW);

  if (Abs (aD) < Precision::Confusion())
  {
    if (Abs (aN) < Precision::Angular())
    {
      theDepth = DBL_MAX;
      return;
    }
    else
    {
      theDepth = DBL_MAX;
      return;
    }
  }

  Standard_Real aParam = aN / aD;
  if (aParam < 0.0 || aParam > 1.0)
  {
    theDepth = DBL_MAX;
    return;
  }

  SelectMgr_Vec3 aClosestPnt = myNearPickedPnt + anU * aParam;
  theDepth = DISTANCE (myNearPickedPnt, aClosestPnt);
}

namespace
{
  // =======================================================================
  // function : computeNormals
  // purpose  : Computes normals to frustum faces
  // =======================================================================
  void computeNormals (const SelectMgr_Vec3* theVertices, SelectMgr_Vec3* theNormals)
  {
    // Top
    theNormals[0] = SelectMgr_Vec3::Cross (theVertices[1] - theVertices[0],
                                           theVertices[4] - theVertices[0]);
    // Bottom
    theNormals[1] = SelectMgr_Vec3::Cross (theVertices[3] - theVertices[2],
                                           theVertices[6] - theVertices[2]);
    // Left
    theNormals[2] = SelectMgr_Vec3::Cross (theVertices[1] - theVertices[0],
                                           theVertices[2] - theVertices[0]);
    // Right
    theNormals[3] = SelectMgr_Vec3::Cross (theVertices[5] - theVertices[4],
                                           theVertices[6] - theVertices[4]);
    // Near
    theNormals[4] = SelectMgr_Vec3::Cross (theVertices[6] - theVertices[4],
                                           theVertices[0] - theVertices[4]);
    // Far
    theNormals[5] = SelectMgr_Vec3::Cross (theVertices[7] - theVertices[5],
                                           theVertices[1] - theVertices[5]);
  }
}

// =======================================================================
// function : Build
// purpose  : Build volume according to the point and given pixel
//            tolerance
// =======================================================================
void SelectMgr_RectangularFrustum::Build (const gp_Pnt2d &thePoint)
{
  myNearPickedPnt = myBuilder->ProjectPntOnViewPlane (thePoint.X(), thePoint.Y(), 0.0);
  myFarPickedPnt  = myBuilder->ProjectPntOnViewPlane (thePoint.X(), thePoint.Y(), 1.0);
  myViewRayDir = myFarPickedPnt - myNearPickedPnt;
  myMousePos = thePoint;

  // LeftTopNear
  myVertices[0] = myBuilder->ProjectPntOnViewPlane (thePoint.X() - myPixelTolerance / 2.0,
                                                    thePoint.Y() + myPixelTolerance / 2.0,
                                                    0.0);
  // LeftTopFar
  myVertices[1] = myBuilder->ProjectPntOnViewPlane (thePoint.X() - myPixelTolerance / 2.0,
                                                    thePoint.Y() + myPixelTolerance / 2.0,
                                                    1.0);
  // LeftBottomNear
  myVertices[2] = myBuilder->ProjectPntOnViewPlane (thePoint.X() - myPixelTolerance / 2.0,
                                                    thePoint.Y() - myPixelTolerance / 2.0,
                                                    0.0);
  // LeftBottomFar
  myVertices[3] = myBuilder->ProjectPntOnViewPlane (thePoint.X() - myPixelTolerance / 2.0,
                                                    thePoint.Y() - myPixelTolerance / 2.0,
                                                    1.0);
  // RightTopNear
  myVertices[4] = myBuilder->ProjectPntOnViewPlane (thePoint.X() + myPixelTolerance / 2.0,
                                                    thePoint.Y() + myPixelTolerance / 2.0,
                                                    0.0);
  // RightTopFar
  myVertices[5] = myBuilder->ProjectPntOnViewPlane (thePoint.X() + myPixelTolerance / 2.0,
                                                    thePoint.Y() + myPixelTolerance / 2.0,
                                                    1.0);
  // RightBottomNear
  myVertices[6] = myBuilder->ProjectPntOnViewPlane (thePoint.X() + myPixelTolerance / 2.0,
                                                    thePoint.Y() - myPixelTolerance / 2.0,
                                                    0.0);
  // RightBottomFar
  myVertices[7] = myBuilder->ProjectPntOnViewPlane (thePoint.X() + myPixelTolerance / 2.0,
                                                    thePoint.Y() - myPixelTolerance / 2.0,
                                                    1.0);

  // compute frustum normals
  computeNormals (myVertices, myPlanes);

  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 6; ++aPlaneIdx)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    const SelectMgr_Vec3 aPlane = myPlanes[aPlaneIdx];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
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
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
    {
      Standard_Real aProjection = DOT (aDimensions[aDim], myVertices[aVertIdx]);
      aMax = Max (aProjection, aMax);
      aMin = Min (aProjection, aMin);
    }
    myMaxOrthoVertsProjections[aDim] = aMax;
    myMinOrthoVertsProjections[aDim] = aMin;
  }

  // Horizontal
  myEdgeDirs[0] = myVertices[4] - myVertices[0];
  // Vertical
  myEdgeDirs[1] = myVertices[2] - myVertices[0];
  // LeftLower
  myEdgeDirs[2] = myVertices[2] - myVertices[3];
  // RightLower
  myEdgeDirs[3] = myVertices[6] - myVertices[7];
  // LeftUpper
  myEdgeDirs[4] = myVertices[0] - myVertices[1];
  // RightUpper
  myEdgeDirs[5] = myVertices[4] - myVertices[5];
}

// =======================================================================
// function : Build
// purpose  : Build volume according to the selected rectangle
// =======================================================================
void SelectMgr_RectangularFrustum::Build (const gp_Pnt2d& theMinPnt,
                                          const gp_Pnt2d& theMaxPnt)
{
  myNearPickedPnt = myBuilder->ProjectPntOnViewPlane ((theMinPnt.X() + theMaxPnt.X()) * 0.5,
                                                      (theMinPnt.Y() + theMaxPnt.Y()) * 0.5,
                                                      0.0);
  myFarPickedPnt = myBuilder->ProjectPntOnViewPlane ((theMinPnt.X() + theMaxPnt.X()) * 0.5,
                                                     (theMinPnt.Y() + theMaxPnt.Y()) * 0.5,
                                                     1.0);
  myViewRayDir = myFarPickedPnt - myNearPickedPnt;

  // LeftTopNear
  myVertices[0] = myBuilder->ProjectPntOnViewPlane (theMinPnt.X(),
                                                    theMaxPnt.Y(),
                                                    0.0);
  // LeftTopFar
  myVertices[1] = myBuilder->ProjectPntOnViewPlane (theMinPnt.X(),
                                                    theMaxPnt.Y(),
                                                    1.0);
  // LeftBottomNear
  myVertices[2] = myBuilder->ProjectPntOnViewPlane (theMinPnt.X(),
                                                    theMinPnt.Y(),
                                                    0.0);
  // LeftBottomFar
  myVertices[3] = myBuilder->ProjectPntOnViewPlane (theMinPnt.X(),
                                                    theMinPnt.Y(),
                                                    1.0);
  // RightTopNear
  myVertices[4] = myBuilder->ProjectPntOnViewPlane (theMaxPnt.X(),
                                                    theMaxPnt.Y(),
                                                    0.0);
  // RightTopFar
  myVertices[5] = myBuilder->ProjectPntOnViewPlane (theMaxPnt.X(),
                                                    theMaxPnt.Y(),
                                                    1.0);
  // RightBottomNear
  myVertices[6] = myBuilder->ProjectPntOnViewPlane (theMaxPnt.X(),
                                                    theMinPnt.Y(),
                                                    0.0);
  // RightBottomFar
  myVertices[7] = myBuilder->ProjectPntOnViewPlane (theMaxPnt.X(),
                                                    theMinPnt.Y(),
                                                    1.0);

  // compute frustum normals
  computeNormals (myVertices, myPlanes);

  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 6; ++aPlaneIdx)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    const SelectMgr_Vec3 aPlane = myPlanes[aPlaneIdx];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
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
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
    {
      Standard_Real aProjection = DOT (aDimensions[aDim], myVertices[aVertIdx]);
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    myMaxOrthoVertsProjections[aDim] = aMax;
    myMinOrthoVertsProjections[aDim] = aMin;
  }

  // Horizontal
  myEdgeDirs[0] = myVertices[4] - myVertices[0];
  // Vertical
  myEdgeDirs[1] = myVertices[2] - myVertices[0];
  // LeftLower
  myEdgeDirs[2] = myVertices[2] - myVertices[3];
  // RightLower
  myEdgeDirs[3] = myVertices[6] - myVertices[7];
  // LeftUpper
  myEdgeDirs[4] = myVertices[0] - myVertices[1];
  // RightUpper
  myEdgeDirs[5] = myVertices[4] - myVertices[5];
}

// =======================================================================
// function : Transform
// purpose  : Returns a copy of the frustum transformed according to the matrix given
// =======================================================================
NCollection_Handle<SelectMgr_BaseFrustum> SelectMgr_RectangularFrustum::Transform (const gp_Trsf& theTrsf)
{
  SelectMgr_RectangularFrustum* aRes = new SelectMgr_RectangularFrustum();

  aRes->myNearPickedPnt = SelectMgr_MatOp::Transform (theTrsf, myNearPickedPnt);
  aRes->myFarPickedPnt  = SelectMgr_MatOp::Transform (theTrsf, myFarPickedPnt);
  aRes->myViewRayDir    = aRes->myFarPickedPnt - aRes->myNearPickedPnt;

  aRes->myIsOrthographic = myIsOrthographic;

  // LeftTopNear
  aRes->myVertices[0] = SelectMgr_MatOp::Transform (theTrsf, myVertices[0]);
  // LeftTopFar
  aRes->myVertices[1] = SelectMgr_MatOp::Transform (theTrsf, myVertices[1]);
  // LeftBottomNear
  aRes->myVertices[2] = SelectMgr_MatOp::Transform (theTrsf, myVertices[2]);
  // LeftBottomFar
  aRes->myVertices[3] = SelectMgr_MatOp::Transform (theTrsf, myVertices[3]);
  // RightTopNear
  aRes->myVertices[4] = SelectMgr_MatOp::Transform (theTrsf, myVertices[4]);
  // RightTopFar
  aRes->myVertices[5] = SelectMgr_MatOp::Transform (theTrsf, myVertices[5]);
  // RightBottomNear
  aRes->myVertices[6] = SelectMgr_MatOp::Transform (theTrsf, myVertices[6]);
  // RightBottomFar
  aRes->myVertices[7] = SelectMgr_MatOp::Transform (theTrsf, myVertices[7]);

  // compute frustum normals
  computeNormals (aRes->myVertices, aRes->myPlanes);

  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 6; ++aPlaneIdx)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    const SelectMgr_Vec3 aPlane = aRes->myPlanes[aPlaneIdx];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
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
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
    {
      Standard_Real aProjection = DOT (aDimensions[aDim], aRes->myVertices[aVertIdx]);
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    aRes->myMaxOrthoVertsProjections[aDim] = aMax;
    aRes->myMinOrthoVertsProjections[aDim] = aMin;
  }

  // Horizontal
  aRes->myEdgeDirs[0] = aRes->myVertices[4] - aRes->myVertices[0];
  // Vertical
  aRes->myEdgeDirs[1] = aRes->myVertices[2] - aRes->myVertices[0];
  // LeftLower
  aRes->myEdgeDirs[2] = aRes->myVertices[2] - aRes->myVertices[3];
  // RightLower
  aRes->myEdgeDirs[3] = aRes->myVertices[6] - aRes->myVertices[7];
  // LeftUpper
  aRes->myEdgeDirs[4] = aRes->myVertices[0] - aRes->myVertices[1];
  // RightUpper
  aRes->myEdgeDirs[5] = aRes->myVertices[4] - aRes->myVertices[5];

  return NCollection_Handle<SelectMgr_BaseFrustum> (aRes);
}

// =======================================================================
// function : Scale
// purpose  : IMPORTANT: Makes sense only for frustum built on a single point!
//            Returns a copy of the frustum resized according to the scale factor given
// =======================================================================
NCollection_Handle<SelectMgr_BaseFrustum> SelectMgr_RectangularFrustum::Scale (const Standard_Real theScaleFactor)
{
  SelectMgr_RectangularFrustum* aRes = new SelectMgr_RectangularFrustum();

  aRes->myNearPickedPnt = myNearPickedPnt;
  aRes->myFarPickedPnt  = myFarPickedPnt;
  aRes->myViewRayDir    = myViewRayDir;

  aRes->myIsOrthographic = myIsOrthographic;

    // LeftTopNear
  aRes->myVertices[0] = myBuilder->ProjectPntOnViewPlane (myMousePos.X() - theScaleFactor / 2.0,
                                                          myMousePos.Y() + theScaleFactor / 2.0,
                                                          0.0);
  // LeftTopFar
  aRes->myVertices[1] = myBuilder->ProjectPntOnViewPlane (myMousePos.X() - theScaleFactor / 2.0,
                                                          myMousePos.Y() + theScaleFactor / 2.0,
                                                          1.0);
  // LeftBottomNear
  aRes->myVertices[2] = myBuilder->ProjectPntOnViewPlane (myMousePos.X() - theScaleFactor / 2.0,
                                                          myMousePos.Y() - theScaleFactor / 2.0,
                                                          0.0);
  // LeftBottomFar
  aRes->myVertices[3] = myBuilder->ProjectPntOnViewPlane (myMousePos.X() - theScaleFactor / 2.0,
                                                          myMousePos.Y() - theScaleFactor / 2.0,
                                                          1.0);
  // RightTopNear
  aRes->myVertices[4] = myBuilder->ProjectPntOnViewPlane (myMousePos.X() + theScaleFactor / 2.0,
                                                          myMousePos.Y() + theScaleFactor / 2.0,
                                                          0.0);
  // RightTopFar
  aRes->myVertices[5] = myBuilder->ProjectPntOnViewPlane (myMousePos.X() + theScaleFactor / 2.0,
                                                          myMousePos.Y() + theScaleFactor / 2.0,
                                                          1.0);
  // RightBottomNear
  aRes->myVertices[6] = myBuilder->ProjectPntOnViewPlane (myMousePos.X() + theScaleFactor / 2.0,
                                                          myMousePos.Y() - theScaleFactor / 2.0,
                                                          0.0);
  // RightBottomFar
  aRes->myVertices[7] = myBuilder->ProjectPntOnViewPlane (myMousePos.X() + theScaleFactor / 2.0,
                                                          myMousePos.Y() - theScaleFactor / 2.0,
                                                          1.0);
  // compute frustum normals
  computeNormals (aRes->myVertices, aRes->myPlanes);

  for (Standard_Integer aPlaneIdx = 0; aPlaneIdx < 6; ++aPlaneIdx)
  {
    Standard_Real aMax = -DBL_MAX;
    Standard_Real aMin =  DBL_MAX;
    const SelectMgr_Vec3 aPlane = aRes->myPlanes[aPlaneIdx];
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
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
    for (Standard_Integer aVertIdx = 0; aVertIdx < 8; ++aVertIdx)
    {
      Standard_Real aProjection = DOT (aDimensions[aDim], aRes->myVertices[aVertIdx]);
      aMax = Max (aMax, aProjection);
      aMin = Min (aMin, aProjection);
    }
    aRes->myMaxOrthoVertsProjections[aDim] = aMax;
    aRes->myMinOrthoVertsProjections[aDim] = aMin;
  }

  // Horizontal
  aRes->myEdgeDirs[0] = aRes->myVertices[4] - aRes->myVertices[0];
  // Vertical
  aRes->myEdgeDirs[1] = aRes->myVertices[2] - aRes->myVertices[0];
  // LeftLower
  aRes->myEdgeDirs[2] = aRes->myVertices[2] - aRes->myVertices[3];
  // RightLower
  aRes->myEdgeDirs[3] = aRes->myVertices[6] - aRes->myVertices[7];
  // LeftUpper
  aRes->myEdgeDirs[4] = aRes->myVertices[0] - aRes->myVertices[1];
  // RightUpper
  aRes->myEdgeDirs[5] = aRes->myVertices[4] - aRes->myVertices[5];

  return NCollection_Handle<SelectMgr_BaseFrustum> (aRes);
}

// =======================================================================
// function : Overlaps
// purpose  : Returns true if selecting volume is overlapped by
//            axis-aligned bounding box with minimum corner at point
//            theMinPnt and maximum at point theMaxPnt
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const SelectMgr_Vec3& theBoxMin,
                                                         const SelectMgr_Vec3& theBoxMax,
                                                         Standard_Boolean*     theInside)
{
  return hasOverlap (theBoxMin, theBoxMax, theInside);
}

// =======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and
//            given axis-aligned box
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const BVH_Box<Standard_Real, 3>& theBox,
                                                         Standard_Real& theDepth)
{
  const SelectMgr_Vec3& aMinPnt = theBox.CornerMin();
  const SelectMgr_Vec3& aMaxPnt = theBox.CornerMax();
  if (!hasOverlap (aMinPnt, aMaxPnt))
    return Standard_False;

  SelectMgr_Vec3 aNearestPnt = SelectMgr_Vec3 (RealLast(), RealLast(), RealLast());
  aNearestPnt.x() = Max (Min (myNearPickedPnt.x(), aMaxPnt.x()), aMinPnt.x());
  aNearestPnt.y() = Max (Min (myNearPickedPnt.y(), aMaxPnt.y()), aMinPnt.y());
  aNearestPnt.z() = Max (Min (myNearPickedPnt.z(), aMaxPnt.z()), aMinPnt.z());

  theDepth = DISTANCE (aNearestPnt, myNearPickedPnt);

  return Standard_True;
}

// =======================================================================
// function : Overlaps
// purpose  : Intersection test between defined volume and given point
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const gp_Pnt& thePnt,
                                                         Standard_Real& theDepth)
{
  if (!hasOverlap (thePnt))
    return Standard_False;

  SelectMgr_Vec3 aPnt (thePnt.X(), thePnt.Y(), thePnt.Z());
  SelectMgr_Vec3 aV = aPnt - myNearPickedPnt;
  SelectMgr_Vec3 aDetectedPnt = myNearPickedPnt + myViewRayDir * (DOT (aV, myViewRayDir) / DOT (myViewRayDir, myViewRayDir));

  theDepth = DISTANCE (aDetectedPnt, myNearPickedPnt);

  return Standard_True;
}

// =======================================================================
// function : Overlaps
// purpose  : Checks if line segment overlaps selecting frustum
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const gp_Pnt& thePnt1,
                                                         const gp_Pnt& thePnt2,
                                                         Standard_Real& theDepth)
{
  theDepth = -DBL_MAX;
  if (!hasOverlap (thePnt1, thePnt2))
    return Standard_False;

  segmentSegmentDistance (thePnt1, thePnt2, theDepth);
  return Standard_True;
}

// =======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and given
//            ordered set of points, representing line segments. The test
//            may be considered of interior part or boundary line defined
//            by segments depending on given sensitivity type
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const Handle(TColgp_HArray1OfPnt)& theArrayOfPnts,
                                                         Select3D_TypeOfSensitivity theSensType,
                                                         Standard_Real& theDepth)
{
  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    Standard_Integer aMatchingSegmentsNb = -1;
    theDepth = DBL_MAX;
    Standard_Integer aLower = theArrayOfPnts->Lower();
    Standard_Integer anUpper = theArrayOfPnts->Upper();

    for (Standard_Integer aPntIter = aLower; aPntIter <= anUpper; ++aPntIter)
    {
      const gp_Pnt& aStartPnt = theArrayOfPnts->Value (aPntIter);
      const gp_Pnt& aEndPnt = aPntIter == anUpper ? theArrayOfPnts->Value (aLower)
        : theArrayOfPnts->Value (aPntIter + 1);

      if (hasOverlap (aStartPnt, aEndPnt))
      {
        aMatchingSegmentsNb++;
        Standard_Real aSegmentDepth = RealLast();
        segmentSegmentDistance (aStartPnt, aEndPnt, aSegmentDepth);
        theDepth = Min (theDepth, aSegmentDepth);
      }
    }

    if (aMatchingSegmentsNb == -1)
      return Standard_False;
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    SelectMgr_Vec3 aPolyNorm (RealLast());
    if (!hasOverlap (theArrayOfPnts, aPolyNorm))
      return Standard_False;

    segmentPlaneIntersection (aPolyNorm,
                              theArrayOfPnts->Value (theArrayOfPnts->Lower()),
                              theDepth);
  }

  return Standard_True;
}

// =======================================================================
// function : Overlaps
// purpose  : SAT intersection test between defined volume and given
//            triangle. The test may be considered of interior part or
//            boundary line defined by triangle vertices depending on
//            given sensitivity type
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::Overlaps (const gp_Pnt& thePnt1,
                                                         const gp_Pnt& thePnt2,
                                                         const gp_Pnt& thePnt3,
                                                         Select3D_TypeOfSensitivity theSensType,
                                                         Standard_Real& theDepth)
{
  if (theSensType == Select3D_TOS_BOUNDARY)
  {
    Handle(TColgp_HArray1OfPnt) aPntsArray = new TColgp_HArray1OfPnt(1, 4);
    aPntsArray->SetValue (1, thePnt1);
    aPntsArray->SetValue (2, thePnt2);
    aPntsArray->SetValue (3, thePnt3);
    aPntsArray->SetValue (4, thePnt1);
    return Overlaps (aPntsArray, Select3D_TOS_BOUNDARY, theDepth);
  }
  else if (theSensType == Select3D_TOS_INTERIOR)
  {
    SelectMgr_Vec3 aTriangleNormal (RealLast());
    if (!hasOverlap (thePnt1, thePnt2, thePnt3, aTriangleNormal))
      return Standard_False;

    // check if intersection point belongs to triangle's interior part
    SelectMgr_Vec3 aPnt1 (thePnt1.X(), thePnt1.Y(), thePnt1.Z());
    SelectMgr_Vec3 aTrEdges[3] = { SelectMgr_Vec3 (thePnt2.X() - thePnt1.X(), thePnt2.Y() - thePnt1.Y(), thePnt2.Z() - thePnt1.Z()),
                                   SelectMgr_Vec3 (thePnt3.X() - thePnt2.X(), thePnt3.Y() - thePnt2.Y(), thePnt3.Z() - thePnt2.Z()),
                                   SelectMgr_Vec3 (thePnt1.X() - thePnt3.X(), thePnt1.Y() - thePnt3.Y(), thePnt1.Z() - thePnt3.Z()) };
    SelectMgr_Vec3 anEdge = (aPnt1 - myNearPickedPnt) * (1.0 / DOT (aTriangleNormal, myViewRayDir));

    Standard_Real aTime = DOT (aTriangleNormal, anEdge);

    SelectMgr_Vec3 aVec = SelectMgr_Vec3 (myViewRayDir.y() * anEdge.z() - myViewRayDir.z() * anEdge.y(),
                                          myViewRayDir.z() * anEdge.x() - myViewRayDir.x() * anEdge.z(),
                                          myViewRayDir.x() * anEdge.y() - myViewRayDir.y() * anEdge.x());

    Standard_Real anU = DOT (aVec, aTrEdges[2]);
    Standard_Real aV  = DOT (aVec, aTrEdges[0]);

    Standard_Boolean isInterior = (aTime >= 0.0) && (anU >= 0.0) && (aV >= 0.0) && (anU + aV <= 1.0);

    if (isInterior)
    {
      SelectMgr_Vec3 aDetectedPnt = myNearPickedPnt + myViewRayDir * aTime;
      theDepth = DISTANCE (myNearPickedPnt, aDetectedPnt);
      return Standard_True;
    }

    gp_Pnt aPnts[3] = {thePnt1, thePnt2, thePnt3};
    Standard_Real aMinDist = RealLast();
    Standard_Integer aNearestEdgeIdx = -1;
    SelectMgr_Vec3 aPtOnPlane = myNearPickedPnt + myViewRayDir * aTime;
    for (Standard_Integer anEdgeIdx = 0; anEdgeIdx < 3; ++anEdgeIdx)
    {
      SelectMgr_Vec3 aW = SelectMgr_Vec3 (aPtOnPlane.x() - aPnts[anEdgeIdx].X(),
                                          aPtOnPlane.y() - aPnts[anEdgeIdx].Y(),
                                          aPtOnPlane.z() - aPnts[anEdgeIdx].Z());
      Standard_Real aCoef = DOT (aTrEdges[anEdgeIdx], aW) / DOT (aTrEdges[anEdgeIdx], aTrEdges[anEdgeIdx]);
      Standard_Real aDist = DISTANCE (aPtOnPlane, SelectMgr_Vec3 (aPnts[anEdgeIdx].X() + aCoef * aTrEdges[anEdgeIdx].x(),
                                                                  aPnts[anEdgeIdx].Y() + aCoef * aTrEdges[anEdgeIdx].y(),
                                                                  aPnts[anEdgeIdx].Z() + aCoef * aTrEdges[anEdgeIdx].z()));
      if (aMinDist > aDist)
      {
        aMinDist = aDist;
        aNearestEdgeIdx = anEdgeIdx;
      }
    }
    segmentSegmentDistance (aPnts[aNearestEdgeIdx], aPnts[(aNearestEdgeIdx + 1) % 3], theDepth);
  }

  return Standard_True;
}

// =======================================================================
// function : DistToGeometryCenter
// purpose  : Measures distance between 3d projection of user-picked
//            screen point and given point theCOG
// =======================================================================
Standard_Real SelectMgr_RectangularFrustum::DistToGeometryCenter (const gp_Pnt& theCOG)
{
  const SelectMgr_Vec3& aCOG = SelectMgr_Vec3 (theCOG.X(), theCOG.Y(), theCOG.Z());
  return DISTANCE (aCOG, myNearPickedPnt);
}

// =======================================================================
// function : DetectedPoint
// purpose  : Calculates the point on a view ray that was detected during
//            the run of selection algo by given depth
// =======================================================================
SelectMgr_Vec3 SelectMgr_RectangularFrustum::DetectedPoint (const Standard_Real theDepth) const
{
  return myNearPickedPnt + myViewRayDir * theDepth;
}

// =======================================================================
// function : IsClipped
// purpose  : Checks if the point of sensitive in which selection was
//            detected belongs to the region defined by clipping planes
// =======================================================================
Standard_Boolean SelectMgr_RectangularFrustum::IsClipped (const Graphic3d_SequenceOfHClipPlane& thePlanes,
                                                          const Standard_Real theDepth)
{
  Graphic3d_SequenceOfHClipPlane::Iterator aPlaneIt (thePlanes);
  Standard_Real aMaxDepth = DBL_MAX;
  Standard_Real aMinDepth = -DBL_MAX;
  Standard_Real aPlaneA, aPlaneB, aPlaneC, aPlaneD;
  for ( ; aPlaneIt.More(); aPlaneIt.Next())
  {
    const Handle(Graphic3d_ClipPlane)& aClipPlane = aPlaneIt.Value();
    if (!aClipPlane->IsOn())
      continue;

    gp_Pln aGeomPlane = aClipPlane->ToPlane();

    aGeomPlane.Coefficients (aPlaneA, aPlaneB, aPlaneC, aPlaneD);

    const gp_XYZ& aPlaneDirXYZ = aGeomPlane.Axis().Direction().XYZ();

    Standard_Real aDotProduct = DOTp (myViewRayDir, aPlaneDirXYZ);
    Standard_Real aDistance = - (DOTp (myNearPickedPnt, aPlaneDirXYZ) + aPlaneD);

    // check whether the pick line is parallel to clip plane
    if (Abs (aDotProduct) < Precision::Angular())
    {
      // line lies below the plane and is not clipped, skip
      continue;
    }

    // compute distance to point of pick line intersection with the plane
    Standard_Real aParam = aDistance / aDotProduct;

    // check if ray intersects the plane, in case aIntDist < 0
    // the plane is "behind" the ray
    if (aParam < 0.0)
    {
      continue;
    }

    const SelectMgr_Vec3 anIntersectionPt = myNearPickedPnt + myViewRayDir * aParam;
    const Standard_Real aDistToPln = DISTANCE (anIntersectionPt, myNearPickedPnt);

    // change depth limits for case of opposite and directed planes
    if (aDotProduct < 0.0)
    {
      aMaxDepth = Min (aDistToPln, aMaxDepth);
    }
    else if (aDistToPln > aMinDepth)
    {
      aMinDepth = Max (aDistToPln, aMinDepth);
    }
  }

  return (theDepth <= aMinDepth || theDepth >= aMaxDepth);
}
