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

#include <SelectMgr_BaseFrustum.hxx>

#include <Message.hxx>
#include <SelectMgr_FrustumBuilder.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(SelectMgr_BaseFrustum, SelectMgr_BaseIntersector)

//=======================================================================
// function : SelectMgr_BaseFrustum
// purpose  :
//=======================================================================
SelectMgr_BaseFrustum::SelectMgr_BaseFrustum()
: myPixelTolerance (2)
{
  myBuilder = new SelectMgr_FrustumBuilder();
}

//=======================================================================
// function : SetCamera
// purpose  :
//=======================================================================
void SelectMgr_BaseFrustum::SetCamera (const Handle(Graphic3d_Camera)& theCamera)
{
  SelectMgr_BaseIntersector::SetCamera (theCamera);
  if (!myBuilder.IsNull())
  {
    myBuilder->SetCamera (theCamera);
    myBuilder->InvalidateViewport();
  }
}

//=======================================================================
// function : SetViewport
// purpose  : Passes viewport parameters to builder
//=======================================================================
void SelectMgr_BaseFrustum::SetViewport (const Standard_Real theX,
                                         const Standard_Real theY,
                                         const Standard_Real theWidth,
                                         const Standard_Real theHeight)
{
  myBuilder->SetViewport (theX, theY, theWidth, theHeight);
}

//=======================================================================
// function : SetPixelTolerance
// purpose  :
//=======================================================================
void SelectMgr_BaseFrustum::SetPixelTolerance (const Standard_Integer theTol)
{
  myPixelTolerance = theTol;
}

//=======================================================================
// function : SetWindowSize
// purpose  :
//=======================================================================
void SelectMgr_BaseFrustum::SetWindowSize (const Standard_Integer theWidth, const Standard_Integer theHeight)
{
  myBuilder->SetWindowSize (theWidth, theHeight);
}

//=======================================================================
// function : WindowSize
// purpose  :
//=======================================================================
void SelectMgr_BaseFrustum::WindowSize (Standard_Integer& theWidth,
                                        Standard_Integer& theHeight) const
{
  myBuilder->WindowSize (theWidth, theHeight);
}

//=======================================================================
// function : SetBuilder
// purpose  :
//=======================================================================
void SelectMgr_BaseFrustum::SetBuilder (const Handle(SelectMgr_FrustumBuilder)& theBuilder)
{
  myBuilder.Nullify();
  myBuilder = theBuilder;
  if (!myBuilder.IsNull())
  {
    myCamera = myBuilder->Camera();
  }
}

//=======================================================================
// function : IsBoundariesIntersectSphere
// purpose  :
//=======================================================================
Standard_Boolean SelectMgr_BaseFrustum::IsBoundaryIntersectSphere (const gp_Pnt& theCenter,
                                                                   const Standard_Real theRadius,
                                                                   const gp_Dir& thePlaneNormal,
                                                                   const TColgp_Array1OfPnt& theBoundaries,
                                                                   Standard_Boolean& theBoundaryInside) const
{
  for (Standard_Integer anIdx = theBoundaries.Lower(); anIdx < theBoundaries.Upper(); ++anIdx)
  {
    const Standard_Integer aNextIdx = ((anIdx + 1) == theBoundaries.Upper()) ? theBoundaries.Lower() : (anIdx + 1);
    const gp_Pnt aPnt1 = theBoundaries.Value (anIdx);
    const gp_Pnt aPnt2 = theBoundaries.Value (aNextIdx);
    if (aPnt1.Distance (aPnt2) < Precision::Confusion())
    {
      continue;
    }

    // Projections of the points on the plane
    const gp_Pnt aPntProj1 = aPnt1.XYZ() - thePlaneNormal.XYZ() * aPnt1.XYZ().Dot (thePlaneNormal.XYZ());
    const gp_Pnt aPntProj2 = aPnt2.XYZ() - thePlaneNormal.XYZ() * aPnt2.XYZ().Dot (thePlaneNormal.XYZ());
    if (aPntProj1.Distance (theCenter) < theRadius || aPntProj2.Distance (theCenter) < theRadius) // polygon intersects the sphere
    {
      theBoundaryInside = Standard_True;
      return Standard_True;
    }

    gp_Dir aRayDir (gp_Vec (aPntProj1, aPntProj2));
    Standard_Real aTimeEnter = 0.0, aTimeLeave = 0.0;
    if (RaySphereIntersection (theCenter, theRadius, aPntProj1, aRayDir, aTimeEnter, aTimeLeave))
    {
      if ((aTimeEnter > 0 && aTimeEnter < aPntProj1.Distance (aPntProj2))
       || (aTimeLeave > 0 && aTimeLeave < aPntProj1.Distance (aPntProj2)))
      {
        return Standard_True; // polygon crosses  the sphere
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void SelectMgr_BaseFrustum::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_CLASS_BEGIN (theOStream, SelectMgr_BaseFrustum)
  OCCT_DUMP_BASE_CLASS (theOStream, theDepth, SelectMgr_BaseIntersector)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myPixelTolerance)
  OCCT_DUMP_FIELD_VALUE_POINTER (theOStream, myBuilder)
}
