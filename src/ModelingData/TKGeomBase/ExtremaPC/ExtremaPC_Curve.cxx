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

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Adaptor3d_Curve& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(theCurve.GetType())
{
  myConfig.Domain = ExtremaPC::Domain1D(theCurve.FirstParameter(), theCurve.LastParameter());

  switch (myCurveType)
  {
    case GeomAbs_Line:
      myEvaluator = ExtremaPC_Line(theCurve.Line());
      break;

    case GeomAbs_Circle:
      myEvaluator = ExtremaPC_Circle(theCurve.Circle());
      break;

    case GeomAbs_Ellipse:
      myEvaluator = ExtremaPC_Ellipse(theCurve.Ellipse());
      break;

    case GeomAbs_Hyperbola:
      myEvaluator = ExtremaPC_Hyperbola(theCurve.Hyperbola());
      break;

    case GeomAbs_Parabola:
      myEvaluator = ExtremaPC_Parabola(theCurve.Parabola());
      break;

    case GeomAbs_BezierCurve:
      myEvaluator = ExtremaPC_BezierCurve(theCurve.Bezier());
      break;

    case GeomAbs_BSplineCurve:
      myEvaluator = ExtremaPC_BSplineCurve(theCurve.BSpline());
      break;

    case GeomAbs_OffsetCurve:
      myEvaluator = ExtremaPC_OffsetCurve(theCurve);
      break;

    default:
      myEvaluator = ExtremaPC_OtherCurve(theCurve);
      break;
  }
}

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Adaptor3d_Curve& theCurve, double theUMin, double theUMax)
    : myEvaluator(std::monostate{}),
      myCurveType(theCurve.GetType())
{
  myConfig.Domain = ExtremaPC::Domain1D(theUMin, theUMax);

  switch (myCurveType)
  {
    case GeomAbs_Line:
      myEvaluator = ExtremaPC_Line(theCurve.Line());
      break;

    case GeomAbs_Circle:
      myEvaluator = ExtremaPC_Circle(theCurve.Circle());
      break;

    case GeomAbs_Ellipse:
      myEvaluator = ExtremaPC_Ellipse(theCurve.Ellipse());
      break;

    case GeomAbs_Hyperbola:
      myEvaluator = ExtremaPC_Hyperbola(theCurve.Hyperbola());
      break;

    case GeomAbs_Parabola:
      myEvaluator = ExtremaPC_Parabola(theCurve.Parabola());
      break;

    case GeomAbs_BezierCurve:
      myEvaluator = ExtremaPC_BezierCurve(theCurve.Bezier());
      break;

    case GeomAbs_BSplineCurve:
      myEvaluator = ExtremaPC_BSplineCurve(theCurve.BSpline());
      break;

    case GeomAbs_OffsetCurve:
      myEvaluator = ExtremaPC_OffsetCurve(theCurve);
      break;

    default:
      myEvaluator = ExtremaPC_OtherCurve(theCurve);
      break;
  }
}

//==================================================================================================

void ExtremaPC_Curve::initFromGeomCurve(const Handle(Geom_Curve)& theCurve)
{
  // Try specific curve types for direct initialization
  Handle(Geom_Line) aLine = Handle(Geom_Line)::DownCast(theCurve);
  if (!aLine.IsNull())
  {
    myCurveType = GeomAbs_Line;
    myEvaluator = ExtremaPC_Line(aLine->Lin());
    return;
  }

  Handle(Geom_Circle) aCircle = Handle(Geom_Circle)::DownCast(theCurve);
  if (!aCircle.IsNull())
  {
    myCurveType = GeomAbs_Circle;
    myEvaluator = ExtremaPC_Circle(aCircle->Circ());
    return;
  }

  Handle(Geom_Ellipse) anEllipse = Handle(Geom_Ellipse)::DownCast(theCurve);
  if (!anEllipse.IsNull())
  {
    myCurveType = GeomAbs_Ellipse;
    myEvaluator = ExtremaPC_Ellipse(anEllipse->Elips());
    return;
  }

  Handle(Geom_Hyperbola) aHyperbola = Handle(Geom_Hyperbola)::DownCast(theCurve);
  if (!aHyperbola.IsNull())
  {
    myCurveType = GeomAbs_Hyperbola;
    myEvaluator = ExtremaPC_Hyperbola(aHyperbola->Hypr());
    return;
  }

  Handle(Geom_Parabola) aParabola = Handle(Geom_Parabola)::DownCast(theCurve);
  if (!aParabola.IsNull())
  {
    myCurveType = GeomAbs_Parabola;
    myEvaluator = ExtremaPC_Parabola(aParabola->Parab());
    return;
  }

  Handle(Geom_BezierCurve) aBezier = Handle(Geom_BezierCurve)::DownCast(theCurve);
  if (!aBezier.IsNull())
  {
    myCurveType = GeomAbs_BezierCurve;
    myEvaluator = ExtremaPC_BezierCurve(aBezier);
    return;
  }

  Handle(Geom_BSplineCurve) aBSpline = Handle(Geom_BSplineCurve)::DownCast(theCurve);
  if (!aBSpline.IsNull())
  {
    myCurveType = GeomAbs_BSplineCurve;
    myEvaluator = ExtremaPC_BSplineCurve(aBSpline);
    return;
  }

  // For offset and other curves, store adaptor and use it
  // Use domain bounds if set, otherwise use natural curve bounds
  if (myConfig.Domain.has_value())
  {
    myAdaptor = new GeomAdaptor_Curve(theCurve, myConfig.Domain->Min, myConfig.Domain->Max);
  }
  else
  {
    myAdaptor = new GeomAdaptor_Curve(theCurve);
  }
  myCurveType = myAdaptor->GetType();

  if (myCurveType == GeomAbs_OffsetCurve)
  {
    myEvaluator = ExtremaPC_OffsetCurve(*myAdaptor);
  }
  else
  {
    myEvaluator = ExtremaPC_OtherCurve(*myAdaptor);
  }
}

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Handle(Geom_Curve)& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  if (theCurve.IsNull())
  {
    return;
  }

  // Check for trimmed curve - if so, use bounds
  Handle(Geom_TrimmedCurve) aTrimmed = Handle(Geom_TrimmedCurve)::DownCast(theCurve);
  if (!aTrimmed.IsNull())
  {
    myConfig.Domain = ExtremaPC::Domain1D(aTrimmed->FirstParameter(), aTrimmed->LastParameter());
    initFromGeomCurve(aTrimmed->BasisCurve());
    return;
  }

  // Initialize based on curve type - without setting domain (unbounded)
  initFromGeomCurve(theCurve);
}

//==================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const Handle(Geom_Curve)& theCurve, double theUMin, double theUMax)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
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

  // Set effective domain bounds
  myConfig.Domain = ExtremaPC::Domain1D(aEffectiveUMin, aEffectiveUMax);

  // Use common helper for curve type detection and evaluator creation
  initFromGeomCurve(aBaseCurve);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_Curve::Perform(const gp_Pnt& theP) const
{
  // If domain is set in config, use bounded Perform; otherwise use unbounded
  if (myConfig.Domain.has_value())
  {
    return Perform(theP, myConfig.Domain.value());
  }

  // No domain set - use unbounded Perform on evaluator
  return std::visit(
    [&](const auto& theEval) -> ExtremaPC::Result {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        ExtremaPC::Result aResult;
        aResult.Status = ExtremaPC::Status::NotDone;
        return aResult;
      }
      else
      {
        return theEval.Perform(theP, myConfig.Tolerance, myConfig.Mode);
      }
    },
    myEvaluator);
}

//==================================================================================================

ExtremaPC::Result ExtremaPC_Curve::Perform(const gp_Pnt& theP, const ExtremaPC::Domain1D& theDomain) const
{
  return std::visit(
    [&](const auto& theEval) -> ExtremaPC::Result {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        ExtremaPC::Result aResult;
        aResult.Status = ExtremaPC::Status::NotDone;
        return aResult;
      }
      else
      {
        // Use bounded Perform with domain
        if (myConfig.IncludeEndpoints)
        {
          return theEval.PerformWithEndpoints(theP, theDomain, myConfig.Tolerance, myConfig.Mode);
        }
        else
        {
          return theEval.Perform(theP, theDomain, myConfig.Tolerance, myConfig.Mode);
        }
      }
    },
    myEvaluator);
}
