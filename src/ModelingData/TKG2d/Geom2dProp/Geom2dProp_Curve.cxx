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

#include <Geom2dProp_Curve.hxx>

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
#include <Precision.hxx>

//=================================================================================================

Geom2dProp_Curve::Geom2dProp_Curve(const Adaptor2d_Curve2d&          theCurve,
                                   const Geom2dProp::CurveDerivOrder theOrder)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve, theOrder);
}

//=================================================================================================

Geom2dProp_Curve::Geom2dProp_Curve(const occ::handle<Geom2d_Curve>&  theCurve,
                                   const Geom2dProp::CurveDerivOrder theOrder)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve, theOrder);
}

//=================================================================================================

void Geom2dProp_Curve::initialization(const Adaptor2d_Curve2d&          theCurve,
                                      const Geom2dProp::CurveDerivOrder theOrder)
{
  if (theCurve.IsKind(STANDARD_TYPE(Geom2dAdaptor_Curve)))
  {
    const Geom2dAdaptor_Curve& aGeomAdaptor = static_cast<const Geom2dAdaptor_Curve&>(theCurve);
    if (!aGeomAdaptor.Curve().IsNull())
    {
      // Dispatch specialized evaluators directly from the GeomAdaptor.
      myOwnedAdaptor.Nullify();
      myCurveType = aGeomAdaptor.GetType();
      switch (myCurveType)
      {
        case GeomAbs_Line:
          myEvaluator.emplace<Geom2dProp_Line>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_Circle:
          myEvaluator.emplace<Geom2dProp_Circle>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_Ellipse:
          myEvaluator.emplace<Geom2dProp_Ellipse>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_Hyperbola:
          myEvaluator.emplace<Geom2dProp_Hyperbola>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_Parabola:
          myEvaluator.emplace<Geom2dProp_Parabola>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_BezierCurve:
          myEvaluator.emplace<Geom2dProp_BezierCurve>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_BSplineCurve:
          myEvaluator.emplace<Geom2dProp_BSplineCurve>(&aGeomAdaptor, theOrder);
          break;
        case GeomAbs_OffsetCurve:
          myEvaluator.emplace<Geom2dProp_OffsetCurve>(&aGeomAdaptor, theOrder);
          break;
        default:
          myEvaluator.emplace<Geom2dProp_OtherCurve>(&theCurve, theOrder);
          break;
      }
      return;
    }
  }

  // Non-Geom adaptor or empty curve handle: use OtherCurve with adaptor pointer.
  myOwnedAdaptor.Nullify();
  myCurveType = theCurve.GetType();
  myEvaluator.emplace<Geom2dProp_OtherCurve>(&theCurve, theOrder);
}

//=================================================================================================

void Geom2dProp_Curve::initialization(const occ::handle<Geom2d_Curve>&  theCurve,
                                      const Geom2dProp::CurveDerivOrder theOrder)
{
  if (theCurve.IsNull())
  {
    myOwnedAdaptor.Nullify();
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
    return;
  }

  // No adaptor creation for the handle path.
  myOwnedAdaptor.Nullify();

  // Unwrap TrimmedCurve to basis curve + optional CurveDomain.
  occ::handle<Geom2d_Curve>              aBasis = theCurve;
  std::optional<Geom2dProp::CurveDomain> aDomain;
  if (aBasis->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    aDomain = Geom2dProp::CurveDomain{theCurve->FirstParameter(), theCurve->LastParameter()};
    while (aBasis->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
    {
      aBasis = occ::down_cast<Geom2d_TrimmedCurve>(aBasis)->BasisCurve();
    }
  }

  // Type detection using DynamicType() - extracted once to avoid repeated virtual calls.
  const occ::handle<Standard_Type>& aType = aBasis->DynamicType();
  if (aType == STANDARD_TYPE(Geom2d_Line))
  {
    myCurveType = GeomAbs_Line;
    myEvaluator.emplace<Geom2dProp_Line>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_Circle))
  {
    myCurveType = GeomAbs_Circle;
    myEvaluator.emplace<Geom2dProp_Circle>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_Ellipse))
  {
    myCurveType = GeomAbs_Ellipse;
    myEvaluator.emplace<Geom2dProp_Ellipse>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_Hyperbola))
  {
    myCurveType = GeomAbs_Hyperbola;
    myEvaluator.emplace<Geom2dProp_Hyperbola>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_Parabola))
  {
    myCurveType = GeomAbs_Parabola;
    myEvaluator.emplace<Geom2dProp_Parabola>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_BezierCurve))
  {
    myCurveType = GeomAbs_BezierCurve;
    myEvaluator.emplace<Geom2dProp_BezierCurve>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_BSplineCurve))
  {
    myCurveType = GeomAbs_BSplineCurve;
    myEvaluator.emplace<Geom2dProp_BSplineCurve>(aBasis, aDomain, theOrder);
  }
  else if (aType == STANDARD_TYPE(Geom2d_OffsetCurve))
  {
    myCurveType = GeomAbs_OffsetCurve;
    myEvaluator.emplace<Geom2dProp_OffsetCurve>(aBasis, aDomain, theOrder);
  }
  else
  {
    myCurveType = GeomAbs_OtherCurve;
    myEvaluator.emplace<Geom2dProp_OtherCurve>(aBasis, aDomain, theOrder);
  }
}

//=================================================================================================

void Geom2dProp_Curve::SetDerivOrder(const Geom2dProp::CurveDerivOrder theOrder)
{
  std::visit(
    [theOrder](auto& theEval) {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (!std::is_same_v<T, std::monostate>)
      {
        theEval.SetDerivOrder(theOrder);
      }
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::CurveDerivOrder Geom2dProp_Curve::DerivOrder() const
{
  return std::visit(
    [](const auto& theEval) -> Geom2dProp::CurveDerivOrder {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return Geom2dProp::CurveDerivOrder::Undefined;
      }
      else
      {
        return theEval.DerivOrder();
      }
    },
    myEvaluator);
}

//=================================================================================================

const Adaptor2d_Curve2d* Geom2dProp_Curve::Adaptor() const
{
  if (!myOwnedAdaptor.IsNull())
  {
    return myOwnedAdaptor.get();
  }
  return nullptr;
}

//=================================================================================================

const Geom2d_Curve* Geom2dProp_Curve::Geometry() const
{
  return std::visit(
    [](const auto& theEval) -> const Geom2d_Curve* {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return nullptr;
      }
      else
      {
        return theEval.Geometry();
      }
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::TangentResult Geom2dProp_Curve::Tangent(const double theParam,
                                                    const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> Geom2dProp::TangentResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.Tangent(theParam, theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::CurvatureResult Geom2dProp_Curve::Curvature(const double theParam,
                                                        const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> Geom2dProp::CurvatureResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {0.0, false, false};
      }
      else
      {
        return theEval.Curvature(theParam, theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::NormalResult Geom2dProp_Curve::Normal(const double theParam, const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> Geom2dProp::NormalResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.Normal(theParam, theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::CentreResult Geom2dProp_Curve::CentreOfCurvature(const double theParam,
                                                             const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> Geom2dProp::CentreResult {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.CentreOfCurvature(theParam, theTol);
      }
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_Curve::FindCurvatureExtrema() const
{
  return std::visit(
    [](const auto& theEval) -> Geom2dProp::CurveAnalysis {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.FindCurvatureExtrema();
      }
    },
    myEvaluator);
}

//=================================================================================================

Geom2dProp::CurveAnalysis Geom2dProp_Curve::FindInflections() const
{
  return std::visit(
    [](const auto& theEval) -> Geom2dProp::CurveAnalysis {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
      {
        return {{}, false};
      }
      else
      {
        return theEval.FindInflections();
      }
    },
    myEvaluator);
}

//=================================================================================================

// Helper to convert GeomAbs_Shape to integer for comparison.
static int geomAbsToInteger(const GeomAbs_Shape theCont)
{
  switch (theCont)
  {
    case GeomAbs_C0:
      return 0;
    case GeomAbs_G1:
      return 1;
    case GeomAbs_C1:
      return 2;
    case GeomAbs_G2:
      return 3;
    case GeomAbs_C2:
      return 4;
    case GeomAbs_C3:
      return 5;
    case GeomAbs_CN:
      return 6;
  }
  return 0;
}

//=================================================================================================

GeomAbs_Shape Geom2dProp_Curve::Continuity(const occ::handle<Geom2d_Curve>& theC1,
                                            const occ::handle<Geom2d_Curve>& theC2,
                                            const double                     theU1,
                                            const double                     theU2,
                                            const bool                       theR1,
                                            const bool                       theR2,
                                            const double                     theTolLinear,
                                            const double                     theTolAngular)
{
  GeomAbs_Shape aCont     = GeomAbs_C0;
  bool          isChecked = false;

  // Determine continuity order of each curve.
  int aCont1 = geomAbsToInteger(theC1->Continuity());
  int aCont2 = geomAbsToInteger(theC2->Continuity());

  // Unwrap TrimmedCurve to basis curve for BSpline knot analysis.
  occ::handle<Geom2d_Curve> aCurve1 = theC1;
  occ::handle<Geom2d_Curve> aCurve2 = theC2;
  if (aCurve1->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    aCurve1 = occ::down_cast<Geom2d_TrimmedCurve>(aCurve1)->BasisCurve();
  }
  if (aCurve2->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    aCurve2 = occ::down_cast<Geom2d_TrimmedCurve>(aCurve2)->BasisCurve();
  }

  // For BSpline curves, refine continuity from knot multiplicity.
  if (aCurve1->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    const occ::handle<Geom2d_BSplineCurve> aBSpl = occ::down_cast<Geom2d_BSplineCurve>(aCurve1);
    double                                 aTolerance = 0.0;
    int                                    anIdx1 = 0, anIdx2 = 0;
    aBSpl->Resolution(theTolLinear, aTolerance);
    aBSpl->LocateU(theU1, aTolerance, anIdx1, anIdx2);
    if (anIdx1 > 1 && anIdx2 < aBSpl->NbKnots() && anIdx1 == anIdx2)
    {
      aCont1 = aBSpl->Degree() - aBSpl->Multiplicity(anIdx1);
    }
    else
    {
      aCont1 = 5;
    }
  }
  if (aCurve2->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    const occ::handle<Geom2d_BSplineCurve> aBSpl = occ::down_cast<Geom2d_BSplineCurve>(aCurve2);
    double                                 aTolerance = 0.0;
    int                                    anIdx1 = 0, anIdx2 = 0;
    aBSpl->Resolution(theTolLinear, aTolerance);
    aBSpl->LocateU(theU2, aTolerance, anIdx1, anIdx2);
    if (anIdx1 > 1 && anIdx2 < aBSpl->NbKnots() && anIdx1 == anIdx2)
    {
      aCont2 = aBSpl->Degree() - aBSpl->Multiplicity(anIdx1);
    }
    else
    {
      aCont2 = 5;
    }
  }

  // Determine maximum derivative order to evaluate.
  int aN1 = 0, aN2 = 0;
  if (aCont1 >= 5)
    aN1 = 3;
  else if (aCont1 == 4)
    aN1 = 2;
  else if (aCont1 >= 2)
    aN1 = 1;
  if (aCont2 >= 5)
    aN2 = 3;
  else if (aCont2 == 4)
    aN2 = 2;
  else if (aCont2 >= 2)
    aN2 = 1;

  // Evaluate derivatives at junction points.
  Geom2dAdaptor_Curve anAdapt1(theC1);
  Geom2dAdaptor_Curve anAdapt2(theC2);

  // Check point coincidence using D0 only.
  gp_Pnt2d aPnt1 = anAdapt1.Value(theU1);
  gp_Pnt2d aPnt2 = anAdapt2.Value(theU2);
  if (!aPnt1.IsEqual(aPnt2, theTolLinear))
  {
    throw Standard_Failure("Curves not connected");
  }

  const int aMinOrder = std::min(aN1, aN2);
  if (aMinOrder >= 1)
  {
    gp_Pnt2d aDummyPnt;
    gp_Vec2d aD1_1, aD1_2;
    anAdapt1.D1(theU1, aDummyPnt, aD1_1);
    anAdapt2.D1(theU2, aDummyPnt, aD1_2);
    gp_Vec2d aVec1 = aD1_1;
    gp_Vec2d aVec2 = aD1_2;
    if (theR1)
      aVec1.Reverse();
    if (theR2)
      aVec2.Reverse();

    if (aVec1.IsEqual(aVec2, theTolLinear, theTolAngular))
    {
      aCont = GeomAbs_C1;
    }
    else
    {
      // Check G1 continuity via tangent directions.
      Geom2dProp_Curve                aProp1(theC1);
      Geom2dProp_Curve                aProp2(theC2);
      const Geom2dProp::TangentResult aTan1 = aProp1.Tangent(theU1, theTolLinear);
      const Geom2dProp::TangentResult aTan2 = aProp2.Tangent(theU2, theTolLinear);
      if (aTan1.IsDefined && aTan2.IsDefined)
      {
        gp_Dir2d aDir1 = aTan1.Direction;
        gp_Dir2d aDir2 = aTan2.Direction;
        if (theR1)
          aDir1.Reverse();
        if (theR2)
          aDir2.Reverse();
        if (aDir1.IsEqual(aDir2, theTolAngular))
        {
          aCont = GeomAbs_G1;
        }
      }
      isChecked = true;
    }
  }

  if (aMinOrder >= 2 && !isChecked)
  {
    // Compare second derivatives.
    gp_Pnt2d aDummy;
    gp_Vec2d aDummy1, aDummy2;
    gp_Vec2d aD2_1, aD2_2;
    anAdapt1.D2(theU1, aDummy, aDummy1, aD2_1);
    anAdapt2.D2(theU2, aDummy, aDummy2, aD2_2);
    if (aD2_1.IsEqual(aD2_2, theTolLinear, theTolAngular))
    {
      aCont = GeomAbs_C2;
    }
  }

  return aCont;
}

//=================================================================================================

GeomAbs_Shape Geom2dProp_Curve::Continuity(const occ::handle<Geom2d_Curve>& theC1,
                                            const occ::handle<Geom2d_Curve>& theC2,
                                            const double                     theU1,
                                            const double                     theU2,
                                            const bool                       theR1,
                                            const bool                       theR2)
{
  return Continuity(theC1,
                    theC2,
                    theU1,
                    theU2,
                    theR1,
                    theR2,
                    Precision::Confusion(),
                    Precision::Angular());
}
