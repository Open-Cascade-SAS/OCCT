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

#include <GeomBndLib_Curve.hxx>

#include <Adaptor3d_Curve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>

#include <type_traits>

//=================================================================================================

GeomBndLib_Curve::GeomBndLib_Curve(const Adaptor3d_Curve& theCurve)
    : myCurveType(theCurve.GetType())
{
  myAdaptorRef = &theCurve;

  switch (myCurveType)
  {
    case GeomAbs_Line: {
      occ::handle<Geom_Line> aLine = new Geom_Line(theCurve.Line());
      myEvaluator.emplace<GeomBndLib_Line>(aLine);
      break;
    }
    case GeomAbs_Circle: {
      occ::handle<Geom_Circle> aCircle = new Geom_Circle(theCurve.Circle());
      myEvaluator.emplace<GeomBndLib_Circle>(aCircle);
      break;
    }
    case GeomAbs_Ellipse: {
      occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(theCurve.Ellipse());
      myEvaluator.emplace<GeomBndLib_Ellipse>(anEllipse);
      break;
    }
    case GeomAbs_Hyperbola: {
      occ::handle<Geom_Hyperbola> aHyp = new Geom_Hyperbola(theCurve.Hyperbola());
      myEvaluator.emplace<GeomBndLib_Hyperbola>(aHyp);
      break;
    }
    case GeomAbs_Parabola: {
      occ::handle<Geom_Parabola> aPar = new Geom_Parabola(theCurve.Parabola());
      myEvaluator.emplace<GeomBndLib_Parabola>(aPar);
      break;
    }
    case GeomAbs_BezierCurve: {
      myEvaluator.emplace<GeomBndLib_BezierCurve>(theCurve.Bezier());
      break;
    }
    case GeomAbs_BSplineCurve: {
      myEvaluator.emplace<GeomBndLib_BSplineCurve>(theCurve.BSpline());
      break;
    }
    case GeomAbs_OffsetCurve: {
      const GeomAdaptor_Curve* aGA = dynamic_cast<const GeomAdaptor_Curve*>(myAdaptorRef);
      if (aGA != nullptr)
      {
        auto anOff = occ::down_cast<Geom_OffsetCurve>(aGA->Curve());
        if (!anOff.IsNull())
        {
          myEvaluator.emplace<GeomBndLib_OffsetCurve>(anOff);
          break;
        }
      }
      myCurveType = GeomAbs_OtherCurve;
      myEvaluator.emplace<GeomBndLib_OtherCurve>(*myAdaptorRef);
      break;
    }
    default: {
      myCurveType = GeomAbs_OtherCurve;
      myEvaluator.emplace<GeomBndLib_OtherCurve>(*myAdaptorRef);
      break;
    }
  }
}

//=================================================================================================

GeomBndLib_Curve::GeomBndLib_Curve(const occ::handle<Geom_Curve>& theCurve)
    : myCurveType(GeomAbs_OtherCurve)
{
  auto tryEmplaceEvaluator = [this](const occ::handle<Geom_Curve>& theDetectedCurve) {
    if (auto aLine = occ::down_cast<Geom_Line>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Line;
      myEvaluator.emplace<GeomBndLib_Line>(aLine);
      return true;
    }
    if (auto aCircle = occ::down_cast<Geom_Circle>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Circle;
      myEvaluator.emplace<GeomBndLib_Circle>(aCircle);
      return true;
    }
    if (auto anEllipse = occ::down_cast<Geom_Ellipse>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Ellipse;
      myEvaluator.emplace<GeomBndLib_Ellipse>(anEllipse);
      return true;
    }
    if (auto aHyp = occ::down_cast<Geom_Hyperbola>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Hyperbola;
      myEvaluator.emplace<GeomBndLib_Hyperbola>(aHyp);
      return true;
    }
    if (auto aPar = occ::down_cast<Geom_Parabola>(theDetectedCurve))
    {
      myCurveType = GeomAbs_Parabola;
      myEvaluator.emplace<GeomBndLib_Parabola>(aPar);
      return true;
    }
    if (auto aBez = occ::down_cast<Geom_BezierCurve>(theDetectedCurve))
    {
      myCurveType = GeomAbs_BezierCurve;
      myEvaluator.emplace<GeomBndLib_BezierCurve>(aBez);
      return true;
    }
    if (auto aBSpl = occ::down_cast<Geom_BSplineCurve>(theDetectedCurve))
    {
      myCurveType = GeomAbs_BSplineCurve;
      myEvaluator.emplace<GeomBndLib_BSplineCurve>(aBSpl);
      return true;
    }
    if (auto anOff = occ::down_cast<Geom_OffsetCurve>(theDetectedCurve))
    {
      myCurveType = GeomAbs_OffsetCurve;
      myEvaluator.emplace<GeomBndLib_OffsetCurve>(anOff);
      return true;
    }
    return false;
  };

  occ::handle<Geom_Curve> aCurveForDetection = theCurve;
  if (auto aTrim = occ::down_cast<Geom_TrimmedCurve>(theCurve))
  {
    myAdaptorOwned      = new GeomAdaptor_Curve(theCurve);
    aCurveForDetection = aTrim->BasisCurve();
  }

  if (tryEmplaceEvaluator(aCurveForDetection))
  {
    return;
  }

  if (myAdaptorOwned.IsNull())
  {
    myAdaptorOwned = new GeomAdaptor_Curve(theCurve);
  }
  myCurveType = GeomAbs_OtherCurve;
  myEvaluator.emplace<GeomBndLib_OtherCurve>(*myAdaptorOwned);
}

//=================================================================================================

const Adaptor3d_Curve* GeomBndLib_Curve::adaptor() const
{
  return myAdaptorRef != nullptr ? myAdaptorRef
                                 : (myAdaptorOwned.IsNull() ? nullptr : &*myAdaptorOwned);
}

//=================================================================================================

double GeomBndLib_Curve::effectiveU1() const
{
  const Adaptor3d_Curve* anAdaptor = adaptor();
  return anAdaptor != nullptr ? anAdaptor->FirstParameter() : 0.0;
}

//=================================================================================================

double GeomBndLib_Curve::effectiveU2() const
{
  const Adaptor3d_Curve* anAdaptor = adaptor();
  return anAdaptor != nullptr ? anAdaptor->LastParameter() : 0.0;
}

//=================================================================================================

void GeomBndLib_Curve::Add(double theTol, Bnd_Box& theBox) const
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

void GeomBndLib_Curve::Add(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
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

void GeomBndLib_Curve::AddOptimal(double theTol, Bnd_Box& theBox) const
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
      else if constexpr (std::is_same_v<T, GeomBndLib_OtherCurve>)
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

void GeomBndLib_Curve::AddOptimal(double theU1, double theU2, double theTol, Bnd_Box& theBox) const
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
