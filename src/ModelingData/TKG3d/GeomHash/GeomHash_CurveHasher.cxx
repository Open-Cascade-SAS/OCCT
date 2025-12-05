// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomHash_CurveHasher.hxx>

#include <Standard_CStringHasher.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Geom_OffsetCurve.hxx>

#include <GeomHash_LineHasher.pxx>
#include <GeomHash_CircleHasher.pxx>
#include <GeomHash_EllipseHasher.pxx>
#include <GeomHash_HyperbolaHasher.pxx>
#include <GeomHash_ParabolaHasher.pxx>
#include <GeomHash_BezierCurveHasher.pxx>
#include <GeomHash_BSplineCurveHasher.pxx>
#include <GeomHash_TrimmedCurveHasher.pxx>
#include <GeomHash_OffsetCurveHasher.pxx>

//=================================================================================================

std::size_t GeomHash_CurveHasher::operator()(const Handle(Geom_Curve)& theCurve) const noexcept
{
  if (theCurve.IsNull())
  {
    return 0;
  }

  // Dispatch based on actual curve type
  if (Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(theCurve))
  {
    return GeomHash_LineHasher{}(aLine);
  }
  if (Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast(theCurve))
  {
    return GeomHash_CircleHasher{}(aCircle);
  }
  if (Handle(Geom_Ellipse) anEllipse = Handle(Geom_Ellipse)::DownCast(theCurve))
  {
    return GeomHash_EllipseHasher{}(anEllipse);
  }
  if (Handle(Geom_Hyperbola) aHyperbola = Handle(Geom_Hyperbola)::DownCast(theCurve))
  {
    return GeomHash_HyperbolaHasher{}(aHyperbola);
  }
  if (Handle(Geom_Parabola) aParabola = Handle(Geom_Parabola)::DownCast(theCurve))
  {
    return GeomHash_ParabolaHasher{}(aParabola);
  }
  if (Handle(Geom_BezierCurve) aBezier = Handle(Geom_BezierCurve)::DownCast(theCurve))
  {
    return GeomHash_BezierCurveHasher{}(aBezier);
  }
  if (Handle(Geom_BSplineCurve) aBSpline = Handle(Geom_BSplineCurve)::DownCast(theCurve))
  {
    return GeomHash_BSplineCurveHasher{}(aBSpline);
  }
  if (Handle(Geom_TrimmedCurve) aTrimmed = Handle(Geom_TrimmedCurve)::DownCast(theCurve))
  {
    return GeomHash_TrimmedCurveHasher{}(aTrimmed);
  }
  if (Handle(Geom_OffsetCurve) anOffset = Handle(Geom_OffsetCurve)::DownCast(theCurve))
  {
    return GeomHash_OffsetCurveHasher{}(anOffset);
  }

  // Unknown curve type - hash the type name
  return Standard_CStringHasher{}(theCurve->DynamicType()->Name());
}

//=================================================================================================

bool GeomHash_CurveHasher::operator()(const Handle(Geom_Curve)& theCurve1,
                                      const Handle(Geom_Curve)& theCurve2) const noexcept
{
  if (theCurve1.IsNull() || theCurve2.IsNull())
  {
    return theCurve1.IsNull() && theCurve2.IsNull();
  }

  if (theCurve1 == theCurve2)
  {
    return true;
  }

  // Must be same type
  if (theCurve1->DynamicType() != theCurve2->DynamicType())
  {
    return false;
  }

  // Dispatch based on actual curve type
  if (Handle(Geom_Line) aLine1 = Handle(Geom_Line)::DownCast(theCurve1))
  {
    return GeomHash_LineHasher{}(aLine1, Handle(Geom_Line)::DownCast(theCurve2));
  }
  if (Handle(Geom_Circle) aCircle1 = Handle(Geom_Circle)::DownCast(theCurve1))
  {
    return GeomHash_CircleHasher{}(aCircle1, Handle(Geom_Circle)::DownCast(theCurve2));
  }
  if (Handle(Geom_Ellipse) anEllipse1 = Handle(Geom_Ellipse)::DownCast(theCurve1))
  {
    return GeomHash_EllipseHasher{}(anEllipse1, Handle(Geom_Ellipse)::DownCast(theCurve2));
  }
  if (Handle(Geom_Hyperbola) aHyp1 = Handle(Geom_Hyperbola)::DownCast(theCurve1))
  {
    return GeomHash_HyperbolaHasher{}(aHyp1, Handle(Geom_Hyperbola)::DownCast(theCurve2));
  }
  if (Handle(Geom_Parabola) aPar1 = Handle(Geom_Parabola)::DownCast(theCurve1))
  {
    return GeomHash_ParabolaHasher{}(aPar1, Handle(Geom_Parabola)::DownCast(theCurve2));
  }
  if (Handle(Geom_BezierCurve) aBez1 = Handle(Geom_BezierCurve)::DownCast(theCurve1))
  {
    return GeomHash_BezierCurveHasher{}(aBez1, Handle(Geom_BezierCurve)::DownCast(theCurve2));
  }
  if (Handle(Geom_BSplineCurve) aBSpl1 = Handle(Geom_BSplineCurve)::DownCast(theCurve1))
  {
    return GeomHash_BSplineCurveHasher{}(aBSpl1, Handle(Geom_BSplineCurve)::DownCast(theCurve2));
  }
  if (Handle(Geom_TrimmedCurve) aTrim1 = Handle(Geom_TrimmedCurve)::DownCast(theCurve1))
  {
    return GeomHash_TrimmedCurveHasher{}(aTrim1, Handle(Geom_TrimmedCurve)::DownCast(theCurve2));
  }
  if (Handle(Geom_OffsetCurve) aOff1 = Handle(Geom_OffsetCurve)::DownCast(theCurve1))
  {
    return GeomHash_OffsetCurveHasher{}(aOff1, Handle(Geom_OffsetCurve)::DownCast(theCurve2));
  }

  // Unknown curve type - compare by pointer
  return theCurve1.get() == theCurve2.get();
}
