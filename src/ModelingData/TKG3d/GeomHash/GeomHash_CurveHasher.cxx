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

GeomHash_CurveHasher::GeomHash_CurveHasher(double theCompTolerance,
                                            double theHashTolerance)
  : CompTolerance(theCompTolerance),
    HashTolerance(theHashTolerance)
{
}

//=================================================================================================

std::size_t GeomHash_CurveHasher::operator()(const occ::handle<Geom_Curve>& theCurve) const noexcept
{
  if (theCurve.IsNull())
  {
    return 0;
  }

  // Dispatch based on actual curve type using DynamicType check first (cheaper than down_cast)
  const Handle(Standard_Type)& aType = theCurve->DynamicType();
  if (aType == STANDARD_TYPE(Geom_Line))
  {
    return GeomHash_LineHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Line>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom_Circle))
  {
    return GeomHash_CircleHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Circle>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom_Ellipse))
  {
    return GeomHash_EllipseHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Ellipse>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom_Hyperbola))
  {
    return GeomHash_HyperbolaHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Hyperbola>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom_Parabola))
  {
    return GeomHash_ParabolaHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Parabola>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom_BezierCurve))
  {
    return GeomHash_BezierCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_BezierCurve>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom_BSplineCurve))
  {
    return GeomHash_BSplineCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_BSplineCurve>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    return GeomHash_TrimmedCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_TrimmedCurve>(theCurve));
  }
  else if (aType == STANDARD_TYPE(Geom_OffsetCurve))
  {
    return GeomHash_OffsetCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_OffsetCurve>(theCurve));
  }

  // Unknown curve type - hash the type name
  return Standard_CStringHasher{}(aType->Name());
}

//=================================================================================================

bool GeomHash_CurveHasher::operator()(const occ::handle<Geom_Curve>& theCurve1,
                                      const occ::handle<Geom_Curve>& theCurve2) const noexcept
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
  if (aType == STANDARD_TYPE(Geom_Line))
  {
    return GeomHash_LineHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Line>(theCurve1),
      occ::down_cast<Geom_Line>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom_Circle))
  {
    return GeomHash_CircleHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Circle>(theCurve1),
      occ::down_cast<Geom_Circle>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom_Ellipse))
  {
    return GeomHash_EllipseHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Ellipse>(theCurve1),
      occ::down_cast<Geom_Ellipse>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom_Hyperbola))
  {
    return GeomHash_HyperbolaHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Hyperbola>(theCurve1),
      occ::down_cast<Geom_Hyperbola>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom_Parabola))
  {
    return GeomHash_ParabolaHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_Parabola>(theCurve1),
      occ::down_cast<Geom_Parabola>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom_BezierCurve))
  {
    return GeomHash_BezierCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_BezierCurve>(theCurve1),
      occ::down_cast<Geom_BezierCurve>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom_BSplineCurve))
  {
    return GeomHash_BSplineCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_BSplineCurve>(theCurve1),
      occ::down_cast<Geom_BSplineCurve>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom_TrimmedCurve))
  {
    return GeomHash_TrimmedCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_TrimmedCurve>(theCurve1),
      occ::down_cast<Geom_TrimmedCurve>(theCurve2));
  }
  else if (aType == STANDARD_TYPE(Geom_OffsetCurve))
  {
    return GeomHash_OffsetCurveHasher{CompTolerance, HashTolerance}(
      occ::down_cast<Geom_OffsetCurve>(theCurve1),
      occ::down_cast<Geom_OffsetCurve>(theCurve2));
  }

  // Unknown curve type - compare by pointer
  return theCurve1.get() == theCurve2.get();
}
