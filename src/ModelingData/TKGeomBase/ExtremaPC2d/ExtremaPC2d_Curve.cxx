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

#include <ExtremaPC2d_Curve.hxx>

#include <algorithm>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>

namespace
{
//! Static result for uninitialized evaluator case
static const ExtremaPC2d::Result THE_NOT_DONE_RESULT = [] {
  ExtremaPC2d::Result aResult;
  aResult.Status = ExtremaPC2d::Status::NotDone;
  return aResult;
}();
} // namespace

//=================================================================================================

void ExtremaPC2d_Curve::initFromAdaptor(const Adaptor2d_Curve2d&     theCurve,
                                        const ExtremaPC2d::Domain1D& theDomain)
{
  const GeomAbs_CurveType aCurveType = theCurve.GetType();

  switch (aCurveType)
  {
    case GeomAbs_Line:
      myEvaluator = ExtremaPC2d_Line(theCurve.Line(), theDomain);
      break;

    case GeomAbs_Circle:
      myEvaluator = ExtremaPC2d_Circle(theCurve.Circle(), theDomain);
      break;

    case GeomAbs_Ellipse:
      myEvaluator = ExtremaPC2d_Ellipse(theCurve.Ellipse(), theDomain);
      break;

    case GeomAbs_Hyperbola:
      myEvaluator = ExtremaPC2d_Hyperbola(theCurve.Hyperbola(), theDomain);
      break;

    case GeomAbs_Parabola:
      myEvaluator = ExtremaPC2d_Parabola(theCurve.Parabola(), theDomain);
      break;

    case GeomAbs_BezierCurve:
      myEvaluator = ExtremaPC2d_BezierCurve(theCurve.Bezier(), theDomain);
      break;

    case GeomAbs_BSplineCurve:
      myEvaluator = ExtremaPC2d_BSplineCurve(theCurve.BSpline(), theDomain);
      break;

    case GeomAbs_OffsetCurve:
      myAdaptorRef = &theCurve;
      myEvaluator  = ExtremaPC2d_OffsetCurve(*myAdaptorRef, theDomain);
      break;

    default:
      myAdaptorRef = &theCurve;
      myEvaluator  = ExtremaPC2d_OtherCurve(*myAdaptorRef, theDomain);
      break;
  }
}

//=================================================================================================

ExtremaPC2d_Curve::ExtremaPC2d_Curve(const Adaptor2d_Curve2d& theCurve)
    : myEvaluator(std::monostate{})
{
  ExtremaPC2d::Domain1D aDomain(theCurve.FirstParameter(), theCurve.LastParameter());
  initFromAdaptor(theCurve, aDomain);
}

//=================================================================================================

ExtremaPC2d_Curve::ExtremaPC2d_Curve(const Adaptor2d_Curve2d& theCurve,
                                     double                   theUMin,
                                     double                   theUMax)
    : myEvaluator(std::monostate{})
{
  ExtremaPC2d::Domain1D aDomain(theUMin, theUMax);
  initFromAdaptor(theCurve, aDomain);
}

//=================================================================================================

void ExtremaPC2d_Curve::initFromGeomCurve(const occ::handle<Geom2d_Curve>&            theCurve,
                                          const std::optional<ExtremaPC2d::Domain1D>& theDomain)
{
  // Try specific curve types for direct initialization
  occ::handle<Geom2d_Line> aLine = occ::down_cast<Geom2d_Line>(theCurve);
  if (!aLine.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_Line(aLine->Lin2d(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_Line(aLine->Lin2d());
    }
    return;
  }

  occ::handle<Geom2d_Circle> aCircle = occ::down_cast<Geom2d_Circle>(theCurve);
  if (!aCircle.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_Circle(aCircle->Circ2d(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_Circle(aCircle->Circ2d());
    }
    return;
  }

  occ::handle<Geom2d_Ellipse> anEllipse = occ::down_cast<Geom2d_Ellipse>(theCurve);
  if (!anEllipse.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_Ellipse(anEllipse->Elips2d(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_Ellipse(anEllipse->Elips2d());
    }
    return;
  }

  occ::handle<Geom2d_Hyperbola> aHyperbola = occ::down_cast<Geom2d_Hyperbola>(theCurve);
  if (!aHyperbola.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_Hyperbola(aHyperbola->Hypr2d(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_Hyperbola(aHyperbola->Hypr2d());
    }
    return;
  }

  occ::handle<Geom2d_Parabola> aParabola = occ::down_cast<Geom2d_Parabola>(theCurve);
  if (!aParabola.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_Parabola(aParabola->Parab2d(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_Parabola(aParabola->Parab2d());
    }
    return;
  }

  occ::handle<Geom2d_BezierCurve> aBezier = occ::down_cast<Geom2d_BezierCurve>(theCurve);
  if (!aBezier.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_BezierCurve(aBezier, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_BezierCurve(aBezier);
    }
    return;
  }

  occ::handle<Geom2d_BSplineCurve> aBSpline = occ::down_cast<Geom2d_BSplineCurve>(theCurve);
  if (!aBSpline.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_BSplineCurve(aBSpline, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_BSplineCurve(aBSpline);
    }
    return;
  }

  // For offset and other curves, store adaptor and use it
  // Use domain bounds if set, otherwise use natural curve bounds
  if (theDomain.has_value())
  {
    myAdaptorOwned = new Geom2dAdaptor_Curve(theCurve, theDomain->Min, theDomain->Max);
  }
  else
  {
    myAdaptorOwned = new Geom2dAdaptor_Curve(theCurve);
  }
  const GeomAbs_CurveType aCurveType = myAdaptorOwned->GetType();

  if (aCurveType == GeomAbs_OffsetCurve)
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_OffsetCurve(*myAdaptorOwned, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_OffsetCurve(*myAdaptorOwned);
    }
  }
  else
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC2d_OtherCurve(*myAdaptorOwned, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC2d_OtherCurve(*myAdaptorOwned);
    }
  }
}

//=================================================================================================

ExtremaPC2d_Curve::ExtremaPC2d_Curve(const occ::handle<Geom2d_Curve>& theCurve)
    : myEvaluator(std::monostate{})
{
  if (theCurve.IsNull())
  {
    return;
  }

  // Check for trimmed curve - if so, use bounds
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = occ::down_cast<Geom2d_TrimmedCurve>(theCurve);
  if (!aTrimmed.IsNull())
  {
    ExtremaPC2d::Domain1D aDomain(aTrimmed->FirstParameter(), aTrimmed->LastParameter());
    initFromGeomCurve(aTrimmed->BasisCurve(), aDomain);
    return;
  }

  // Initialize based on curve type - without setting domain (unbounded)
  initFromGeomCurve(theCurve, std::nullopt);
}

//=================================================================================================

ExtremaPC2d_Curve::ExtremaPC2d_Curve(const occ::handle<Geom2d_Curve>& theCurve,
                                     double                           theUMin,
                                     double                           theUMax)
    : myEvaluator(std::monostate{})
{
  if (theCurve.IsNull())
  {
    return;
  }

  // Get base curve and effective bounds
  occ::handle<Geom2d_Curve> aBaseCurve     = theCurve;
  double                    aEffectiveUMin = theUMin;
  double                    aEffectiveUMax = theUMax;

  // For trimmed curve, intersect input bounds with trimmed bounds
  occ::handle<Geom2d_TrimmedCurve> aTrimmed = occ::down_cast<Geom2d_TrimmedCurve>(theCurve);
  if (!aTrimmed.IsNull())
  {
    aBaseCurve = aTrimmed->BasisCurve();
    // Intersect bounds: take max of mins, min of maxs
    aEffectiveUMin = std::max(theUMin, aTrimmed->FirstParameter());
    aEffectiveUMax = std::min(theUMax, aTrimmed->LastParameter());
  }

  // Use common helper for curve type detection and evaluator creation
  ExtremaPC2d::Domain1D aDomain(aEffectiveUMin, aEffectiveUMax);
  initFromGeomCurve(aBaseCurve, aDomain);
}

//=================================================================================================

const ExtremaPC2d::Result& ExtremaPC2d_Curve::Perform(const gp_Pnt2d&         theP,
                                                      double                  theTol,
                                                      ExtremaPC2d::SearchMode theMode) const
{
  const ExtremaPC2d::Result* aResultPtr = &THE_NOT_DONE_RESULT;

  std::visit(
    [&](auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        aResultPtr = &theEval.Perform(theP, theTol, theMode);
      }
    },
    myEvaluator);
  return *aResultPtr;
}

//=================================================================================================

const ExtremaPC2d::Result& ExtremaPC2d_Curve::PerformWithEndpoints(
  const gp_Pnt2d&         theP,
  double                  theTol,
  ExtremaPC2d::SearchMode theMode) const
{
  const ExtremaPC2d::Result* aResultPtr = &THE_NOT_DONE_RESULT;

  std::visit(
    [&](auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        aResultPtr = &theEval.PerformWithEndpoints(theP, theTol, theMode);
      }
    },
    myEvaluator);
  return *aResultPtr;
}
