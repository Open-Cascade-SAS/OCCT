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

Geom2dHash_CurveHasher::Geom2dHash_CurveHasher(double theCompTolerance, double theHashTolerance)
    : CompTolerance(theCompTolerance),
      HashTolerance(theHashTolerance)
{
}

//=================================================================================================

std::size_t Geom2dHash_CurveHasher::operator()(
  const occ::handle<Geom2d_Curve>& theCurve) const noexcept
{
  if (theCurve.IsNull())
  {
    return 0;
  }

  // Dispatch based on actual curve type using DynamicType check first (cheaper than down_cast)
  const Handle(Standard_Type)& aType = theCurve->DynamicType();
  if (aType == STANDARD_TYPE(Geom2d_Line))
  {
    return Geom2dHash_LineHasher{CompTolerance,
                                 HashTolerance}(occ::down_cast<Geom2d_Line>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom2d_Circle))
  {
    return Geom2dHash_CircleHasher{CompTolerance,
                                   HashTolerance}(occ::down_cast<Geom2d_Circle>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom2d_Ellipse))
  {
    return Geom2dHash_EllipseHasher{CompTolerance,
                                    HashTolerance}(occ::down_cast<Geom2d_Ellipse>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    return Geom2dHash_HyperbolaHasher{CompTolerance,
                                      HashTolerance}(occ::down_cast<Geom2d_Hyperbola>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom2d_Parabola))
  {
    return Geom2dHash_ParabolaHasher{CompTolerance,
                                     HashTolerance}(occ::down_cast<Geom2d_Parabola>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    return Geom2dHash_BezierCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom2d_BezierCurve>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    return Geom2dHash_BSplineCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom2d_BSplineCurve>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    return Geom2dHash_TrimmedCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom2d_TrimmedCurve>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    return Geom2dHash_OffsetCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom2d_OffsetCurve>(theCurve));
  }

  // Unknown curve type - hash the type name
  return Standard_CStringHasher{}(aType->Name());
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

  // Dispatch based on actual curve type using DynamicType check (already confirmed types match)
  const Handle(Standard_Type)& aType = theCurve1->DynamicType();
  if (aType == STANDARD_TYPE(Geom2d_Line))
  {
    return Geom2dHash_LineHasher{CompTolerance,
                                 HashTolerance}(occ::down_cast<Geom2d_Line>(theCurve1),
                                                occ::down_cast<Geom2d_Line>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom2d_Circle))
  {
    return Geom2dHash_CircleHasher{CompTolerance,
                                   HashTolerance}(occ::down_cast<Geom2d_Circle>(theCurve1),
                                                  occ::down_cast<Geom2d_Circle>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom2d_Ellipse))
  {
    return Geom2dHash_EllipseHasher{CompTolerance,
                                    HashTolerance}(occ::down_cast<Geom2d_Ellipse>(theCurve1),
                                                   occ::down_cast<Geom2d_Ellipse>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    return Geom2dHash_HyperbolaHasher{CompTolerance,
                                      HashTolerance}(occ::down_cast<Geom2d_Hyperbola>(theCurve1),
                                                     occ::down_cast<Geom2d_Hyperbola>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom2d_Parabola))
  {
    return Geom2dHash_ParabolaHasher{CompTolerance,
                                     HashTolerance}(occ::down_cast<Geom2d_Parabola>(theCurve1),
                                                    occ::down_cast<Geom2d_Parabola>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    return Geom2dHash_BezierCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom2d_BezierCurve>(theCurve1),
      occ::down_cast<Geom2d_BezierCurve>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    return Geom2dHash_BSplineCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom2d_BSplineCurve>(theCurve1),
      occ::down_cast<Geom2d_BSplineCurve>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom2d_TrimmedCurve))
  {
    return Geom2dHash_TrimmedCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom2d_TrimmedCurve>(theCurve1),
      occ::down_cast<Geom2d_TrimmedCurve>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    return Geom2dHash_OffsetCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom2d_OffsetCurve>(theCurve1),
      occ::down_cast<Geom2d_OffsetCurve>(theCurve2));
  }

  // Unknown curve type - compare by pointer
  return theCurve1.get() == theCurve2.get();
}
