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

#include <GeomBndLib_Curve2d.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>

#include <type_traits>

//=================================================================================================

GeomBndLib_Curve2d::GeomBndLib_Curve2d(const Adaptor2d_Curve2d& theCurve)
    : myCurveType(theCurve.GetType())
{
  myAdaptorRef = &theCurve;

  switch (myCurveType)
  {
    case GeomAbs_Line: {
      occ::handle<Geom2d_Line> aLine = new Geom2d_Line(theCurve.Line());
      myEvaluator.emplace<GeomBndLib_Line2d>(aLine);
      break;
    }
    case GeomAbs_Circle: {
      occ::handle<Geom2d_Circle> aCircle = new Geom2d_Circle(theCurve.Circle());
      myEvaluator.emplace<GeomBndLib_Circle2d>(aCircle);
      break;
    }
    case GeomAbs_Ellipse: {
      occ::handle<Geom2d_Ellipse> anEllipse = new Geom2d_Ellipse(theCurve.Ellipse());
      myEvaluator.emplace<GeomBndLib_Ellipse2d>(anEllipse);
      break;
    }
    case GeomAbs_Hyperbola: {
      occ::handle<Geom2d_Hyperbola> aHyp = new Geom2d_Hyperbola(theCurve.Hyperbola());
      myEvaluator.emplace<GeomBndLib_Hyperbola2d>(aHyp);
      break;
    }
    case GeomAbs_Parabola: {
      occ::handle<Geom2d_Parabola> aPar = new Geom2d_Parabola(theCurve.Parabola());
      myEvaluator.emplace<GeomBndLib_Parabola2d>(aPar);
      break;
    }
    case GeomAbs_BezierCurve: {
      myEvaluator.emplace<GeomBndLib_BezierCurve2d>(theCurve.Bezier());
      break;
    }
    case GeomAbs_BSplineCurve: {
      myEvaluator.emplace<GeomBndLib_BSplineCurve2d>(theCurve.BSpline());
      break;
    }
    case GeomAbs_OffsetCurve: {
      const Geom2dAdaptor_Curve* aGA = dynamic_cast<const Geom2dAdaptor_Curve*>(myAdaptorRef);
      if (aGA != nullptr)
      {
        auto anOff = occ::down_cast<Geom2d_OffsetCurve>(aGA->Curve());
        if (!anOff.IsNull())
        {
          myEvaluator.emplace<GeomBndLib_OffsetCurve2d>(anOff);
          break;
        }
      }
      myCurveType = GeomAbs_OtherCurve;
      myEvaluator.emplace<GeomBndLib_OtherCurve2d>(*myAdaptorRef);
      break;
    }
    default: {
      myCurveType = GeomAbs_OtherCurve;
      myEvaluator.emplace<GeomBndLib_OtherCurve2d>(*myAdaptorRef);
      break;
    }
  }
}

//=================================================================================================

GeomBndLib_Curve2d::GeomBndLib_Curve2d(const occ::handle<Geom2d_Curve>& theCurve)
    : myCurveType(GeomAbs_OtherCurve)
{
  auto tryEmplaceEvaluator = [this](const occ::handle<Geom2d_Curve>& theDetectedCurve) {
    if (auto aLine = occ::down_cast<Geom2d_Line>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Line;
      myEvaluator.emplace<GeomBndLib_Line2d>(aLine);
      return true;
    }
    if (auto aCircle = occ::down_cast<Geom2d_Circle>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Circle;
      myEvaluator.emplace<GeomBndLib_Circle2d>(aCircle);
      return true;
    }
    if (auto anEllipse = occ::down_cast<Geom2d_Ellipse>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Ellipse;
      myEvaluator.emplace<GeomBndLib_Ellipse2d>(anEllipse);
      return true;
    }
    if (auto aHyp = occ::down_cast<Geom2d_Hyperbola>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Hyperbola;
      myEvaluator.emplace<GeomBndLib_Hyperbola2d>(aHyp);
      return true;
    }
    if (auto aPar = occ::down_cast<Geom2d_Parabola>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Parabola;
      myEvaluator.emplace<GeomBndLib_Parabola2d>(aPar);
      return true;
    }
    if (auto aBez = occ::down_cast<Geom2d_BezierCurve>(theDetectedCurve))
    {
      myCurveType = GeomAbs_BezierCurve;
      myEvaluator.emplace<GeomBndLib_BezierCurve2d>(aBez);
      return true;
    }
    if (auto aBSpl = occ::down_cast<Geom2d_BSplineCurve>(theDetectedCurve))
    {
      myCurveType = GeomAbs_BSplineCurve;
      myEvaluator.emplace<GeomBndLib_BSplineCurve2d>(aBSpl);
      return true;
    }
    if (auto anOff = occ::down_cast<Geom2d_OffsetCurve>(theDetectedCurve))
    {
      myCurveType = GeomAbs_OffsetCurve;
      myEvaluator.emplace<GeomBndLib_OffsetCurve2d>(anOff);
      return true;
    }
    return false;
  };

  occ::handle<Geom2d_Curve> aCurveForDetection = theCurve;
  if (auto aTrim = occ::down_cast<Geom2d_TrimmedCurve>(theCurve))
  {
    myAdaptorOwned      = new Geom2dAdaptor_Curve(theCurve);
    aCurveForDetection = aTrim->BasisCurve();
  }

  if (tryEmplaceEvaluator(aCurveForDetection))
  {
    return;
  }

  if (myAdaptorOwned.IsNull())
  {
    myAdaptorOwned = new Geom2dAdaptor_Curve(theCurve);
  }
  myCurveType = GeomAbs_OtherCurve;
  myEvaluator.emplace<GeomBndLib_OtherCurve2d>(*myAdaptorOwned);
}

//=================================================================================================

const Adaptor2d_Curve2d* GeomBndLib_Curve2d::adaptor() const
{
  return myAdaptorRef != nullptr ? myAdaptorRef
                                 : (myAdaptorOwned.IsNull() ? nullptr : &*myAdaptorOwned);
}

//=================================================================================================

double GeomBndLib_Curve2d::effectiveU1() const
{
  const Adaptor2d_Curve2d* anAdaptor = adaptor();
  return anAdaptor != nullptr ? anAdaptor->FirstParameter() : 0.0;
}

//=================================================================================================

double GeomBndLib_Curve2d::effectiveU2() const
{
  const Adaptor2d_Curve2d* anAdaptor = adaptor();
  return anAdaptor != nullptr ? anAdaptor->LastParameter() : 0.0;
}

//=================================================================================================

void GeomBndLib_Curve2d::Add(double theTol, Bnd_Box2d& theBox) const
{
  if (adaptor() != nullptr)
  {
    Add(effectiveU1(), effectiveU2(), theTol, theBox);
    return;
  }
  std::visit(
    [theTol, &theBox](const auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.Add(theTol, theBox);
      }
    },
    myEvaluator);
}

//=================================================================================================

void GeomBndLib_Curve2d::Add(double theU1, double theU2, double theTol, Bnd_Box2d& theBox) const
{
  std::visit(
    [theU1, theU2, theTol, &theBox](const auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.Add(theU1, theU2, theTol, theBox);
      }
    },
    myEvaluator);
}

//=================================================================================================

void GeomBndLib_Curve2d::AddOptimal(double theTol, Bnd_Box2d& theBox) const
{
  if (adaptor() != nullptr)
  {
    AddOptimal(effectiveU1(), effectiveU2(), theTol, theBox);
    return;
  }

  std::visit(
    [theTol, &theBox](const auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return;
      }
      else if constexpr (std::is_same_v<T, GeomBndLib_OtherCurve2d>)
      {
        theEval.AddOptimal(theTol, theBox);
      }
      else
      {
        theEval.AddOptimal(theEval.Geometry()->FirstParameter(),
                           theEval.Geometry()->LastParameter(),
                           theTol,
                           theBox);
      }
    },
    myEvaluator);
}

//=================================================================================================

void GeomBndLib_Curve2d::AddOptimal(double     theU1,
                                    double     theU2,
                                    double     theTol,
                                    Bnd_Box2d& theBox) const
{
  std::visit(
    [theU1, theU2, theTol, &theBox](const auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.AddOptimal(theU1, theU2, theTol, theBox);
      }
    },
    myEvaluator);
}
