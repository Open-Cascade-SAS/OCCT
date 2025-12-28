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

#include <Geom2dHash_CurveHasher.hxx>

#include <Standard_CStringHasher.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_OffsetCurve.hxx>

#include <Geom2dHash_LineHasher.pxx>
#include <Geom2dHash_CircleHasher.pxx>
#include <Geom2dHash_EllipseHasher.pxx>
#include <Geom2dHash_HyperbolaHasher.pxx>
#include <Geom2dHash_ParabolaHasher.pxx>
#include <Geom2dHash_BezierCurveHasher.pxx>
#include <Geom2dHash_BSplineCurveHasher.pxx>
#include <Geom2dHash_TrimmedCurveHasher.pxx>
#include <Geom2dHash_OffsetCurveHasher.pxx>

//=================================================================================================

std::size_t Geom2dHash_CurveHasher::operator()(
  const occ::handle<Geom2d_Curve>& theCurve) const noexcept
{
  if (theCurve.IsNull())
  {
    return 0;
  }

  // Dispatch based on actual curve type
  if (occ::handle<Geom2d_Line> aLine = occ::down_cast<Geom2d_Line>(theCurve))
  {
    return Geom2dHash_LineHasher{}(aLine);
  }
  if (occ::handle<Geom2d_Circle> aCircle = occ::down_cast<Geom2d_Circle>(theCurve))
  {
    return Geom2dHash_CircleHasher{}(aCircle);
  }
  if (occ::handle<Geom2d_Ellipse> anEllipse = occ::down_cast<Geom2d_Ellipse>(theCurve))
  {
    return Geom2dHash_EllipseHasher{}(anEllipse);
  }
  if (occ::handle<Geom2d_Hyperbola> aHyperbola = occ::down_cast<Geom2d_Hyperbola>(theCurve))
  {
    return Geom2dHash_HyperbolaHasher{}(aHyperbola);
  }
  if (occ::handle<Geom2d_Parabola> aParabola = occ::down_cast<Geom2d_Parabola>(theCurve))
  {
    return Geom2dHash_ParabolaHasher{}(aParabola);
  }
  if (occ::handle<Geom2d_BezierCurve> aBezier = occ::down_cast<Geom2d_BezierCurve>(theCurve))
  {
    return Geom2dHash_BezierCurveHasher{}(aBezier);
  }
  if (occ::handle<Geom2d_BSplineCurve> aBSpline = occ::down_cast<Geom2d_BSplineCurve>(theCurve))
  {
    return Geom2dHash_BSplineCurveHasher{}(aBSpline);
  }
  if (occ::handle<Geom2d_TrimmedCurve> aTrimmed = occ::down_cast<Geom2d_TrimmedCurve>(theCurve))
  {
    return Geom2dHash_TrimmedCurveHasher{}(aTrimmed);
  }
  if (occ::handle<Geom2d_OffsetCurve> anOffset = occ::down_cast<Geom2d_OffsetCurve>(theCurve))
  {
    return Geom2dHash_OffsetCurveHasher{}(anOffset);
  }

  // Unknown curve type - hash the type name
  return Standard_CStringHasher{}(theCurve->DynamicType()->Name());
}

//=================================================================================================

bool Geom2dHash_CurveHasher::operator()(const occ::handle<Geom2d_Curve>& theCurve1,
                                        const occ::handle<Geom2d_Curve>& theCurve2) const noexcept
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
  if (occ::handle<Geom2d_Line> aLine1 = occ::down_cast<Geom2d_Line>(theCurve1))
  {
    return Geom2dHash_LineHasher{}(aLine1, occ::down_cast<Geom2d_Line>(theCurve2));
  }
  if (occ::handle<Geom2d_Circle> aCircle1 = occ::down_cast<Geom2d_Circle>(theCurve1))
  {
    return Geom2dHash_CircleHasher{}(aCircle1, occ::down_cast<Geom2d_Circle>(theCurve2));
  }
  if (occ::handle<Geom2d_Ellipse> anEllipse1 = occ::down_cast<Geom2d_Ellipse>(theCurve1))
  {
    return Geom2dHash_EllipseHasher{}(anEllipse1, occ::down_cast<Geom2d_Ellipse>(theCurve2));
  }
  if (occ::handle<Geom2d_Hyperbola> aHyp1 = occ::down_cast<Geom2d_Hyperbola>(theCurve1))
  {
    return Geom2dHash_HyperbolaHasher{}(aHyp1, occ::down_cast<Geom2d_Hyperbola>(theCurve2));
  }
  if (occ::handle<Geom2d_Parabola> aPar1 = occ::down_cast<Geom2d_Parabola>(theCurve1))
  {
    return Geom2dHash_ParabolaHasher{}(aPar1, occ::down_cast<Geom2d_Parabola>(theCurve2));
  }
  if (occ::handle<Geom2d_BezierCurve> aBez1 = occ::down_cast<Geom2d_BezierCurve>(theCurve1))
  {
    return Geom2dHash_BezierCurveHasher{}(aBez1, occ::down_cast<Geom2d_BezierCurve>(theCurve2));
  }
  if (occ::handle<Geom2d_BSplineCurve> aBSpl1 = occ::down_cast<Geom2d_BSplineCurve>(theCurve1))
  {
    return Geom2dHash_BSplineCurveHasher{}(aBSpl1, occ::down_cast<Geom2d_BSplineCurve>(theCurve2));
  }
  if (occ::handle<Geom2d_TrimmedCurve> aTrim1 = occ::down_cast<Geom2d_TrimmedCurve>(theCurve1))
  {
    return Geom2dHash_TrimmedCurveHasher{}(aTrim1, occ::down_cast<Geom2d_TrimmedCurve>(theCurve2));
  }
  if (occ::handle<Geom2d_OffsetCurve> aOff1 = occ::down_cast<Geom2d_OffsetCurve>(theCurve1))
  {
    return Geom2dHash_OffsetCurveHasher{}(aOff1, occ::down_cast<Geom2d_OffsetCurve>(theCurve2));
  }

  // Unknown curve type - compare by pointer
  return theCurve1.get() == theCurve2.get();
}
