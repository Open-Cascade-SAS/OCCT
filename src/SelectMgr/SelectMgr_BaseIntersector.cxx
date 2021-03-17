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

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_BaseIntersector, Standard_Transient)

//=======================================================================
// function : SelectMgr_BaseIntersector
// purpose  :
//=======================================================================
SelectMgr_BaseIntersector::SelectMgr_BaseIntersector()
: mySelectionType (SelectMgr_SelectionType_Unknown)
{
  //
}

//=======================================================================
// function : ~SelectMgr_BaseIntersector
// purpose  :
//=======================================================================
SelectMgr_BaseIntersector::~SelectMgr_BaseIntersector()
{
  //
}

//=======================================================================
// function : SetCamera
// purpose  :
//=======================================================================
void SelectMgr_BaseIntersector::SetCamera (const Handle(Graphic3d_Camera)& theCamera)
{
  myCamera = theCamera;
}

//=======================================================================
// function : SetPixelTolerance
// purpose  :
//=======================================================================
void SelectMgr_BaseIntersector::SetPixelTolerance (const Standard_Integer)
{
}

//=======================================================================
// function : WindowSize
// purpose  :
//=======================================================================
void SelectMgr_BaseIntersector::WindowSize (Standard_Integer&,
                                            Standard_Integer&) const
{
}

//=======================================================================
// function : SetWindowSize
// purpose  :
//=======================================================================
void SelectMgr_BaseIntersector::SetWindowSize (const Standard_Integer,
                                               const Standard_Integer)
{
}

//=======================================================================
// function : SetViewport
// purpose  :
//=======================================================================
void SelectMgr_BaseIntersector::SetViewport (const Standard_Real,
                                             const Standard_Real,
                                             const Standard_Real,
                                             const Standard_Real)
{
}

//=======================================================================
// function : GetNearPnt
// purpose  :
//=======================================================================
const gp_Pnt& SelectMgr_BaseIntersector::GetNearPnt() const
{
  static const gp_Pnt anEmptyPnt;
  return anEmptyPnt;
}

//=======================================================================
// function : GetFarPnt
// purpose  :
//=======================================================================
const gp_Pnt& SelectMgr_BaseIntersector::GetFarPnt() const
{
  static const gp_Pnt anEmptyPnt(RealLast(), RealLast(), RealLast());
  return anEmptyPnt;
}

//=======================================================================
// function : GetViewRayDirection
// purpose  :
//=======================================================================
const gp_Dir& SelectMgr_BaseIntersector::GetViewRayDirection() const
{
  static const gp_Dir anEmptyDir;
  return anEmptyDir;
}

//=======================================================================
// function : GetMousePosition
// purpose  :
//=======================================================================
const gp_Pnt2d& SelectMgr_BaseIntersector::GetMousePosition() const
{
  static const gp_Pnt2d aPnt(RealLast(), RealLast());
  return aPnt;
}

//=======================================================================
// function : RaySphereIntersection
// purpose  :
//=======================================================================
Standard_Boolean SelectMgr_BaseIntersector::RaySphereIntersection (const gp_Pnt& theCenter,
                                                                   const Standard_Real theRadius,
                                                                   const gp_Pnt& theLoc,
                                                                   const gp_Dir& theRayDir,
                                                                   Standard_Real& theTimeEnter,
                                                                   Standard_Real& theTimeLeave) const
{
  // to find the intersection of the ray (theLoc, theRayDir) and sphere with theCenter(x0, y0, z0) and theRadius(R), you need to solve the equation
  // (x' - x0)^2 + (y' - y0)^2 + (z' - z0)^2 = R^2, where P(x',y',z') = theLoc(x,y,z) + theRayDir(vx,vy,vz) * T
  // at the end of solving, you receive a square equation with respect to T
  // T^2 * (vx^2 + vy^2 + vz^2) + 2 * T * (vx*(x - x0) + vy*(y - y0) + vz*(z - z0)) + ((x-x0)^2 + (y-y0)^2 + (z-z0)^2 -R^2) = 0 (= A*T^2 + K*T + C)
  // and find T by discriminant D = K^2 - A*C
  const Standard_Real anA = theRayDir.Dot (theRayDir);
  const Standard_Real aK = theRayDir.X() * (theLoc.X() - theCenter.X())
                         + theRayDir.Y() * (theLoc.Y() - theCenter.Y())
                         + theRayDir.Z() * (theLoc.Z() - theCenter.Z());
  const Standard_Real aC = theLoc.Distance (theCenter) * theLoc.Distance (theCenter) - theRadius * theRadius;
  const Standard_Real aDiscr = aK * aK - anA * aC;
  if (aDiscr < 0)
  {
    return Standard_False;
  }

  const Standard_Real aTime1 = (-aK - Sqrt (aDiscr)) / anA;
  const Standard_Real aTime2 = (-aK + Sqrt (aDiscr)) / anA;
  if (Abs (aTime1) < Abs (aTime2))
  {
    theTimeEnter = aTime1;
    theTimeLeave = aTime2;
  }
  else
  {
    theTimeEnter = aTime2;
    theTimeLeave = aTime1;
  }
  return Standard_True;
}

//=======================================================================
// function : DistToGeometryCenter
// purpose  :
//=======================================================================
Standard_Real SelectMgr_BaseIntersector::DistToGeometryCenter (const gp_Pnt&) const
{
  return RealLast();
}

//=======================================================================
// function : DetectedPoint
// purpose  :
//=======================================================================
gp_Pnt SelectMgr_BaseIntersector::DetectedPoint (const Standard_Real) const
{
  return gp_Pnt(RealLast(), RealLast(), RealLast());
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void SelectMgr_BaseIntersector::DumpJson (Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, mySelectionType)
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myCamera)
}
