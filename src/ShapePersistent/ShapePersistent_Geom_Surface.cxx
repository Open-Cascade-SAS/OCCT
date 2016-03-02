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

#include <ShapePersistent_Geom_Surface.hxx>

#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_OffsetSurface.hxx>

#include <gp_Ax1.hxx>


Handle(Geom_Surface) ShapePersistent_Geom_Surface::pLinearExtrusion::Import()
  const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom_SurfaceOfLinearExtrusion
    (myBasisCurve->Import(), myDirection);
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pRevolution::Import() const
{
  if (myBasisCurve.IsNull())
    return NULL;

  return new Geom_SurfaceOfRevolution
    (myBasisCurve->Import(), gp_Ax1 (myLocation, myDirection));
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pBezier::Import() const
{
  if (myPoles.IsNull())
    return NULL;

  if (myURational || myVRational)
  {
    if (myWeights.IsNull())
      return NULL;
    return new Geom_BezierSurface (*myPoles->Array(), *myWeights->Array());
  }
  else
    return new Geom_BezierSurface (*myPoles->Array());
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pBSpline::Import() const
{
  if (myPoles.IsNull() || myUKnots.IsNull() || myVKnots.IsNull()
   || myUMultiplicities.IsNull() || myVMultiplicities.IsNull())
    return NULL;

  if (myURational || myVRational)
  {
    if (myWeights.IsNull())
      return NULL;

    return new Geom_BSplineSurface (*myPoles->Array(),
                                    *myWeights->Array(),
                                    *myUKnots->Array(),
                                    *myVKnots->Array(),
                                    *myUMultiplicities->Array(),
                                    *myVMultiplicities->Array(),
                                    myUSpineDegree,
                                    myVSpineDegree,
                                    myUPeriodic,
                                    myVPeriodic);
  }
  else
    return new Geom_BSplineSurface (*myPoles->Array(),
                                    *myUKnots->Array(),
                                    *myVKnots->Array(),
                                    *myUMultiplicities->Array(),
                                    *myVMultiplicities->Array(),
                                    myUSpineDegree,
                                    myVSpineDegree,
                                    myUPeriodic,
                                    myVPeriodic);
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pRectangularTrimmed::Import()
  const
{
  if (myBasisSurface.IsNull())
    return NULL;

  return new Geom_RectangularTrimmedSurface
    (myBasisSurface->Import(), myFirstU, myLastU, myFirstV, myLastV);
}

Handle(Geom_Surface) ShapePersistent_Geom_Surface::pOffset::Import() const
{
  if (myBasisSurface.IsNull())
    return NULL;

  return new Geom_OffsetSurface (myBasisSurface->Import(), myOffsetValue);
}
