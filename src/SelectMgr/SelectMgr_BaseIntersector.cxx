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
