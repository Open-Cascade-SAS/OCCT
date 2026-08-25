// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <ExtremaPC_Planar.hxx>

#include <gp_Ax3.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Vec.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_LocalArray.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>

#include <cmath>

namespace
{
bool findExactPolePlane(const NCollection_Array1<gp_Pnt>& thePoles, gp_Pln& thePlane)
{
  if (thePoles.IsEmpty())
  {
    return false;
  }

  const gp_Pnt& anOrigin = thePoles.First();
  gp_Vec        aBase;
  for (const gp_Pnt& aPole : thePoles)
  {
    aBase = gp_Vec(anOrigin, aPole);
    if (aBase.SquareMagnitude() > 0.0)
    {
      break;
    }
  }

  if (aBase.SquareMagnitude() == 0.0)
  {
    thePlane = gp_Pln(gp_Ax3(anOrigin, gp::DZ(), gp::DX()));
    return true;
  }

  gp_Vec aSecond;
  gp_Vec aNormal;
  for (const gp_Pnt& aPole : thePoles)
  {
    aSecond = gp_Vec(anOrigin, aPole);
    aNormal = aBase.Crossed(aSecond);
    if (aNormal.SquareMagnitude() > 0.0)
    {
      break;
    }
  }

  const gp_Dir anXDirection(aBase);
  if (aNormal.SquareMagnitude() == 0.0)
  {
    const gp_Dir aReference = std::abs(anXDirection.Z()) < 0.9 ? gp::DZ() : gp::DY();
    aNormal                 = gp_Vec(anXDirection).Crossed(gp_Vec(aReference));
    thePlane                = gp_Pln(gp_Ax3(anOrigin, gp_Dir(aNormal), anXDirection));
    return true;
  }

  for (const gp_Pnt& aPole : thePoles)
  {
    const gp_Vec      aVector(anOrigin, aPole);
    const long double aTriple = static_cast<long double>(aBase.X())
                                  * (static_cast<long double>(aSecond.Y()) * aVector.Z()
                                     - static_cast<long double>(aSecond.Z()) * aVector.Y())
                                - static_cast<long double>(aBase.Y())
                                    * (static_cast<long double>(aSecond.X()) * aVector.Z()
                                       - static_cast<long double>(aSecond.Z()) * aVector.X())
                                + static_cast<long double>(aBase.Z())
                                    * (static_cast<long double>(aSecond.X()) * aVector.Y()
                                       - static_cast<long double>(aSecond.Y()) * aVector.X());
    if (aTriple != 0.0L)
    {
      return false;
    }
  }

  thePlane = gp_Pln(gp_Ax3(anOrigin, gp_Dir(aNormal), anXDirection));
  return true;
}

bool isParallel(const gp_Dir& theFirst, const gp_Dir& theSecond)
{
  return gp_Vec(theFirst).Crossed(gp_Vec(theSecond)).SquareMagnitude() == 0.0;
}

bool projectBasisCurve(const occ::handle<Geom_Curve>& theCurve,
                       const gp_Dir&                  theNormal,
                       gp_Pln&                        thePlane,
                       occ::handle<Geom2d_Curve>&     theCurve2d)
{
  theCurve2d.Nullify();
  if (theCurve.IsNull())
  {
    return false;
  }

  const occ::handle<Geom_TrimmedCurve> aTrimmed = occ::down_cast<Geom_TrimmedCurve>(theCurve);
  if (!aTrimmed.IsNull())
  {
    occ::handle<Geom2d_Curve> aBasis2d;
    if (!projectBasisCurve(aTrimmed->BasisCurve(), theNormal, thePlane, aBasis2d))
    {
      return false;
    }
    theCurve2d =
      new Geom2d_TrimmedCurve(aBasis2d, aTrimmed->FirstParameter(), aTrimmed->LastParameter());
    return true;
  }

  const occ::handle<Geom_Line> aLine = occ::down_cast<Geom_Line>(theCurve);
  if (!aLine.IsNull())
  {
    if (gp_Vec(aLine->Lin().Direction()).Dot(gp_Vec(theNormal)) != 0.0)
    {
      return false;
    }
    thePlane   = gp_Pln(gp_Ax3(aLine->Lin().Location(), theNormal, aLine->Lin().Direction()));
    theCurve2d = new Geom2d_Line(ProjLib::Project(thePlane, aLine->Lin()));
    return true;
  }

  const occ::handle<Geom_Circle> aCircle = occ::down_cast<Geom_Circle>(theCurve);
  if (!aCircle.IsNull() && isParallel(aCircle->Circ().Axis().Direction(), theNormal))
  {
    thePlane   = gp_Pln(gp_Ax3(aCircle->Circ().Position()));
    theCurve2d = new Geom2d_Circle(ProjLib::Project(thePlane, aCircle->Circ()));
    return true;
  }

  const occ::handle<Geom_Ellipse> anEllipse = occ::down_cast<Geom_Ellipse>(theCurve);
  if (!anEllipse.IsNull() && isParallel(anEllipse->Elips().Axis().Direction(), theNormal))
  {
    thePlane   = gp_Pln(gp_Ax3(anEllipse->Elips().Position()));
    theCurve2d = new Geom2d_Ellipse(ProjLib::Project(thePlane, anEllipse->Elips()));
    return true;
  }

  const occ::handle<Geom_Hyperbola> aHyperbola = occ::down_cast<Geom_Hyperbola>(theCurve);
  if (!aHyperbola.IsNull() && isParallel(aHyperbola->Hypr().Axis().Direction(), theNormal))
  {
    thePlane   = gp_Pln(gp_Ax3(aHyperbola->Hypr().Position()));
    theCurve2d = new Geom2d_Hyperbola(ProjLib::Project(thePlane, aHyperbola->Hypr()));
    return true;
  }

  const occ::handle<Geom_Parabola> aParabola = occ::down_cast<Geom_Parabola>(theCurve);
  if (!aParabola.IsNull() && isParallel(aParabola->Parab().Axis().Direction(), theNormal))
  {
    thePlane   = gp_Pln(gp_Ax3(aParabola->Parab().Position()));
    theCurve2d = new Geom2d_Parabola(ProjLib::Project(thePlane, aParabola->Parab()));
    return true;
  }

  const occ::handle<Geom_BezierCurve> aBezier = occ::down_cast<Geom_BezierCurve>(theCurve);
  occ::handle<Geom2d_BezierCurve>     aBezier2d;
  if (!aBezier.IsNull() && ExtremaPC::ProjectBezier(aBezier, thePlane, aBezier2d)
      && isParallel(thePlane.Axis().Direction(), theNormal))
  {
    theCurve2d = aBezier2d;
    return true;
  }

  const occ::handle<Geom_BSplineCurve> aBSpline = occ::down_cast<Geom_BSplineCurve>(theCurve);
  occ::handle<Geom2d_BSplineCurve>     aBSpline2d;
  if (!aBSpline.IsNull() && ExtremaPC::ProjectBSpline(aBSpline, thePlane, aBSpline2d)
      && isParallel(thePlane.Axis().Direction(), theNormal))
  {
    theCurve2d = aBSpline2d;
    return true;
  }
  return false;
}
} // namespace

//=================================================================================================

gp_Pln ExtremaPC::LinePlane(const gp_Lin& theLine)
{
  const gp_Dir& aLineDirection = theLine.Direction();
  const gp_Dir  aReference     = std::abs(aLineDirection.Z()) < 0.9 ? gp::DZ() : gp::DY();
  const gp_Dir  aNormal(gp_Vec(aLineDirection).Crossed(gp_Vec(aReference)));
  return gp_Pln(gp_Ax3(theLine.Location(), aNormal, aLineDirection));
}

//=================================================================================================

bool ExtremaPC::ProjectBezier(const occ::handle<Geom_BezierCurve>& theCurve,
                              gp_Pln&                              thePlane,
                              occ::handle<Geom2d_BezierCurve>&     theCurve2d)
{
  theCurve2d.Nullify();
  if (theCurve.IsNull() || theCurve->HasEvalRepresentation()
      || !findExactPolePlane(theCurve->Poles(), thePlane))
  {
    return false;
  }

  NCollection_LocalArray<gp_Pnt2d, 32> aPoleStorage(theCurve->Poles().Size());
  NCollection_Array1<gp_Pnt2d>         aPoles(aPoleStorage[0],
                                      theCurve->Poles().Lower(),
                                      theCurve->Poles().Upper());
  for (size_t anIndex = 0; anIndex < aPoles.Size(); ++anIndex)
  {
    aPoles.ChangeAt(anIndex) = ProjLib::Project(thePlane, theCurve->Poles().At(anIndex));
  }
  theCurve2d = theCurve->IsRational() ? new Geom2d_BezierCurve(aPoles, theCurve->WeightsArray())
                                      : new Geom2d_BezierCurve(aPoles);
  return theCurve2d->IsRational() == theCurve->IsRational();
}

//=================================================================================================

bool ExtremaPC::ProjectBSpline(const occ::handle<Geom_BSplineCurve>& theCurve,
                               gp_Pln&                               thePlane,
                               occ::handle<Geom2d_BSplineCurve>&     theCurve2d)
{
  theCurve2d.Nullify();
  if (theCurve.IsNull() || theCurve->HasEvalRepresentation()
      || !findExactPolePlane(theCurve->Poles(), thePlane))
  {
    return false;
  }

  NCollection_LocalArray<gp_Pnt2d, 64> aPoleStorage(theCurve->Poles().Size());
  NCollection_Array1<gp_Pnt2d>         aPoles(aPoleStorage[0],
                                      theCurve->Poles().Lower(),
                                      theCurve->Poles().Upper());
  for (size_t anIndex = 0; anIndex < aPoles.Size(); ++anIndex)
  {
    aPoles.ChangeAt(anIndex) = ProjLib::Project(thePlane, theCurve->Poles().At(anIndex));
  }
  theCurve2d = theCurve->IsRational() ? new Geom2d_BSplineCurve(aPoles,
                                                                theCurve->WeightsArray(),
                                                                theCurve->Knots(),
                                                                theCurve->Multiplicities(),
                                                                theCurve->Degree(),
                                                                theCurve->IsPeriodic())
                                      : new Geom2d_BSplineCurve(aPoles,
                                                                theCurve->Knots(),
                                                                theCurve->Multiplicities(),
                                                                theCurve->Degree(),
                                                                theCurve->IsPeriodic());
  return theCurve2d->IsRational() == theCurve->IsRational();
}

//=================================================================================================

bool ExtremaPC::ProjectOffset(const occ::handle<Geom_OffsetCurve>& theCurve,
                              gp_Pln&                              thePlane,
                              occ::handle<Geom2d_OffsetCurve>&     theCurve2d)
{
  theCurve2d.Nullify();
  if (theCurve.IsNull() || theCurve->HasEvalRepresentation())
  {
    return false;
  }

  occ::handle<Geom2d_Curve> aBasis2d;
  if (!projectBasisCurve(theCurve->BasisCurve(), theCurve->Direction(), thePlane, aBasis2d))
  {
    return false;
  }

  const double aProbe = (theCurve->FirstParameter() + theCurve->LastParameter()) * 0.5;
  if (!std::isfinite(aProbe))
  {
    return false;
  }

  for (const double aSign : {-1.0, 1.0})
  {
    occ::handle<Geom2d_OffsetCurve> aCandidate =
      new Geom2d_OffsetCurve(aBasis2d, aSign * theCurve->Offset());
    const gp_Pnt2d aPoint2d = aCandidate->Value(aProbe);
    const gp_Pnt   aPoint3d = ElSLib::Value(aPoint2d.X(), aPoint2d.Y(), thePlane);
    if (aPoint3d.SquareDistance(theCurve->Value(aProbe)) == 0.0)
    {
      theCurve2d = aCandidate;
      return true;
    }
  }
  return false;
}
