// Created on: 2015-09-21
// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <GeomEvaluator_OffsetSurface.hxx>

#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurfaceUtils.pxx>
#include <Geom_UndefinedValue.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Standard_RangeError.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomEvaluator_OffsetSurface, GeomEvaluator_Surface)

GeomEvaluator_OffsetSurface::GeomEvaluator_OffsetSurface(
  const Handle(Geom_Surface)&           theBase,
  const Standard_Real                   theOffset,
  const Handle(Geom_OsculatingSurface)& theOscSurf)
    : GeomEvaluator_Surface(),
      myBaseSurf(theBase),
      myOffset(theOffset),
      myOscSurf(theOscSurf)
{
  if (!myOscSurf.IsNull())
    return; // osculating surface already exists

  // Create osculating surface for B-spline and Besier surfaces only
  if (myBaseSurf->IsKind(STANDARD_TYPE(Geom_BSplineSurface))
      || myBaseSurf->IsKind(STANDARD_TYPE(Geom_BezierSurface)))
    myOscSurf = new Geom_OsculatingSurface(myBaseSurf, Precision::Confusion());
}

GeomEvaluator_OffsetSurface::GeomEvaluator_OffsetSurface(
  const Handle(GeomAdaptor_Surface)&    theBase,
  const Standard_Real                   theOffset,
  const Handle(Geom_OsculatingSurface)& theOscSurf)
    : GeomEvaluator_Surface(),
      myBaseAdaptor(theBase),
      myOffset(theOffset),
      myOscSurf(theOscSurf)
{
}

//==================================================================================================

void GeomEvaluator_OffsetSurface::D0(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Pnt&             theValue) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateD0(theU,
                                               theV,
                                               myBaseAdaptor,
                                               myOffset,
                                               myOscSurf.get(),
                                               theValue);
  }
  else
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateD0(theU,
                                               theV,
                                               myBaseSurf,
                                               myOffset,
                                               myOscSurf.get(),
                                               theValue);
  }

  if (!isOK)
  {
    throw Geom_UndefinedValue("GeomEvaluator_OffsetSurface::D0(): Unable to calculate normal");
  }
}

//==================================================================================================

void GeomEvaluator_OffsetSurface::D1(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Pnt&             theValue,
                                     gp_Vec&             theD1U,
                                     gp_Vec&             theD1V) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateD1(theU,
                                               theV,
                                               myBaseAdaptor,
                                               myOffset,
                                               myOscSurf.get(),
                                               theValue,
                                               theD1U,
                                               theD1V);
  }
  else
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateD1(theU,
                                               theV,
                                               myBaseSurf,
                                               myOffset,
                                               myOscSurf.get(),
                                               theValue,
                                               theD1U,
                                               theD1V);
  }

  if (!isOK)
  {
    throw Geom_UndefinedValue("GeomEvaluator_OffsetSurface::D1(): Unable to calculate normal");
  }
}

//==================================================================================================

void GeomEvaluator_OffsetSurface::D2(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Pnt&             theValue,
                                     gp_Vec&             theD1U,
                                     gp_Vec&             theD1V,
                                     gp_Vec&             theD2U,
                                     gp_Vec&             theD2V,
                                     gp_Vec&             theD2UV) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateD2(theU,
                                               theV,
                                               myBaseAdaptor,
                                               myOffset,
                                               myOscSurf.get(),
                                               theValue,
                                               theD1U,
                                               theD1V,
                                               theD2U,
                                               theD2V,
                                               theD2UV);
  }
  else
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateD2(theU,
                                               theV,
                                               myBaseSurf,
                                               myOffset,
                                               myOscSurf.get(),
                                               theValue,
                                               theD1U,
                                               theD1V,
                                               theD2U,
                                               theD2V,
                                               theD2UV);
  }

  if (!isOK)
  {
    throw Geom_UndefinedValue("GeomEvaluator_OffsetSurface::D2(): Unable to calculate normal");
  }
}

//==================================================================================================

void GeomEvaluator_OffsetSurface::D3(const Standard_Real theU,
                                     const Standard_Real theV,
                                     gp_Pnt&             theValue,
                                     gp_Vec&             theD1U,
                                     gp_Vec&             theD1V,
                                     gp_Vec&             theD2U,
                                     gp_Vec&             theD2V,
                                     gp_Vec&             theD2UV,
                                     gp_Vec&             theD3U,
                                     gp_Vec&             theD3V,
                                     gp_Vec&             theD3UUV,
                                     gp_Vec&             theD3UVV) const
{
  bool isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateD3(theU,
                                               theV,
                                               myBaseAdaptor,
                                               myOffset,
                                               myOscSurf.get(),
                                               theValue,
                                               theD1U,
                                               theD1V,
                                               theD2U,
                                               theD2V,
                                               theD2UV,
                                               theD3U,
                                               theD3V,
                                               theD3UUV,
                                               theD3UVV);
  }
  else
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateD3(theU,
                                               theV,
                                               myBaseSurf,
                                               myOffset,
                                               myOscSurf.get(),
                                               theValue,
                                               theD1U,
                                               theD1V,
                                               theD2U,
                                               theD2V,
                                               theD2UV,
                                               theD3U,
                                               theD3V,
                                               theD3UUV,
                                               theD3UVV);
  }

  if (!isOK)
  {
    throw Geom_UndefinedValue("GeomEvaluator_OffsetSurface::D3(): Unable to calculate normal");
  }
}

//==================================================================================================

gp_Vec GeomEvaluator_OffsetSurface::DN(const Standard_Real    theU,
                                       const Standard_Real    theV,
                                       const Standard_Integer theDerU,
                                       const Standard_Integer theDerV) const
{
  Standard_RangeError_Raise_if(theDerU < 0, "GeomEvaluator_OffsetSurface::DN(): theDerU < 0");
  Standard_RangeError_Raise_if(theDerV < 0, "GeomEvaluator_OffsetSurface::DN(): theDerV < 0");
  Standard_RangeError_Raise_if(theDerU + theDerV < 1,
                               "GeomEvaluator_OffsetSurface::DN(): theDerU + theDerV < 1");

  gp_Vec aResult;
  bool   isOK = false;
  if (!myBaseAdaptor.IsNull())
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateDN(theU,
                                               theV,
                                               theDerU,
                                               theDerV,
                                               myBaseAdaptor,
                                               myOffset,
                                               myOscSurf.get(),
                                               aResult);
  }
  else
  {
    isOK = Geom_OffsetSurfaceUtils::EvaluateDN(theU,
                                               theV,
                                               theDerU,
                                               theDerV,
                                               myBaseSurf,
                                               myOffset,
                                               myOscSurf.get(),
                                               aResult);
  }

  if (!isOK)
  {
    throw Geom_UndefinedValue("GeomEvaluator_OffsetSurface::DN(): Unable to calculate normal");
  }

  return aResult;
}

//==================================================================================================

Handle(GeomEvaluator_Surface) GeomEvaluator_OffsetSurface::ShallowCopy() const
{
  Handle(GeomEvaluator_OffsetSurface) aCopy;
  if (!myBaseAdaptor.IsNull())
  {
    aCopy = new GeomEvaluator_OffsetSurface(
      Handle(GeomAdaptor_Surface)::DownCast(myBaseAdaptor->ShallowCopy()),
      myOffset,
      myOscSurf);
  }
  else
  {
    aCopy = new GeomEvaluator_OffsetSurface(myBaseSurf, myOffset, myOscSurf);
  }

  return aCopy;
}
