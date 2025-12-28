// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <SelectMgr_BaseIntersector.hxx>

#include <Graphic3d_Camera.hxx>
#include <gp_Ax3.hxx>

#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_BaseIntersector, Standard_Transient)

//=================================================================================================

SelectMgr_BaseIntersector::SelectMgr_BaseIntersector()
    : mySelectionType(SelectMgr_SelectionType_Unknown)
{
  //
}

//=================================================================================================

SelectMgr_BaseIntersector::~SelectMgr_BaseIntersector()
{
  //
}

//=================================================================================================

void SelectMgr_BaseIntersector::SetCamera(const occ::handle<Graphic3d_Camera>& theCamera)
{
  myCamera = theCamera;
}

//=================================================================================================

void SelectMgr_BaseIntersector::SetPixelTolerance(const int) {}

//=================================================================================================

void SelectMgr_BaseIntersector::WindowSize(int&, int&) const {}

//=================================================================================================

void SelectMgr_BaseIntersector::SetWindowSize(const int, const int) {}

//=================================================================================================

void SelectMgr_BaseIntersector::SetViewport(const double, const double, const double, const double)
{
}

//=================================================================================================

const gp_Pnt& SelectMgr_BaseIntersector::GetNearPnt() const
{
  static const gp_Pnt anEmptyPnt;
  return anEmptyPnt;
}

//=================================================================================================

const gp_Pnt& SelectMgr_BaseIntersector::GetFarPnt() const
{
  static const gp_Pnt anEmptyPnt(RealLast(), RealLast(), RealLast());
  return anEmptyPnt;
}

//=================================================================================================

const gp_Dir& SelectMgr_BaseIntersector::GetViewRayDirection() const
{
  static const gp_Dir anEmptyDir;
  return anEmptyDir;
}

//=================================================================================================

const gp_Pnt2d& SelectMgr_BaseIntersector::GetMousePosition() const
{
  static const gp_Pnt2d aPnt(RealLast(), RealLast());
  return aPnt;
}

//=================================================================================================

bool SelectMgr_BaseIntersector::RaySphereIntersection(const gp_Pnt& theCenter,
                                                      const double  theRadius,
                                                      const gp_Pnt& theLoc,
                                                      const gp_Dir& theRayDir,
                                                      double&       theTimeEnter,
                                                      double&       theTimeLeave) const
{
  // to find the intersection of the ray (theLoc, theRayDir) and sphere with theCenter(x0, y0, z0)
  // and theRadius(R), you need to solve the equation (x' - x0)^2 + (y' - y0)^2 + (z' - z0)^2 = R^2,
  // where P(x',y',z') = theLoc(x,y,z) + theRayDir(vx,vy,vz) * T at the end of solving, you receive
  // a square equation with respect to T T^2 * (vx^2 + vy^2 + vz^2) + 2 * T * (vx*(x - x0) + vy*(y -
  // y0) + vz*(z - z0)) + ((x-x0)^2 + (y-y0)^2 + (z-z0)^2 -R^2) = 0 (= A*T^2 + K*T + C) and find T
  // by discriminant D = K^2 - A*C
  const double anA = theRayDir.Dot(theRayDir);
  const double aK  = theRayDir.X() * (theLoc.X() - theCenter.X())
                    + theRayDir.Y() * (theLoc.Y() - theCenter.Y())
                    + theRayDir.Z() * (theLoc.Z() - theCenter.Z());
  const double aC = theLoc.Distance(theCenter) * theLoc.Distance(theCenter) - theRadius * theRadius;
  const double aDiscr = aK * aK - anA * aC;
  if (aDiscr < 0)
  {
    return false;
  }

  const double aTime1 = (-aK - std::sqrt(aDiscr)) / anA;
  const double aTime2 = (-aK + std::sqrt(aDiscr)) / anA;
  if (std::abs(aTime1) < std::abs(aTime2))
  {
    theTimeEnter = aTime1;
    theTimeLeave = aTime2;
  }
  else
  {
    theTimeEnter = aTime2;
    theTimeLeave = aTime1;
  }
  return true;
}

//=================================================================================================

bool SelectMgr_BaseIntersector::RayCylinderIntersection(const double  theBottomRadius,
                                                        const double  theTopRadius,
                                                        const double  theHeight,
                                                        const gp_Pnt& theLoc,
                                                        const gp_Dir& theRayDir,
                                                        const bool    theIsHollow,
                                                        double&       theTimeEnter,
                                                        double&       theTimeLeave) const
{
  int    aNbIntersections   = 0;
  double anIntersections[4] = {RealLast(), RealLast(), RealLast(), RealLast()};
  // Check intersections with end faces
  // point of intersection theRayDir and z = 0
  if (!theIsHollow && theRayDir.Z() != 0)
  {
    const double aTime1 = (0 - theLoc.Z()) / theRayDir.Z();
    const double aX1    = theLoc.X() + theRayDir.X() * aTime1;
    const double anY1   = theLoc.Y() + theRayDir.Y() * aTime1;
    if (aX1 * aX1 + anY1 * anY1 <= theBottomRadius * theBottomRadius)
    {
      anIntersections[aNbIntersections++] = aTime1;
    }
    // point of intersection theRayDir and z = theHeight
    const double aTime2 = (theHeight - theLoc.Z()) / theRayDir.Z();
    const double aX2    = theLoc.X() + theRayDir.X() * aTime2;
    const double anY2   = theLoc.Y() + theRayDir.Y() * aTime2;
    if (aX2 * aX2 + anY2 * anY2 <= theTopRadius * theTopRadius)
    {
      anIntersections[aNbIntersections++] = aTime2;
    }
  }
  // ray intersection with cone / truncated cone
  if (theTopRadius != theBottomRadius)
  {
    const double aTriangleHeight = std::min(theBottomRadius, theTopRadius) * theHeight
                                   / (std::abs(theBottomRadius - theTopRadius));
    gp_Ax3 aSystem;
    if (theBottomRadius > theTopRadius)
    {
      aSystem.SetLocation(gp_Pnt(0, 0, theHeight + aTriangleHeight));
      aSystem.SetDirection(-gp::DZ());
    }
    else
    {
      aSystem.SetLocation(gp_Pnt(0, 0, -aTriangleHeight));
      aSystem.SetDirection(gp::DZ());
    }
    gp_Trsf aTrsfCone;
    aTrsfCone.SetTransformation(gp_Ax3(), aSystem);
    const gp_Pnt aPnt(theLoc.Transformed(aTrsfCone));
    const gp_Dir aDir(theRayDir.Transformed(aTrsfCone));
    const double aMaxRad     = std::max(theBottomRadius, theTopRadius);
    const double aConeHeight = theHeight + aTriangleHeight;

    // solving quadratic equation anA * T^2 + 2 * aK * T + aC = 0
    const double anA = aDir.X() * aDir.X() / (aMaxRad * aMaxRad)
                       + aDir.Y() * aDir.Y() / (aMaxRad * aMaxRad)
                       - aDir.Z() * aDir.Z() / (aConeHeight * aConeHeight);
    const double aK = aDir.X() * aPnt.X() / (aMaxRad * aMaxRad)
                      + aDir.Y() * aPnt.Y() / (aMaxRad * aMaxRad)
                      - aDir.Z() * aPnt.Z() / (aConeHeight * aConeHeight);
    const double aC = aPnt.X() * aPnt.X() / (aMaxRad * aMaxRad)
                      + aPnt.Y() * aPnt.Y() / (aMaxRad * aMaxRad)
                      - aPnt.Z() * aPnt.Z() / (aConeHeight * aConeHeight);
    double aDiscr = aK * aK - anA * aC;
    if (aDiscr > 0)
    {
      const double aTimeEnterCone = (-aK - std::sqrt(aDiscr)) / anA;
      const double aTimeLeaveCone = (-aK + std::sqrt(aDiscr)) / anA;
      const double aZFromRoot1    = aPnt.Z() + aTimeEnterCone * aDir.Z();
      const double aZFromRoot2    = aPnt.Z() + aTimeLeaveCone * aDir.Z();

      if (aZFromRoot1 > aTriangleHeight && aZFromRoot1 < aConeHeight)
      {
        anIntersections[aNbIntersections++] = aTimeEnterCone;
      }
      if (aZFromRoot2 > aTriangleHeight && aZFromRoot2 < aConeHeight)
      {
        anIntersections[aNbIntersections++] = aTimeLeaveCone;
      }
    }
  }
  else // ray intersection with cylinder
  {
    const gp_Pnt2d aLoc2d(theLoc.X(), theLoc.Y());
    const gp_Vec2d aRayDir2d(theRayDir.X(), theRayDir.Y());

    // solving quadratic equation anA * T^2 + 2 * aK * T + aC = 0
    const double anA    = aRayDir2d.Dot(aRayDir2d);
    const double aK     = aLoc2d.XY().Dot(aRayDir2d.XY());
    const double aC     = aLoc2d.XY().Dot(aLoc2d.XY()) - theTopRadius * theTopRadius;
    const double aDiscr = aK * aK - anA * aC;
    if (aDiscr > 0)
    {
      const double aRoot1      = (-aK + std::sqrt(aDiscr)) / anA;
      const double aRoot2      = (-aK - std::sqrt(aDiscr)) / anA;
      const double aZFromRoot1 = theLoc.Z() + aRoot1 * theRayDir.Z();
      const double aZFromRoot2 = theLoc.Z() + aRoot2 * theRayDir.Z();
      if (aZFromRoot1 > 0 && aZFromRoot1 < theHeight)
      {
        anIntersections[aNbIntersections++] = aRoot1;
      }
      if (aZFromRoot2 > 0 && aZFromRoot2 < theHeight)
      {
        anIntersections[aNbIntersections++] = aRoot2;
      }
    }
  }
  if (aNbIntersections == 0)
  {
    return false;
  }

  std::sort(anIntersections, anIntersections + aNbIntersections);
  theTimeEnter = anIntersections[0];
  if (aNbIntersections > 1)
  {
    theTimeLeave = anIntersections[1];
  }
  return true;
}

//=================================================================================================

bool SelectMgr_BaseIntersector::RayCircleIntersection(const double  theRadius,
                                                      const gp_Pnt& theLoc,
                                                      const gp_Dir& theRayDir,
                                                      const bool    theIsFilled,
                                                      double&       theTime) const
{
  if (theRayDir.Z() != 0)
  {
    const double aTime = (0 - theLoc.Z()) / theRayDir.Z();
    const double aX1   = theLoc.X() + theRayDir.X() * aTime;
    const double anY1  = theLoc.Y() + theRayDir.Y() * aTime;

    const double aK = aX1 * aX1 + anY1 * anY1;
    if ((theIsFilled && aK <= theRadius * theRadius)
        || (!theIsFilled && std::abs(sqrt(aK) - theRadius) <= Precision::Confusion()))
    {
      theTime = aTime;
      return true;
    }
  }
  return false;
}

//=================================================================================================

double SelectMgr_BaseIntersector::DistToGeometryCenter(const gp_Pnt&) const
{
  return RealLast();
}

//=================================================================================================

gp_Pnt SelectMgr_BaseIntersector::DetectedPoint(const double) const
{
  return gp_Pnt(RealLast(), RealLast(), RealLast());
}

//=================================================================================================

void SelectMgr_BaseIntersector::DumpJson(Standard_OStream& theOStream, int) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySelectionType)
  OCCT_DUMP_FIELD_VALUE_POINTER(theOStream, myCamera)
}
