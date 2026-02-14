// Created on: 1993-06-04
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// 20/02/97 : PMN -> Positionement local sur BSpline (PRO6902)
// 10/07/97 : PMN -> Pas de calcul de resolution dans Nb(Intervals) (PRO9248)
// 20/10/97 : JPI -> traitement des offset curves

#define No_Standard_RangeError
#define No_Standard_OutOfRange

#include <Geom2dAdaptor_Curve.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <BSplCLib_Cache.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_OffsetCurveUtils.pxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <Geom2d_UndefinedValue.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

// #include <Geom2dConvert_BSplineCurveKnotSplitting.hxx>
static const double PosTol = Precision::PConfusion() / 2;

IMPLEMENT_STANDARD_RTTIEXT(Geom2dAdaptor_Curve, Adaptor2d_Curve2d)

namespace
{
bool hasEvalRep(const Geom2dAdaptor_Curve::CurveDataVariant& theData)
{
  if (const Geom2dAdaptor_Curve::BezierData* aBezierData =
        std::get_if<Geom2dAdaptor_Curve::BezierData>(&theData))
  {
    return !aBezierData->EvalRep.IsNull();
  }
  if (const Geom2dAdaptor_Curve::BSplineData* aBSplineData =
        std::get_if<Geom2dAdaptor_Curve::BSplineData>(&theData))
  {
    return !aBSplineData->EvalRep.IsNull();
  }
  if (const Geom2dAdaptor_Curve::OffsetData* anOffsetData =
        std::get_if<Geom2dAdaptor_Curve::OffsetData>(&theData))
  {
    return !anOffsetData->EvalRep.IsNull();
  }
  return false;
}
} // namespace

//=================================================================================================

occ::handle<Adaptor2d_Curve2d> Geom2dAdaptor_Curve::ShallowCopy() const
{
  occ::handle<Geom2dAdaptor_Curve> aCopy = new Geom2dAdaptor_Curve();

  aCopy->myCurve     = myCurve;
  aCopy->myTypeCurve = myTypeCurve;
  aCopy->myFirst     = myFirst;
  aCopy->myLast      = myLast;

  // Copy curve-specific data based on variant type
  if (const auto* anOffsetData = std::get_if<OffsetData>(&myCurveData))
  {
    OffsetData aNewData;
    if (!anOffsetData->BasisAdaptor.IsNull())
    {
      aNewData.BasisAdaptor =
        occ::down_cast<Geom2dAdaptor_Curve>(anOffsetData->BasisAdaptor->ShallowCopy());
    }
    aNewData.Offset    = anOffsetData->Offset;
    aNewData.EvalRep   = anOffsetData->EvalRep;
    aCopy->myCurveData = std::move(aNewData);
  }
  else if (const auto* aBSplineData = std::get_if<BSplineData>(&myCurveData))
  {
    BSplineData aNewData;
    aNewData.Curve     = aBSplineData->Curve;
    aNewData.EvalRep   = aBSplineData->EvalRep;
    aCopy->myCurveData = std::move(aNewData);
  }
  else if (std::holds_alternative<BezierData>(myCurveData))
  {
    const BezierData& aBezierData = std::get<BezierData>(myCurveData);
    BezierData        aNewData;
    aNewData.Curve   = aBezierData.Curve;
    aNewData.EvalRep = aBezierData.EvalRep;
    // Cache is not copied - will be rebuilt on demand.
    aCopy->myCurveData = std::move(aNewData);
  }
  else
  {
    // Elementary curve types (gp_Lin2d, gp_Circ2d, etc.) are value types
    aCopy->myCurveData = myCurveData;
  }

  return aCopy;
}

//=======================================================================
// function : LocalContinuity
// purpose  : Computes the Continuity of a BSplineCurve
//           between the parameters U1 and U2
//           The continuity is C(d-m)
//             with   d = degree,
//                    m = max multiplicity of the Knots between U1 and U2
//=======================================================================

GeomAbs_Shape Geom2dAdaptor_Curve::LocalContinuity(const double U1, const double U2) const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_BSplineCurve, " ");
  const auto&                       aBSpline = std::get<BSplineData>(myCurveData).Curve;
  int                               Nb       = aBSpline->NbKnots();
  int                               Index1   = 0;
  int                               Index2   = 0;
  double                            newFirst, newLast;
  const NCollection_Array1<double>& TK = aBSpline->Knots();
  const NCollection_Array1<int>&    TM = aBSpline->Multiplicities();
  BSplCLib::LocateParameter(aBSpline->Degree(),
                            TK,
                            TM,
                            U1,
                            aBSpline->IsPeriodic(),
                            1,
                            Nb,
                            Index1,
                            newFirst);
  BSplCLib::LocateParameter(aBSpline->Degree(),
                            TK,
                            TM,
                            U2,
                            aBSpline->IsPeriodic(),
                            1,
                            Nb,
                            Index2,
                            newLast);
  if (std::abs(newFirst - TK(Index1 + 1)) < Precision::PConfusion())
  {
    if (Index1 < Nb)
      Index1++;
  }
  if (std::abs(newLast - TK(Index2)) < Precision::PConfusion())
    Index2--;
  int MultMax;
  // attention aux courbes peridiques.
  if (aBSpline->IsPeriodic() && Index1 == Nb)
    Index1 = 1;

  if ((Index2 - Index1 <= 0) && (!aBSpline->IsPeriodic()))
  {
    MultMax = 100; // CN entre 2 Noeuds consecutifs
  }
  else
  {
    MultMax = TM(Index1 + 1);
    for (int i = Index1 + 1; i <= Index2; i++)
    {
      if (TM(i) > MultMax)
        MultMax = TM(i);
    }
    MultMax = aBSpline->Degree() - MultMax;
  }
  if (MultMax <= 0)
  {
    return GeomAbs_C0;
  }
  else if (MultMax == 1)
  {
    return GeomAbs_C1;
  }
  else if (MultMax == 2)
  {
    return GeomAbs_C2;
  }
  else if (MultMax == 3)
  {
    return GeomAbs_C3;
  }
  else
  {
    return GeomAbs_CN;
  }
}

//=================================================================================================

Geom2dAdaptor_Curve::Geom2dAdaptor_Curve()
    : myTypeCurve(GeomAbs_OtherCurve),
      myFirst(0.0),
      myLast(0.0)
{
}

//=================================================================================================

Geom2dAdaptor_Curve::Geom2dAdaptor_Curve(const occ::handle<Geom2d_Curve>& theCrv)
    : myTypeCurve(GeomAbs_OtherCurve),
      myFirst(0.0),
      myLast(0.0)
{
  Load(theCrv);
}

//=================================================================================================

Geom2dAdaptor_Curve::Geom2dAdaptor_Curve(const occ::handle<Geom2d_Curve>& theCrv,
                                         const double                     theUFirst,
                                         const double                     theULast)
    : myTypeCurve(GeomAbs_OtherCurve),
      myFirst(theUFirst),
      myLast(theULast)
{
  Load(theCrv, theUFirst, theULast);
}

//=================================================================================================

void Geom2dAdaptor_Curve::Reset()
{
  myTypeCurve = GeomAbs_OtherCurve;
  myCurve.Nullify();
  myCurveData = std::monostate{};
  myFirst = myLast = 0.0;
}

//=================================================================================================

void Geom2dAdaptor_Curve::load(const occ::handle<Geom2d_Curve>& C,
                               const double                     UFirst,
                               const double                     ULast)
{
  myFirst = UFirst;
  myLast  = ULast;

  if (myCurve != C)
  {
    myCurve     = C;
    myCurveData = std::monostate{};

    occ::handle<Standard_Type> TheType = C->DynamicType();
    if (TheType == STANDARD_TYPE(Geom2d_TrimmedCurve))
    {
      Load(occ::down_cast<Geom2d_TrimmedCurve>(C)->BasisCurve(), UFirst, ULast);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Circle))
    {
      myTypeCurve = GeomAbs_Circle;
      myCurveData = occ::down_cast<Geom2d_Circle>(C)->Circ2d();
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Line))
    {
      myTypeCurve = GeomAbs_Line;
      myCurveData = occ::down_cast<Geom2d_Line>(C)->Lin2d();
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Ellipse))
    {
      myTypeCurve = GeomAbs_Ellipse;
      myCurveData = occ::down_cast<Geom2d_Ellipse>(C)->Elips2d();
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Parabola))
    {
      myTypeCurve = GeomAbs_Parabola;
      myCurveData = occ::down_cast<Geom2d_Parabola>(C)->Parab2d();
    }
    else if (TheType == STANDARD_TYPE(Geom2d_Hyperbola))
    {
      myTypeCurve = GeomAbs_Hyperbola;
      myCurveData = occ::down_cast<Geom2d_Hyperbola>(C)->Hypr2d();
    }
    else if (TheType == STANDARD_TYPE(Geom2d_BezierCurve))
    {
      myTypeCurve = GeomAbs_BezierCurve;
      BezierData aBezierData;
      aBezierData.Curve   = occ::down_cast<Geom2d_BezierCurve>(myCurve);
      aBezierData.EvalRep = aBezierData.Curve->EvalRepresentation();
      myCurveData         = std::move(aBezierData);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_BSplineCurve))
    {
      myTypeCurve = GeomAbs_BSplineCurve;
      BSplineData aBSplineData;
      aBSplineData.Curve   = occ::down_cast<Geom2d_BSplineCurve>(myCurve);
      aBSplineData.EvalRep = aBSplineData.Curve->EvalRepresentation();
      myCurveData          = std::move(aBSplineData);
    }
    else if (TheType == STANDARD_TYPE(Geom2d_OffsetCurve))
    {
      myTypeCurve                                   = GeomAbs_OffsetCurve;
      occ::handle<Geom2d_OffsetCurve> anOffsetCurve = occ::down_cast<Geom2d_OffsetCurve>(myCurve);
      // Create nested adaptor for base curve and store offset data
      occ::handle<Geom2d_Curve> aBaseCurve = anOffsetCurve->BasisCurve();

      OffsetData anOffsetData;
      anOffsetData.BasisAdaptor = new Geom2dAdaptor_Curve(aBaseCurve, UFirst, ULast);
      anOffsetData.Offset       = anOffsetCurve->Offset();
      anOffsetData.EvalRep      = anOffsetCurve->EvalRepresentation();
      myCurveData               = std::move(anOffsetData);
    }
    else
    {
      myTypeCurve = GeomAbs_OtherCurve;
    }
  }
  else
  {
    // Same curve but potentially different parameters - invalidate cache
    if (auto* aBSplineData = std::get_if<BSplineData>(&myCurveData))
    {
      aBSplineData->Cache.Nullify();
    }
    else if (auto* aBezierData = std::get_if<BezierData>(&myCurveData))
    {
      aBezierData->Cache.Nullify();
    }
  }
}

//    --
//    --     Global methods - Apply to the whole curve.
//    --

//=================================================================================================

GeomAbs_Shape Geom2dAdaptor_Curve::Continuity() const
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    return LocalContinuity(myFirst, myLast);
  }
  else if (myTypeCurve == GeomAbs_OffsetCurve)
  {
    GeomAbs_Shape S = occ::down_cast<Geom2d_OffsetCurve>(myCurve)->GetBasisCurveContinuity();
    switch (S)
    {
      case GeomAbs_CN:
        return GeomAbs_CN;
      case GeomAbs_C3:
        return GeomAbs_C2;
      case GeomAbs_C2:
        return GeomAbs_C1;
      case GeomAbs_C1:
        return GeomAbs_C0;
      case GeomAbs_G1:
        return GeomAbs_G1;
      case GeomAbs_G2:
        return GeomAbs_G2;

      default:
        throw Standard_NoSuchObject("Geom2dAdaptor_Curve::Continuity");
    }
  }

  else if (myTypeCurve == GeomAbs_OtherCurve)
  {
    throw Standard_NoSuchObject("Geom2dAdaptor_Curve::Continuity");
  }
  else
  {
    return GeomAbs_CN;
  }
}

//=================================================================================================

int Geom2dAdaptor_Curve::NbIntervals(const GeomAbs_Shape S) const
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    const auto& aBSpline = std::get<BSplineData>(myCurveData).Curve;
    if ((!aBSpline->IsPeriodic() && S <= Continuity()) || S == GeomAbs_C0)
    {
      return 1;
    }

    int aDegree = aBSpline->Degree();
    int aCont;

    switch (S)
    {
      case GeomAbs_C1:
        aCont = 1;
        break;
      case GeomAbs_C2:
        aCont = 2;
        break;
      case GeomAbs_C3:
        aCont = 3;
        break;
      case GeomAbs_CN:
        aCont = aDegree;
        break;
      default:
        throw Standard_DomainError("Geom2dAdaptor_Curve::NbIntervals()");
    }

    double anEps = std::min(Resolution(Precision::Confusion()), Precision::PConfusion());

    return BSplCLib::Intervals(aBSpline->Knots(),
                               aBSpline->Multiplicities(),
                               aDegree,
                               aBSpline->IsPeriodic(),
                               aCont,
                               myFirst,
                               myLast,
                               anEps,
                               nullptr);
  }

  else if (myTypeCurve == GeomAbs_OffsetCurve)
  {
    int           myNbIntervals = 1;
    GeomAbs_Shape BaseS         = GeomAbs_C0;
    switch (S)
    {
      case GeomAbs_G1:
      case GeomAbs_G2:
        throw Standard_DomainError("GeomAdaptor_Curve::NbIntervals");
        break;
      case GeomAbs_C0:
        BaseS = GeomAbs_C1;
        break;
      case GeomAbs_C1:
        BaseS = GeomAbs_C2;
        break;
      case GeomAbs_C2:
        BaseS = GeomAbs_C3;
        break;
      default:
        BaseS = GeomAbs_CN;
    }
    Geom2dAdaptor_Curve anAdaptor(occ::down_cast<Geom2d_OffsetCurve>(myCurve)->BasisCurve(),
                                  myFirst,
                                  myLast);
    myNbIntervals = anAdaptor.NbIntervals(BaseS);
    return myNbIntervals;
  }

  else
  {
    return 1;
  }
}

//=================================================================================================

void Geom2dAdaptor_Curve::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    const auto& aBSpline = std::get<BSplineData>(myCurveData).Curve;
    if ((!aBSpline->IsPeriodic() && S <= Continuity()) || S == GeomAbs_C0)
    {
      T(T.Lower())     = myFirst;
      T(T.Lower() + 1) = myLast;
      return;
    }

    int aDegree = aBSpline->Degree();
    int aCont;

    switch (S)
    {
      case GeomAbs_C1:
        aCont = 1;
        break;
      case GeomAbs_C2:
        aCont = 2;
        break;
      case GeomAbs_C3:
        aCont = 3;
        break;
      case GeomAbs_CN:
        aCont = aDegree;
        break;
      default:
        throw Standard_DomainError("Geom2dAdaptor_Curve::Intervals()");
    }

    double anEps = std::min(Resolution(Precision::Confusion()), Precision::PConfusion());

    BSplCLib::Intervals(aBSpline->Knots(),
                        aBSpline->Multiplicities(),
                        aDegree,
                        aBSpline->IsPeriodic(),
                        aCont,
                        myFirst,
                        myLast,
                        anEps,
                        &T);
  }

  else if (myTypeCurve == GeomAbs_OffsetCurve)
  {
    int           myNbIntervals = 1;
    GeomAbs_Shape BaseS         = GeomAbs_C0;
    switch (S)
    {
      case GeomAbs_G1:
      case GeomAbs_G2:
        throw Standard_DomainError("GeomAdaptor_Curve::NbIntervals");
        break;
      case GeomAbs_C0:
        BaseS = GeomAbs_C1;
        break;
      case GeomAbs_C1:
        BaseS = GeomAbs_C2;
        break;
      case GeomAbs_C2:
        BaseS = GeomAbs_C3;
        break;
      default:
        BaseS = GeomAbs_CN;
    }

    Geom2dAdaptor_Curve anAdaptor(occ::down_cast<Geom2d_OffsetCurve>(myCurve)->BasisCurve(),
                                  myFirst,
                                  myLast);
    myNbIntervals = anAdaptor.NbIntervals(BaseS);
    anAdaptor.Intervals(T, BaseS);
    T(T.Lower())                 = myFirst;
    T(T.Lower() + myNbIntervals) = myLast;
  }

  else
  {
    T(T.Lower())     = myFirst;
    T(T.Lower() + 1) = myLast;
  }
}

//=================================================================================================

occ::handle<Adaptor2d_Curve2d> Geom2dAdaptor_Curve::Trim(const double First,
                                                         const double Last,
                                                         // const double Tol) const
                                                         const double) const
{
  occ::handle<Geom2dAdaptor_Curve> HE = new Geom2dAdaptor_Curve(myCurve, First, Last);
  return HE;
}

//=================================================================================================

bool Geom2dAdaptor_Curve::IsClosed() const
{
  if (!Precision::IsPositiveInfinite(myLast) && !Precision::IsNegativeInfinite(myFirst))
  {
    gp_Pnt2d Pd = Value(myFirst);
    gp_Pnt2d Pf = Value(myLast);
    return (Pd.Distance(Pf) <= Precision::Confusion());
  }
  else
    return false;
}

//=================================================================================================

bool Geom2dAdaptor_Curve::IsPeriodic() const
{
  return myCurve->IsPeriodic();
}

//=================================================================================================

double Geom2dAdaptor_Curve::Period() const
{
  return myCurve->LastParameter() - myCurve->FirstParameter();
}

//=================================================================================================

void Geom2dAdaptor_Curve::RebuildCache(const double theParameter) const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
  {
    // Create cache for Bezier
    auto&                           aBezierData = std::get<BezierData>(myCurveData);
    occ::handle<Geom2d_BezierCurve> aBezier     = occ::down_cast<Geom2d_BezierCurve>(myCurve);
    int                             aDeg        = aBezier->Degree();
    NCollection_Array1<double>      aFlatKnots(BSplCLib::FlatBezierKnots(aDeg), 1, 2 * (aDeg + 1));
    if (aBezierData.Cache.IsNull())
      aBezierData.Cache = new BSplCLib_Cache(aDeg,
                                             aBezier->IsPeriodic(),
                                             aFlatKnots,
                                             aBezier->Poles(),
                                             aBezier->Weights());
    aBezierData.Cache->BuildCache(theParameter, aFlatKnots, aBezier->Poles(), aBezier->Weights());
  }
  else if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    // Create cache for B-spline
    auto&       aBSplineData = std::get<BSplineData>(myCurveData);
    const auto& aBSpline     = aBSplineData.Curve;
    if (aBSplineData.Cache.IsNull())
      aBSplineData.Cache = new BSplCLib_Cache(aBSpline->Degree(),
                                              aBSpline->IsPeriodic(),
                                              aBSpline->KnotSequence(),
                                              aBSpline->Poles(),
                                              aBSpline->Weights());
    aBSplineData.Cache->BuildCache(theParameter,
                                   aBSpline->KnotSequence(),
                                   aBSpline->Poles(),
                                   aBSpline->Weights());
  }
}

//=================================================================================================

bool Geom2dAdaptor_Curve::IsBoundary(const double theU, int& theSpanStart, int& theSpanFinish) const
{
  const auto* aBSplineData = std::get_if<BSplineData>(&myCurveData);
  if (aBSplineData != nullptr && !aBSplineData->Curve.IsNull()
      && (theU == myFirst || theU == myLast))
  {
    const auto& aBSpline = aBSplineData->Curve;
    if (theU == myFirst)
    {
      aBSpline->LocateU(myFirst, PosTol, theSpanStart, theSpanFinish);
      if (theSpanStart < 1)
        theSpanStart = 1;
      if (theSpanStart >= theSpanFinish)
        theSpanFinish = theSpanStart + 1;
    }
    else if (theU == myLast)
    {
      aBSpline->LocateU(myLast, PosTol, theSpanStart, theSpanFinish);
      if (theSpanFinish > aBSpline->NbKnots())
        theSpanFinish = aBSpline->NbKnots();
      if (theSpanStart >= theSpanFinish)
        theSpanStart = theSpanFinish - 1;
    }
    return true;
  }
  return false;
}

//=================================================================================================

gp_Pnt2d Geom2dAdaptor_Curve::Value(const double U) const
{
  gp_Pnt2d aRes;
  D0(U, aRes);
  return aRes;
}

//=================================================================================================

void Geom2dAdaptor_Curve::D0(const double U, gp_Pnt2d& P) const
{
  std::optional<gp_Pnt2d> aResult = EvalD0(U);
  if (!aResult)
    throw Geom2d_UndefinedValue("Geom2dAdaptor_Curve::D0: evaluation failed");
  P = *aResult;
}

//=================================================================================================

std::optional<gp_Pnt2d> Geom2dAdaptor_Curve::EvalD0(double U) const
{
  gp_Pnt2d P;
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      P = ElCLib::Value(U, std::get<gp_Lin2d>(myCurveData));
      return P;

    case GeomAbs_Circle:
      P = ElCLib::Value(U, std::get<gp_Circ2d>(myCurveData));
      return P;

    case GeomAbs_Ellipse:
      P = ElCLib::Value(U, std::get<gp_Elips2d>(myCurveData));
      return P;

    case GeomAbs_Hyperbola:
      P = ElCLib::Value(U, std::get<gp_Hypr2d>(myCurveData));
      return P;

    case GeomAbs_Parabola:
      P = ElCLib::Value(U, std::get<gp_Parab2d>(myCurveData));
      return P;

    case GeomAbs_BezierCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD0(U);
      }
      auto& aBezierData = std::get<BezierData>(myCurveData);
      if (aBezierData.Cache.IsNull())
        RebuildCache(U);
      aBezierData.Cache->D0(U, P);
      return P;
    }

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD0(U);
      }
      int                aStart = 0, aFinish = 0;
      const BSplineData& aBSplineData = std::get<BSplineData>(myCurveData);
      if (IsBoundary(U, aStart, aFinish))
      {
        aBSplineData.Curve->LocalD0(U, aStart, aFinish, P);
      }
      else
      {
        if (aBSplineData.Cache.IsNull() || !aBSplineData.Cache->IsCacheValid(U))
          RebuildCache(U);
        aBSplineData.Cache->D0(U, P);
      }
      return P;
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD0(U);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      if (!Geom2d_OffsetCurveUtils::EvaluateD0(U,
                                               anOffsetData.BasisAdaptor.get(),
                                               anOffsetData.Offset,
                                               P))
      {
        return std::nullopt;
      }
      return P;
    }

    default:
      return myCurve->EvalD0(U);
  }
}

//=================================================================================================

void Geom2dAdaptor_Curve::D1(const double U, gp_Pnt2d& P, gp_Vec2d& V) const
{
  std::optional<Geom2d_Curve::ResD1> aResult = EvalD1(U);
  if (!aResult)
    throw Geom2d_UndefinedDerivative("Geom2dAdaptor_Curve::D1: evaluation failed");
  P = aResult->Point;
  V = aResult->D1;
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD1> Geom2dAdaptor_Curve::EvalD1(double U) const
{
  Geom2d_Curve::ResD1 aResult;
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      ElCLib::D1(U, std::get<gp_Lin2d>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_Circle:
      ElCLib::D1(U, std::get<gp_Circ2d>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_Ellipse:
      ElCLib::D1(U, std::get<gp_Elips2d>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_Hyperbola:
      ElCLib::D1(U, std::get<gp_Hypr2d>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_Parabola:
      ElCLib::D1(U, std::get<gp_Parab2d>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_BezierCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD1(U);
      }
      auto& aBezierData = std::get<BezierData>(myCurveData);
      if (aBezierData.Cache.IsNull())
        RebuildCache(U);
      aBezierData.Cache->D1(U, aResult.Point, aResult.D1);
      return aResult;
    }

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD1(U);
      }
      int                aStart = 0, aFinish = 0;
      const BSplineData& aBSplineData = std::get<BSplineData>(myCurveData);
      if (IsBoundary(U, aStart, aFinish))
      {
        aBSplineData.Curve->LocalD1(U, aStart, aFinish, aResult.Point, aResult.D1);
      }
      else
      {
        if (aBSplineData.Cache.IsNull() || !aBSplineData.Cache->IsCacheValid(U))
          RebuildCache(U);
        aBSplineData.Cache->D1(U, aResult.Point, aResult.D1);
      }
      return aResult;
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD1(U);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      if (!Geom2d_OffsetCurveUtils::EvaluateD1(U,
                                               anOffsetData.BasisAdaptor.get(),
                                               anOffsetData.Offset,
                                               aResult.Point,
                                               aResult.D1))
      {
        return std::nullopt;
      }
      return aResult;
    }

    default:
      return myCurve->EvalD1(U);
  }
}

//=================================================================================================

void Geom2dAdaptor_Curve::D2(const double U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
{
  std::optional<Geom2d_Curve::ResD2> aResult = EvalD2(U);
  if (!aResult)
    throw Geom2d_UndefinedDerivative("Geom2dAdaptor_Curve::D2: evaluation failed");
  P  = aResult->Point;
  V1 = aResult->D1;
  V2 = aResult->D2;
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD2> Geom2dAdaptor_Curve::EvalD2(double U) const
{
  Geom2d_Curve::ResD2 aResult;
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      ElCLib::D1(U, std::get<gp_Lin2d>(myCurveData), aResult.Point, aResult.D1);
      aResult.D2.SetCoord(0., 0.);
      return aResult;

    case GeomAbs_Circle:
      ElCLib::D2(U, std::get<gp_Circ2d>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      return aResult;

    case GeomAbs_Ellipse:
      ElCLib::D2(U, std::get<gp_Elips2d>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      return aResult;

    case GeomAbs_Hyperbola:
      ElCLib::D2(U, std::get<gp_Hypr2d>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      return aResult;

    case GeomAbs_Parabola:
      ElCLib::D2(U, std::get<gp_Parab2d>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      return aResult;

    case GeomAbs_BezierCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD2(U);
      }
      auto& aBezierData = std::get<BezierData>(myCurveData);
      if (aBezierData.Cache.IsNull())
        RebuildCache(U);
      aBezierData.Cache->D2(U, aResult.Point, aResult.D1, aResult.D2);
      return aResult;
    }

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD2(U);
      }
      int                aStart = 0, aFinish = 0;
      const BSplineData& aBSplineData = std::get<BSplineData>(myCurveData);
      if (IsBoundary(U, aStart, aFinish))
      {
        aBSplineData.Curve->LocalD2(U, aStart, aFinish, aResult.Point, aResult.D1, aResult.D2);
      }
      else
      {
        if (aBSplineData.Cache.IsNull() || !aBSplineData.Cache->IsCacheValid(U))
          RebuildCache(U);
        aBSplineData.Cache->D2(U, aResult.Point, aResult.D1, aResult.D2);
      }
      return aResult;
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD2(U);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      if (!Geom2d_OffsetCurveUtils::EvaluateD2(U,
                                               anOffsetData.BasisAdaptor.get(),
                                               anOffsetData.Offset,
                                               aResult.Point,
                                               aResult.D1,
                                               aResult.D2))
      {
        return std::nullopt;
      }
      return aResult;
    }

    default:
      return myCurve->EvalD2(U);
  }
}

//=================================================================================================

void Geom2dAdaptor_Curve::D3(const double U,
                             gp_Pnt2d&    P,
                             gp_Vec2d&    V1,
                             gp_Vec2d&    V2,
                             gp_Vec2d&    V3) const
{
  std::optional<Geom2d_Curve::ResD3> aResult = EvalD3(U);
  if (!aResult)
    throw Geom2d_UndefinedDerivative("Geom2dAdaptor_Curve::D3: evaluation failed");
  P  = aResult->Point;
  V1 = aResult->D1;
  V2 = aResult->D2;
  V3 = aResult->D3;
}

//=================================================================================================

std::optional<Geom2d_Curve::ResD3> Geom2dAdaptor_Curve::EvalD3(double U) const
{
  Geom2d_Curve::ResD3 aResult;
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      ElCLib::D1(U, std::get<gp_Lin2d>(myCurveData), aResult.Point, aResult.D1);
      aResult.D2.SetCoord(0., 0.);
      aResult.D3.SetCoord(0., 0.);
      return aResult;

    case GeomAbs_Circle:
      ElCLib::D3(U,
                 std::get<gp_Circ2d>(myCurveData),
                 aResult.Point,
                 aResult.D1,
                 aResult.D2,
                 aResult.D3);
      return aResult;

    case GeomAbs_Ellipse:
      ElCLib::D3(U,
                 std::get<gp_Elips2d>(myCurveData),
                 aResult.Point,
                 aResult.D1,
                 aResult.D2,
                 aResult.D3);
      return aResult;

    case GeomAbs_Hyperbola:
      ElCLib::D3(U,
                 std::get<gp_Hypr2d>(myCurveData),
                 aResult.Point,
                 aResult.D1,
                 aResult.D2,
                 aResult.D3);
      return aResult;

    case GeomAbs_Parabola:
      ElCLib::D2(U, std::get<gp_Parab2d>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      aResult.D3.SetCoord(0., 0.);
      return aResult;

    case GeomAbs_BezierCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD3(U);
      }
      auto& aBezierData = std::get<BezierData>(myCurveData);
      if (aBezierData.Cache.IsNull())
        RebuildCache(U);
      aBezierData.Cache->D3(U, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
      return aResult;
    }

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD3(U);
      }
      int                aStart = 0, aFinish = 0;
      const BSplineData& aBSplineData = std::get<BSplineData>(myCurveData);
      if (IsBoundary(U, aStart, aFinish))
      {
        aBSplineData.Curve
          ->LocalD3(U, aStart, aFinish, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
      }
      else
      {
        if (aBSplineData.Cache.IsNull() || !aBSplineData.Cache->IsCacheValid(U))
          RebuildCache(U);
        aBSplineData.Cache->D3(U, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
      }
      return aResult;
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD3(U);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      if (!Geom2d_OffsetCurveUtils::EvaluateD3(U,
                                               anOffsetData.BasisAdaptor.get(),
                                               anOffsetData.Offset,
                                               aResult.Point,
                                               aResult.D1,
                                               aResult.D2,
                                               aResult.D3))
      {
        return std::nullopt;
      }
      return aResult;
    }

    default:
      return myCurve->EvalD3(U);
  }
}

//=================================================================================================

gp_Vec2d Geom2dAdaptor_Curve::DN(const double U, const int N) const
{
  std::optional<gp_Vec2d> aResult = EvalDN(U, N);
  if (!aResult)
    throw Geom2d_UndefinedDerivative("Geom2dAdaptor_Curve::DN: evaluation failed");
  return *aResult;
}

//=================================================================================================

std::optional<gp_Vec2d> Geom2dAdaptor_Curve::EvalDN(double U, int N) const
{
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      return ElCLib::DN(U, std::get<gp_Lin2d>(myCurveData), N);

    case GeomAbs_Circle:
      return ElCLib::DN(U, std::get<gp_Circ2d>(myCurveData), N);

    case GeomAbs_Ellipse:
      return ElCLib::DN(U, std::get<gp_Elips2d>(myCurveData), N);

    case GeomAbs_Hyperbola:
      return ElCLib::DN(U, std::get<gp_Hypr2d>(myCurveData), N);

    case GeomAbs_Parabola:
      return ElCLib::DN(U, std::get<gp_Parab2d>(myCurveData), N);

    case GeomAbs_BezierCurve:
      return myCurve->EvalDN(U, N);

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalDN(U, N);
      }
      int aStart = 0, aFinish = 0;
      if (IsBoundary(U, aStart, aFinish))
      {
        return std::get<BSplineData>(myCurveData).Curve->LocalDN(U, aStart, aFinish, N);
      }
      else
        return myCurve->EvalDN(U, N);
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalDN(U, N);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      gp_Vec2d    aDN;
      if (!Geom2d_OffsetCurveUtils::EvaluateDN(U,
                                               anOffsetData.BasisAdaptor.get(),
                                               anOffsetData.Offset,
                                               N,
                                               aDN))
      {
        return std::nullopt;
      }
      return aDN;
    }

    default:
      break;
  }
  return myCurve->EvalDN(U, N);
}

//=================================================================================================

double Geom2dAdaptor_Curve::Resolution(const double Ruv) const
{
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      return Ruv;
    case GeomAbs_Circle: {
      double R = std::get<gp_Circ2d>(myCurveData).Radius();
      if (R > Ruv / 2.)
        return 2 * std::asin(Ruv / (2 * R));
      else
        return 2 * M_PI;
    }
    case GeomAbs_Ellipse: {
      return Ruv / std::get<gp_Elips2d>(myCurveData).MajorRadius();
    }
    case GeomAbs_BezierCurve: {
      double res;
      occ::down_cast<Geom2d_BezierCurve>(myCurve)->Resolution(Ruv, res);
      return res;
    }
    case GeomAbs_BSplineCurve: {
      double res;
      occ::down_cast<Geom2d_BSplineCurve>(myCurve)->Resolution(Ruv, res);
      return res;
    }
    default:
      return Precision::Parametric(Ruv);
  }
}

//    --
//    --     The following methods must  be called when GetType returned
//    --     the corresponding type.
//    --

//=================================================================================================

gp_Lin2d Geom2dAdaptor_Curve::Line() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Line,
                                 "Geom2dAdaptor_Curve::Line() - curve is not a Line");
  return std::get<gp_Lin2d>(myCurveData);
}

//=================================================================================================

gp_Circ2d Geom2dAdaptor_Curve::Circle() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Circle,
                                 "Geom2dAdaptor_Curve::Circle() - curve is not a Circle");
  return std::get<gp_Circ2d>(myCurveData);
}

//=================================================================================================

gp_Elips2d Geom2dAdaptor_Curve::Ellipse() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Ellipse,
                                 "Geom2dAdaptor_Curve::Ellipse() - curve is not an Ellipse");
  return std::get<gp_Elips2d>(myCurveData);
}

//=================================================================================================

gp_Hypr2d Geom2dAdaptor_Curve::Hyperbola() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Hyperbola,
                                 "Geom2dAdaptor_Curve::Hyperbola() - curve is not a Hyperbola");
  return std::get<gp_Hypr2d>(myCurveData);
}

//=================================================================================================

gp_Parab2d Geom2dAdaptor_Curve::Parabola() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Parabola,
                                 "Geom2dAdaptor_Curve::Parabola() - curve is not a Parabola");
  return std::get<gp_Parab2d>(myCurveData);
}

//=================================================================================================

int Geom2dAdaptor_Curve::Degree() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return occ::down_cast<Geom2d_BezierCurve>(myCurve)->Degree();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return std::get<BSplineData>(myCurveData).Curve->Degree();
  else
    throw Standard_NoSuchObject();
}

//=================================================================================================

bool Geom2dAdaptor_Curve::IsRational() const
{
  switch (myTypeCurve)
  {
    case GeomAbs_BSplineCurve:
      return std::get<BSplineData>(myCurveData).Curve->IsRational();
    case GeomAbs_BezierCurve:
      return occ::down_cast<Geom2d_BezierCurve>(myCurve)->IsRational();
    default:
      return false;
  }
}

//=================================================================================================

int Geom2dAdaptor_Curve::NbPoles() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return occ::down_cast<Geom2d_BezierCurve>(myCurve)->NbPoles();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return std::get<BSplineData>(myCurveData).Curve->NbPoles();
  else
    throw Standard_NoSuchObject();
}

//=================================================================================================

int Geom2dAdaptor_Curve::NbKnots() const
{
  if (myTypeCurve != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("Geom2dAdaptor_Curve::NbKnots");
  return std::get<BSplineData>(myCurveData).Curve->NbKnots();
}

//=================================================================================================

occ::handle<Geom2d_BezierCurve> Geom2dAdaptor_Curve::Bezier() const
{
  return occ::down_cast<Geom2d_BezierCurve>(myCurve);
}

//=================================================================================================

occ::handle<Geom2d_BSplineCurve> Geom2dAdaptor_Curve::BSpline() const
{
  if (const auto* aBSplineData = std::get_if<BSplineData>(&myCurveData))
  {
    return aBSplineData->Curve;
  }
  return occ::handle<Geom2d_BSplineCurve>();
}

static int nbPoints(const occ::handle<Geom2d_Curve>& theCurve)
{

  int nbs = 20;

  if (theCurve->IsKind(STANDARD_TYPE(Geom2d_Line)))
    nbs = 2;
  else if (theCurve->IsKind(STANDARD_TYPE(Geom2d_BezierCurve)))
  {
    nbs = 3 + occ::down_cast<Geom2d_BezierCurve>(theCurve)->NbPoles();
  }
  else if (theCurve->IsKind(STANDARD_TYPE(Geom2d_BSplineCurve)))
  {
    nbs = occ::down_cast<Geom2d_BSplineCurve>(theCurve)->NbKnots();
    nbs *= occ::down_cast<Geom2d_BSplineCurve>(theCurve)->Degree();
    if (nbs < 2.0)
      nbs = 2;
  }
  else if (theCurve->IsKind(STANDARD_TYPE(Geom2d_OffsetCurve)))
  {
    occ::handle<Geom2d_Curve> aCurve = occ::down_cast<Geom2d_OffsetCurve>(theCurve)->BasisCurve();
    return std::max(nbs, nbPoints(aCurve));
  }

  else if (theCurve->IsKind(STANDARD_TYPE(Geom2d_TrimmedCurve)))
  {
    occ::handle<Geom2d_Curve> aCurve = occ::down_cast<Geom2d_TrimmedCurve>(theCurve)->BasisCurve();
    return std::max(nbs, nbPoints(aCurve));
  }
  if (nbs > 300)
    nbs = 300;
  return nbs;
}

int Geom2dAdaptor_Curve::NbSamples() const
{
  return nbPoints(myCurve);
}
