// Created by: CKY / Contract Toubro-Larsen
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

//--------------------------------------------------------------------
//--------------------------------------------------------------------

#include <IGESData_IGESEntity.hxx>
#include <IGESGeom_CurveOnSurface.hxx>
#include <IGESGeom_TrimmedSurface.hxx>
#include <Standard_DimensionMismatch.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESGeom_TrimmedSurface,IGESData_IGESEntity)

IGESGeom_TrimmedSurface::IGESGeom_TrimmedSurface ()    {  }


    void IGESGeom_TrimmedSurface::Init
  (const Handle(IGESData_IGESEntity)& aSurface,
   const Standard_Integer aFlag,
   const Handle(IGESGeom_CurveOnSurface)& anOuter,
   const Handle(IGESGeom_HArray1OfCurveOnSurface)& anInner)
{
  if (!anInner.IsNull())
    if (anInner->Lower() != 1)
      throw Standard_DimensionMismatch("IGESGeom_TrimmedSurface : Init");

  theSurface     = aSurface;
  theFlag        = aFlag;
  theOuterCurve  = anOuter;
  theInnerCurves = anInner;
  InitTypeAndForm(144,0);
}

    Handle(IGESData_IGESEntity) IGESGeom_TrimmedSurface::Surface () const
{
  return theSurface;
}

    Standard_Boolean IGESGeom_TrimmedSurface::HasOuterContour () const
{
  return (! theOuterCurve.IsNull());
}

    Handle(IGESGeom_CurveOnSurface) IGESGeom_TrimmedSurface::OuterContour () const
{
  return theOuterCurve;
}

    Standard_Integer IGESGeom_TrimmedSurface::NbInnerContours () const
{
  return (theInnerCurves.IsNull() ? 0 : theInnerCurves->Length());
}

    Standard_Integer IGESGeom_TrimmedSurface::OuterBoundaryType () const
{
  return theFlag;
}

    Handle(IGESGeom_CurveOnSurface) IGESGeom_TrimmedSurface::InnerContour
  (const Standard_Integer anIndex) const
{
  return (theInnerCurves->Value(anIndex));
  // Exception OutOfRange will be raises if anIndex <= 0 or
  //                                        anIndex > NbInnerCounters()
}
