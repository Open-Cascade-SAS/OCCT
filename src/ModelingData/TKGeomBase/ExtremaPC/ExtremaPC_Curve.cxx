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
#include <GeomAdaptor_TransformedCurve.hxx>

namespace
{
//! Static result for uninitialized evaluator case
static ExtremaPC::Result THE_NOT_DONE_RESULT = [] {
  ExtremaPC::Result aResult;
  aResult.Status = ExtremaPC::Status::NotDone;
  return aResult;
}();
} // namespace

//=================================================================================================

void ExtremaPC_Curve::initFromAdaptor(const GeomAdaptor_Curve&   theCurve,
                                      const ExtremaPC::Domain1D& theDomain)
{
  myAdaptorRef = &theCurve;

  const GeomAbs_CurveType aCurveType = theCurve.GetType();

  switch (aCurveType)
  {
    case GeomAbs_Line:
      myEvaluator = ExtremaPC_Line(theCurve.Line(), theDomain);
      break;

    case GeomAbs_Circle:
      myEvaluator = ExtremaPC_Circle(theCurve.Circle(), theDomain);
      break;

    case GeomAbs_Ellipse:
      myEvaluator = ExtremaPC_Ellipse(theCurve.Ellipse(), theDomain);
      break;

    case GeomAbs_Hyperbola:
      myEvaluator = ExtremaPC_Hyperbola(theCurve.Hyperbola(), theDomain);
      break;

    case GeomAbs_Parabola:
      myEvaluator = ExtremaPC_Parabola(theCurve.Parabola(), theDomain);
      break;

    case GeomAbs_BezierCurve:
      myEvaluator = ExtremaPC_BezierCurve(theCurve.Bezier(), theDomain);
      break;

    case GeomAbs_BSplineCurve:
      myEvaluator = ExtremaPC_BSplineCurve(theCurve.BSpline(), theDomain);
      break;

    case GeomAbs_OffsetCurve:
      myEvaluator = ExtremaPC_OffsetCurve(theCurve, theDomain);
      break;

    default:
      myEvaluator = ExtremaPC_OtherCurve(theCurve, theDomain);
      break;
  }
}

//=================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const GeomAdaptor_Curve& theCurve)
    : myEvaluator(std::monostate{})
{
  ExtremaPC::Domain1D aDomain(theCurve.FirstParameter(), theCurve.LastParameter());
  initFromAdaptor(theCurve, aDomain);
}

//=================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const GeomAdaptor_Curve& theCurve, double theUMin, double theUMax)
    : myEvaluator(std::monostate{})
{
  ExtremaPC::Domain1D aDomain(theUMin, theUMax);
  initFromAdaptor(theCurve, aDomain);
}

//=================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const GeomAdaptor_TransformedCurve& theCurve)
    : myEvaluator(std::monostate{})
{
  initFromTransformedCurve(theCurve, std::nullopt);
}

//=================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const GeomAdaptor_TransformedCurve& theCurve,
                                 double                              theUMin,
                                 double                              theUMax)
    : myEvaluator(std::monostate{})
{
  ExtremaPC::Domain1D aDomain(theUMin, theUMax);
  initFromTransformedCurve(theCurve, aDomain);
}

//=================================================================================================

void ExtremaPC_Curve::initFromTransformedCurve(const GeomAdaptor_TransformedCurve&       theCurve,
                                               const std::optional<ExtremaPC::Domain1D>& theDomain)
{
  // Identity transform: delegate to existing GeomAdaptor_Curve path
  if (theCurve.Trsf().Form() == gp_Identity)
  {
    const GeomAdaptor_Curve& aCurve = theCurve.Curve();
    ExtremaPC::Domain1D      aDomain =
      theDomain.has_value() ? theDomain.value()
                                 : ExtremaPC::Domain1D(aCurve.FirstParameter(), aCurve.LastParameter());
    initFromAdaptor(aCurve, aDomain);
    return;
  }

  // CurveOnSurface: use OtherCurve path with non-owning pointer
  if (theCurve.IsCurveOnSurface())
  {
    myAdaptorRef = &theCurve;
    ExtremaPC::Domain1D aDomain =
      theDomain.has_value()
        ? theDomain.value()
        : ExtremaPC::Domain1D(theCurve.FirstParameter(), theCurve.LastParameter());
    myEvaluator = ExtremaPC_OtherCurve(*myAdaptorRef, aDomain);
    return;
  }

  ExtremaPC::Domain1D aDomain =
    theDomain.has_value()
      ? theDomain.value()
      : ExtremaPC::Domain1D(theCurve.FirstParameter(), theCurve.LastParameter());

  const GeomAbs_CurveType aCurveType = theCurve.GetType();

  switch (aCurveType)
  {
    // Elementary curves: Line(), Circle() etc. return already-transformed primitives
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

    // BSpline/Bezier: use untransformed handle, set up inverse-transform path
    case GeomAbs_BSplineCurve:
      myEvaluator = ExtremaPC_BSplineCurve(theCurve.Curve().BSpline(), aDomain);
      myTrsf      = theCurve.Trsf();
      break;

    case GeomAbs_BezierCurve:
      myEvaluator = ExtremaPC_BezierCurve(theCurve.Curve().Bezier(), aDomain);
      myTrsf      = theCurve.Trsf();
      break;

    // Offset/Other: use non-owning pointer to caller's TransformedCurve
    case GeomAbs_OffsetCurve:
      myAdaptorRef = &theCurve;
      myEvaluator  = ExtremaPC_OffsetCurve(*myAdaptorRef, aDomain);
      break;

    default:
      myAdaptorRef = &theCurve;
      myEvaluator  = ExtremaPC_OtherCurve(*myAdaptorRef, aDomain);
      break;
  }
}

//=================================================================================================

void ExtremaPC_Curve::initFromGeomCurve(const occ::handle<Geom_Curve>&            theCurve,
                                        const std::optional<ExtremaPC::Domain1D>& theDomain)
{
  // Try specific curve types for direct initialization
  occ::handle<Geom_Line> aLine = occ::down_cast<Geom_Line>(theCurve);
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

  occ::handle<Geom_Circle> aCircle = occ::down_cast<Geom_Circle>(theCurve);
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

  occ::handle<Geom_Ellipse> anEllipse = occ::down_cast<Geom_Ellipse>(theCurve);
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

  occ::handle<Geom_Hyperbola> aHyperbola = occ::down_cast<Geom_Hyperbola>(theCurve);
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

  occ::handle<Geom_Parabola> aParabola = occ::down_cast<Geom_Parabola>(theCurve);
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

  occ::handle<Geom_BezierCurve> aBezier = occ::down_cast<Geom_BezierCurve>(theCurve);
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

  occ::handle<Geom_BSplineCurve> aBSpline = occ::down_cast<Geom_BSplineCurve>(theCurve);
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
    myAdaptorOwned = new GeomAdaptor_Curve(theCurve, theDomain->Min, theDomain->Max);
  }
  else
  {
    myAdaptorOwned = new GeomAdaptor_Curve(theCurve);
  }
  const GeomAbs_CurveType aCurveType = myAdaptorOwned->GetType();

  if (aCurveType == GeomAbs_OffsetCurve)
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_OffsetCurve(*myAdaptorOwned, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_OffsetCurve(*myAdaptorOwned);
    }
  }
  else
  {
    if (theDomain.has_value())
    {
      myEvaluator = ExtremaPC_OtherCurve(*myAdaptorOwned, theDomain.value());
    }
    else
    {
      myEvaluator = ExtremaPC_OtherCurve(*myAdaptorOwned);
    }
  }
}

//=================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const occ::handle<Geom_Curve>& theCurve)
    : myEvaluator(std::monostate{})
{
  if (theCurve.IsNull())
  {
    return;
  }

  // Check for trimmed curve - if so, use bounds
  occ::handle<Geom_TrimmedCurve> aTrimmed = occ::down_cast<Geom_TrimmedCurve>(theCurve);
  if (!aTrimmed.IsNull())
  {
    ExtremaPC::Domain1D aDomain(aTrimmed->FirstParameter(), aTrimmed->LastParameter());
    initFromGeomCurve(aTrimmed->BasisCurve(), aDomain);
    return;
  }

  // Initialize based on curve type - without setting domain (unbounded)
  initFromGeomCurve(theCurve, std::nullopt);
}

//=================================================================================================

ExtremaPC_Curve::ExtremaPC_Curve(const occ::handle<Geom_Curve>& theCurve,
                                 double                         theUMin,
                                 double                         theUMax)
    : myEvaluator(std::monostate{})
{
  if (theCurve.IsNull())
  {
    return;
  }

  // Get base curve and effective bounds
  occ::handle<Geom_Curve> aBaseCurve     = theCurve;
  double                  aEffectiveUMin = theUMin;
  double                  aEffectiveUMax = theUMax;

  // For trimmed curve, intersect input bounds with trimmed bounds
  occ::handle<Geom_TrimmedCurve> aTrimmed = occ::down_cast<Geom_TrimmedCurve>(theCurve);
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

//=================================================================================================

const ExtremaPC::Result& ExtremaPC_Curve::postProcessTransform(
  const ExtremaPC::Result& theSrc) const
{
  const double aSF   = myTrsf.ScaleFactor();
  const double aSFSq = aSF * aSF;

  myTransformResult.Clear();
  myTransformResult.Status                 = theSrc.Status;
  myTransformResult.InfiniteSquareDistance = theSrc.InfiniteSquareDistance * aSFSq;
  for (int i = 0; i < theSrc.Extrema.Length(); ++i)
  {
    ExtremaPC::ExtremumResult anExt;
    anExt.Parameter      = theSrc[i].Parameter;
    anExt.Point          = theSrc[i].Point.Transformed(myTrsf);
    anExt.SquareDistance = theSrc[i].SquareDistance * aSFSq;
    anExt.IsMinimum      = theSrc[i].IsMinimum;
    myTransformResult.Extrema.Append(anExt);
  }
  return myTransformResult;
}

//=================================================================================================

const ExtremaPC::Result& ExtremaPC_Curve::Perform(const gp_Pnt&         theP,
                                                  double                theTol,
                                                  ExtremaPC::SearchMode theMode) const
{
  const ExtremaPC::Result* aResultPtr = &THE_NOT_DONE_RESULT;

  if (myTrsf.Form() != gp_Identity)
  {
    // Inverse-transform query point and delegate to untransformed evaluator
    gp_Pnt aPInv = theP.Transformed(myTrsf.Inverted());
    std::visit(
      [&](auto& theEval) {
        using T = std::decay_t<decltype(theEval)>;
        if constexpr (!std::is_same_v<T, std::monostate>)
        {
          aResultPtr = &theEval.Perform(aPInv, theTol, theMode);
        }
      },
      myEvaluator);
    return postProcessTransform(*aResultPtr);
  }

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

const ExtremaPC::Result& ExtremaPC_Curve::PerformWithEndpoints(const gp_Pnt&         theP,
                                                               double                theTol,
                                                               ExtremaPC::SearchMode theMode) const
{
  const ExtremaPC::Result* aResultPtr = &THE_NOT_DONE_RESULT;

  if (myTrsf.Form() != gp_Identity)
  {
    // Inverse-transform query point and delegate to untransformed evaluator
    gp_Pnt aPInv = theP.Transformed(myTrsf.Inverted());
    std::visit(
      [&](auto& theEval) {
        using T = std::decay_t<decltype(theEval)>;
        if constexpr (!std::is_same_v<T, std::monostate>)
        {
          aResultPtr = &theEval.PerformWithEndpoints(aPInv, theTol, theMode);
        }
      },
      myEvaluator);
    return postProcessTransform(*aResultPtr);
  }

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
