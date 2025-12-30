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

#include <ExtremaPC_Curve.hxx>

#include <algorithm>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>

namespace
{
//! Static result for uninitialized evaluator case
static ExtremaPC::Result THE_NOT_DONE_RESULT = [] {
  ExtremaPC::Result aResult;
  aResult.Status = ExtremaPC::Status::NotDone;
  return aResult;
}();
} // namespace

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Adaptor3d_Curve& theCurve)
    : myEvaluator(std::monostate{})
{
  ExtremaPC::Domain1D      aDomain(theCurve.FirstParameter(), theCurve.LastParameter());
  const GeomAbs_CurveType aCurveType = theCurve.GetType();

  switch (aCurveType)
  {
    case GeomAbs_Line:
      myEvaluator = ExtremaPC_Line(theCurve.Line(), aDomain);
      break;

    case GeomAbs_Circle:
      myEvaluator = ExtremaPC_Circle(theCurve.Circle(), aDomain);
      break;

    case GeomAbs_Ellipse:
      myEvaluator = ExtremaPC_Ellipse(theCurve.Ellipse(), aDomain);
      break;

    case GeomAbs_Hyperbola:
      myEvaluator = ExtremaPC_Hyperbola(theCurve.Hyperbola(), aDomain);
      break;

    case GeomAbs_Parabola:
      myEvaluator = ExtremaPC_Parabola(theCurve.Parabola(), aDomain);
      break;

    case GeomAbs_BezierCurve:
      myEvaluator = ExtremaPC_BezierCurve(theCurve.Bezier(), aDomain);
      break;

    case GeomAbs_BSplineCurve:
      myEvaluator = ExtremaPC_BSplineCurve(theCurve.BSpline(), aDomain);
      break;

    case GeomAbs_OffsetCurve:
      myEvaluator = ExtremaPC_OffsetCurve(theCurve, aDomain);
      break;

    default:
      myEvaluator = ExtremaPC_OtherCurve(theCurve, aDomain);
      break;
  }
}

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Adaptor3d_Curve& theCurve, double theUMin, double theUMax)
    : myEvaluator(std::monostate{})
{
  ExtremaPC::Domain1D      aDomain(theUMin, theUMax);
  const GeomAbs_CurveType aCurveType = theCurve.GetType();

  switch (aCurveType)
  {
    case GeomAbs_Line:
      myEvaluator = ExtremaPC_Line(theCurve.Line(), aDomain);
      break;

    case GeomAbs_Circle:
      myEvaluator = ExtremaPC_Circle(theCurve.Circle(), aDomain);
      break;

    case GeomAbs_Ellipse:
      myEvaluator = ExtremaPC_Ellipse(theCurve.Ellipse(), aDomain);
      break;

    case GeomAbs_Hyperbola:
      myEvaluator = ExtremaPC_Hyperbola(theCurve.Hyperbola(), aDomain);
      break;

    case GeomAbs_Parabola:
      myEvaluator = ExtremaPC_Parabola(theCurve.Parabola(), aDomain);
      break;

    case GeomAbs_BezierCurve:
      myEvaluator = ExtremaPC_BezierCurve(theCurve.Bezier(), aDomain);
      break;

    case GeomAbs_BSplineCurve:
      myEvaluator = ExtremaPC_BSplineCurve(theCurve.BSpline(), aDomain);
      break;

    case GeomAbs_OffsetCurve:
      myEvaluator = ExtremaPC_OffsetCurve(theCurve, aDomain);
      break;

    default:
      myEvaluator = ExtremaPC_OtherCurve(theCurve, aDomain);
      break;
  }
}

//==================================================================================================

void ExtremaPC_Curve::initFromGeomCurve(const Handle(Geom_Curve)&                 theCurve,
                                         const std::optional<ExtremaPC::Domain1D>& theDomain)
{
  // Try specific curve types for direct initialization
  Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(theCurve);
  if (!aLine.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_Line(aLine->Lin(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_Line(aLine->Lin());
    }
    return;
  }

  Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast(theCurve);
  if (!aCircle.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_Circle(aCircle->Circ(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_Circle(aCircle->Circ());
    }
    return;
  }

  Handle(Geom_Ellipse) anEllipse = Handle(Geom_Ellipse)::DownCast(theCurve);
  if (!anEllipse.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_Ellipse(anEllipse->Elips(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_Ellipse(anEllipse->Elips());
    }
    return;
  }

  Handle(Geom_Hyperbola) aHyperbola = Handle(Geom_Hyperbola)::DownCast(theCurve);
  if (!aHyperbola.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_Hyperbola(aHyperbola->Hypr(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_Hyperbola(aHyperbola->Hypr());
    }
    return;
  }

  Handle(Geom_Parabola) aParabola = Handle(Geom_Parabola)::DownCast(theCurve);
  if (!aParabola.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_Parabola(aParabola->Parab(), theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_Parabola(aParabola->Parab());
    }
    return;
  }

  Handle(Geom_BezierCurve) aBezier = Handle(Geom_BezierCurve)::DownCast(theCurve);
  if (!aBezier.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_BezierCurve(aBezier, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_BezierCurve(aBezier);
    }
    return;
  }

  Handle(Geom_BSplineCurve) aBSpline = Handle(Geom_BSplineCurve)::DownCast(theCurve);
  if (!aBSpline.IsNull())
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_BSplineCurve(aBSpline, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_BSplineCurve(aBSpline);
    }
    return;
  }

  // For offset and other curves, store adaptor and use it
  // Use domain bounds if set, otherwise use natural curve bounds
  if (theDomain.has_value())
  {
    myAdaptor = new GeomAdaptor_Curve(theCurve, theDomain->Min, theDomain->Max);
  }
  else
  {
    myAdaptor = new GeomAdaptor_Curve(theCurve);
  }
  const GeomAbs_CurveType aCurveType = myAdaptor->GetType();

  if (aCurveType == GeomAbs_OffsetCurve)
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_OffsetCurve(*myAdaptor, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_OffsetCurve(*myAdaptor);
    }
  }
  else
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_OtherCurve(*myAdaptor, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_OtherCurve(*myAdaptor);
    }
  }
}

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Handle(Geom_Curve)& theCurve)
    : myEvaluator(std::monostate{})
{
  if (theCurve.IsNull())
  {
    return;
  }

  // Check for trimmed curve - if so, use bounds
  Handle(Geom_TrimmedCurve) aTrimmed = Handle(Geom_TrimmedCurve)::DownCast(theCurve);
  if (!aTrimmed.IsNull())
  {
    ExtremaPC::Domain1D aDomain(aTrimmed->FirstParameter(), aTrimmed->LastParameter());
    initFromGeomCurve(aTrimmed->BasisCurve(), aDomain);
    return;
  }

  // Initialize based on curve type - without setting domain (unbounded)
  initFromGeomCurve(theCurve, std::nullopt);
}

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Handle(Geom_Curve)& theCurve, double theUMin, double theUMax)
    : myEvaluator(std::monostate{})
{
  if (theCurve.IsNull())
  {
    return;
  }

  // Get base curve and effective bounds
  Handle(Geom_Curve) aBaseCurve     = theCurve;
  double             aEffectiveUMin = theUMin;
  double             aEffectiveUMax = theUMax;

  // For trimmed curve, intersect input bounds with trimmed bounds
  Handle(Geom_TrimmedCurve) aTrimmed = Handle(Geom_TrimmedCurve)::DownCast(theCurve);
  if (!aTrimmed.IsNull())
  {
    aBaseCurve = aTrimmed->BasisCurve();
    // Intersect bounds: take max of mins, min of maxs
    aEffectiveUMin = std::max(theUMin, aTrimmed->FirstParameter());
    aEffectiveUMax = std::min(theUMax, aTrimmed->LastParameter());
  }

  // Use common helper for curve type detection and evaluator creation
  ExtremaPC::Domain1D aDomain(aEffectiveUMin, aEffectiveUMax);
  initFromGeomCurve(aBaseCurve, aDomain);
}

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_Curve::Perform(const gp_Pnt&         theP,
                                                   double                theTol,
                                                   ExtremaPC::SearchMode theMode) const
{
  const ExtremaPC::Result* aResultPtr = &THE_NOT_DONE_RESULT;
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

//==================================================================================================

const ExtremaPC::Result& ExtremaPC_Curve::PerformWithEndpoints(const gp_Pnt&         theP,
                                                                double                theTol,
                                                                ExtremaPC::SearchMode theMode) const
{
  const ExtremaPC::Result* aResultPtr = &THE_NOT_DONE_RESULT;
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
