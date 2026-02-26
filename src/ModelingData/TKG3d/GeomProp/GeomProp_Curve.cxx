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

#include <GeomProp_Curve.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Precision.hxx>

//==================================================================================================

GeomProp_Curve::GeomProp_Curve(const Adaptor3d_Curve& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve);
}

//==================================================================================================

GeomProp_Curve::GeomProp_Curve(const occ::handle<Geom_Curve>& theCurve)
    : myEvaluator(std::monostate{}),
      myCurveType(GeomAbs_OtherCurve)
{
  initialization(theCurve);
}

//==================================================================================================

void GeomProp_Curve::initialization(const Adaptor3d_Curve& theCurve)
{
  if (theCurve.IsKind(STANDARD_TYPE(GeomAdaptor_Curve)))
  {
    const auto& aGeomAdaptor = static_cast<const GeomAdaptor_Curve&>(theCurve);
    myAdaptor                = new GeomAdaptor_Curve(aGeomAdaptor);
    initFromAdaptor();
    return;
  }

  // For non-GeomAdaptor, set uninitialized.
  myAdaptor.Nullify();
  myCurveType = theCurve.GetType();
  myEvaluator.emplace<std::monostate>();
}

//==================================================================================================

void GeomProp_Curve::initialization(const occ::handle<Geom_Curve>& theCurve)
{
  if (theCurve.IsNull())
  {
    myAdaptor.Nullify();
    myEvaluator.emplace<std::monostate>();
    myCurveType = GeomAbs_OtherCurve;
    return;
  }

  myAdaptor = new GeomAdaptor_Curve(theCurve);
  initFromAdaptor();
}

//==================================================================================================

void GeomProp_Curve::initFromAdaptor()
{
  myCurveType                   = myAdaptor->GetType();
  const GeomAdaptor_Curve* aPtr = myAdaptor.get();

  switch (myCurveType)
  {
    case GeomAbs_Line:
      myEvaluator.emplace<GeomProp_Line>(aPtr);
      break;
    case GeomAbs_Circle:
      myEvaluator.emplace<GeomProp_Circle>(aPtr);
      break;
    case GeomAbs_Ellipse:
      myEvaluator.emplace<GeomProp_Ellipse>(aPtr);
      break;
    case GeomAbs_Hyperbola:
      myEvaluator.emplace<GeomProp_Hyperbola>(aPtr);
      break;
    case GeomAbs_Parabola:
      myEvaluator.emplace<GeomProp_Parabola>(aPtr);
      break;
    case GeomAbs_BezierCurve:
      myEvaluator.emplace<GeomProp_BezierCurve>(aPtr);
      break;
    case GeomAbs_BSplineCurve:
      myEvaluator.emplace<GeomProp_BSplineCurve>(aPtr);
      break;
    case GeomAbs_OffsetCurve:
      myEvaluator.emplace<GeomProp_OffsetCurve>(aPtr);
      break;
    default:
      myEvaluator.emplace<GeomProp_OtherCurve>(aPtr);
      break;
  }
}

//==================================================================================================

const GeomAdaptor_Curve* GeomProp_Curve::Adaptor() const
{
  return std::visit(
    [](const auto& theEval) -> const GeomAdaptor_Curve* {
      using T = std::decay_t<decltype(theEval)>;
      if constexpr (std::is_same_v<T, std::monostate>)
        return nullptr;
      else
        return theEval.Adaptor();
    },
    myEvaluator);
}

//==================================================================================================

GeomProp::TangentResult GeomProp_Curve::Tangent(const double theParam, const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> GeomProp::TangentResult {
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

//==================================================================================================

GeomProp::CurvatureResult GeomProp_Curve::Curvature(const double theParam,
                                                    const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> GeomProp::CurvatureResult {
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

//==================================================================================================

GeomProp::NormalResult GeomProp_Curve::Normal(const double theParam, const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> GeomProp::NormalResult {
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

//==================================================================================================

GeomProp::CentreResult GeomProp_Curve::CentreOfCurvature(const double theParam,
                                                         const double theTol) const
{
  return std::visit(
    [theParam, theTol](const auto& theEval) -> GeomProp::CentreResult {
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

//==================================================================================================

GeomProp::CurveAnalysis GeomProp_Curve::FindCurvatureExtrema() const
{
  return std::visit(
    [](const auto& theEval) -> GeomProp::CurveAnalysis {
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

//==================================================================================================

GeomProp::CurveAnalysis GeomProp_Curve::FindInflections() const
{
  return std::visit(
    [](const auto& theEval) -> GeomProp::CurveAnalysis {
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

//==================================================================================================

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

//==================================================================================================

GeomAbs_Shape GeomProp_Curve::Continuity(const occ::handle<Geom_Curve>& theC1,
                                         const occ::handle<Geom_Curve>& theC2,
                                         const double                   theU1,
                                         const double                   theU2,
                                         const bool                     theR1,
                                         const bool                     theR2,
                                         const double                   theTolLinear,
                                         const double                   theTolAngular)
{
  GeomAbs_Shape aCont     = GeomAbs_C0;
  bool          isChecked = false;

  // Determine continuity order of each curve.
  int aCont1 = geomAbsToInteger(theC1->Continuity());
  int aCont2 = geomAbsToInteger(theC2->Continuity());

  // Unwrap TrimmedCurve to basis curve for BSpline knot analysis.
  occ::handle<Geom_Curve> aCurve1 = theC1;
  occ::handle<Geom_Curve> aCurve2 = theC2;
  if (aCurve1->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    aCurve1 = occ::down_cast<Geom_TrimmedCurve>(aCurve1)->BasisCurve();
  }
  if (aCurve2->IsKind(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    aCurve2 = occ::down_cast<Geom_TrimmedCurve>(aCurve2)->BasisCurve();
  }

  // For BSpline curves, refine continuity from knot multiplicity.
  if (aCurve1->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    const occ::handle<Geom_BSplineCurve> aBSpl      = occ::down_cast<Geom_BSplineCurve>(aCurve1);
    double                               aTolerance = 0.0;
    int                                  anIdx1 = 0, anIdx2 = 0;
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
  if (aCurve2->IsKind(STANDARD_TYPE(Geom_BSplineCurve)))
  {
    const occ::handle<Geom_BSplineCurve> aBSpl      = occ::down_cast<Geom_BSplineCurve>(aCurve2);
    double                               aTolerance = 0.0;
    int                                  anIdx1 = 0, anIdx2 = 0;
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
  GeomAdaptor_Curve anAdapt1(theC1);
  GeomAdaptor_Curve anAdapt2(theC2);

  // Check point coincidence using D0 only.
  gp_Pnt aPnt1 = anAdapt1.Value(theU1);
  gp_Pnt aPnt2 = anAdapt2.Value(theU2);
  if (!aPnt1.IsEqual(aPnt2, theTolLinear))
  {
    throw Standard_Failure("Curves not connected");
  }

  const int aMinOrder = std::min(aN1, aN2);
  if (aMinOrder >= 1)
  {
    gp_Pnt aDummyPnt;
    gp_Vec aD1_1, aD1_2;
    anAdapt1.D1(theU1, aDummyPnt, aD1_1);
    anAdapt2.D1(theU2, aDummyPnt, aD1_2);
    gp_Vec aVec1 = aD1_1;
    gp_Vec aVec2 = aD1_2;
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
      GeomProp_Curve                aProp1(theC1);
      GeomProp_Curve                aProp2(theC2);
      const GeomProp::TangentResult aTan1 = aProp1.Tangent(theU1, theTolLinear);
      const GeomProp::TangentResult aTan2 = aProp2.Tangent(theU2, theTolLinear);
      if (aTan1.IsDefined && aTan2.IsDefined)
      {
        gp_Dir aDir1 = aTan1.Direction;
        gp_Dir aDir2 = aTan2.Direction;
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
    gp_Pnt aDummy;
    gp_Vec aDummy1, aDummy2;
    gp_Vec aD2_1, aD2_2;
    anAdapt1.D2(theU1, aDummy, aDummy1, aD2_1);
    anAdapt2.D2(theU2, aDummy, aDummy2, aD2_2);
    if (aD2_1.IsEqual(aD2_2, theTolLinear, theTolAngular))
    {
      aCont = GeomAbs_C2;
    }
  }

  return aCont;
}

//==================================================================================================

GeomAbs_Shape GeomProp_Curve::Continuity(const occ::handle<Geom_Curve>& theC1,
                                         const occ::handle<Geom_Curve>& theC2,
                                         const double                   theU1,
                                         const double                   theU2,
                                         const bool                     theR1,
                                         const bool                     theR2)
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
