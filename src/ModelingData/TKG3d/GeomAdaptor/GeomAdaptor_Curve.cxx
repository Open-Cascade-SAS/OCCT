// Created on: 1993-04-29
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
// 10/07/97 : PMN -> Pas de calcul de resolution dans Nb(Intervals)(PRO9248)
// 20/10/97 : RBV -> traitement des offset curves

#define No_Standard_RangeError
#define No_Standard_OutOfRange

#include <GeomAdaptor_Curve.hxx>

#include <Adaptor3d_Curve.hxx>
#include <BSplCLib.hxx>
#include <BSplCLib_Cache.hxx>
#include <ElCLib.hxx>
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
#include <Geom_UndefinedDerivative.hxx>
#include <Geom_UndefinedValue.hxx>
#include <GeomAbs_Shape.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

#include "../Geom/Geom_OffsetCurveUtils.pxx"

// #include <GeomConvert_BSplineCurveKnotSplitting.hxx>
static const double PosTol = Precision::PConfusion() / 2;

IMPLEMENT_STANDARD_RTTIEXT(GeomAdaptor_Curve, Adaptor3d_Curve)

namespace
{
bool hasEvalRep(const GeomAdaptor_Curve::CurveDataVariant& theData)
{
  if (const GeomAdaptor_Curve::BezierData* aBezierData =
        std::get_if<GeomAdaptor_Curve::BezierData>(&theData))
  {
    return !aBezierData->EvalRep.IsNull();
  }
  if (const GeomAdaptor_Curve::BSplineData* aBSplineData =
        std::get_if<GeomAdaptor_Curve::BSplineData>(&theData))
  {
    return !aBSplineData->EvalRep.IsNull();
  }
  if (const GeomAdaptor_Curve::OffsetData* anOffsetData =
        std::get_if<GeomAdaptor_Curve::OffsetData>(&theData))
  {
    return !anOffsetData->EvalRep.IsNull();
  }
  return false;
}
} // namespace

//=================================================================================================

occ::handle<Adaptor3d_Curve> GeomAdaptor_Curve::ShallowCopy() const
{
  occ::handle<GeomAdaptor_Curve> aCopy = new GeomAdaptor_Curve();

  aCopy->myCurve     = myCurve;
  aCopy->myTypeCurve = myTypeCurve;
  aCopy->myFirst     = myFirst;
  aCopy->myLast      = myLast;

  // Copy curve-specific data based on variant type
  if (const auto* anOffsetData = std::get_if<OffsetData>(&myCurveData))
  {
    OffsetData aCopyData;
    aCopyData.BasisAdaptor =
      occ::down_cast<GeomAdaptor_Curve>(anOffsetData->BasisAdaptor->ShallowCopy());
    aCopyData.Offset    = anOffsetData->Offset;
    aCopyData.Direction = anOffsetData->Direction;
    aCopyData.EvalRep   = anOffsetData->EvalRep;
    aCopy->myCurveData  = std::move(aCopyData);
  }
  else if (const auto* aBSplineData = std::get_if<BSplineData>(&myCurveData))
  {
    BSplineData aCopyData;
    aCopyData.Curve    = aBSplineData->Curve;
    aCopyData.EvalRep  = aBSplineData->EvalRep;
    aCopy->myCurveData = std::move(aCopyData);
  }
  else if (std::holds_alternative<BezierData>(myCurveData))
  {
    const BezierData& aBezierData = std::get<BezierData>(myCurveData);
    BezierData        aCopyData;
    aCopyData.Curve   = aBezierData.Curve;
    aCopyData.EvalRep = aBezierData.EvalRep;
    // Cache is not copied - will be rebuilt on demand.
    aCopy->myCurveData = std::move(aCopyData);
  }
  else
  {
    // Elementary curve types (gp_Lin, gp_Circ, etc.) are value types - direct copy
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

GeomAbs_Shape GeomAdaptor_Curve::LocalContinuity(const double U1, const double U2) const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_BSplineCurve, " ");
  const auto&                       aBSpl  = std::get<BSplineData>(myCurveData).Curve;
  int                               Nb     = aBSpl->NbKnots();
  int                               Index1 = 0;
  int                               Index2 = 0;
  double                            newFirst, newLast;
  const NCollection_Array1<double>& TK = aBSpl->Knots();
  const NCollection_Array1<int>&    TM = aBSpl->Multiplicities();
  BSplCLib::LocateParameter(aBSpl->Degree(),
                            TK,
                            TM,
                            U1,
                            aBSpl->IsPeriodic(),
                            1,
                            Nb,
                            Index1,
                            newFirst);
  BSplCLib::LocateParameter(aBSpl->Degree(),
                            TK,
                            TM,
                            U2,
                            aBSpl->IsPeriodic(),
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
  if ((aBSpl->IsPeriodic()) && (Index1 == Nb))
    Index1 = 1;

  if ((Index2 - Index1 <= 0) && (!aBSpl->IsPeriodic()))
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
    MultMax = aBSpl->Degree() - MultMax;
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

void GeomAdaptor_Curve::Reset()
{
  myTypeCurve = GeomAbs_OtherCurve;
  myCurve.Nullify();
  myCurveData = std::monostate{};
  myFirst = myLast = 0.0;
}

//=================================================================================================

void GeomAdaptor_Curve::load(const occ::handle<Geom_Curve>& C,
                             const double                   UFirst,
                             const double                   ULast)
{
  myFirst = UFirst;
  myLast  = ULast;

  if (myCurve != C)
  {
    myCurve     = C;
    myCurveData = std::monostate{};

    const occ::handle<Standard_Type>& TheType = C->DynamicType();
    if (TheType == STANDARD_TYPE(Geom_TrimmedCurve))
    {
      Load(occ::down_cast<Geom_TrimmedCurve>(C)->BasisCurve(), UFirst, ULast);
    }
    else if (TheType == STANDARD_TYPE(Geom_Circle))
    {
      myTypeCurve = GeomAbs_Circle;
      myCurveData = occ::down_cast<Geom_Circle>(C)->Circ();
    }
    else if (TheType == STANDARD_TYPE(Geom_Line))
    {
      myTypeCurve = GeomAbs_Line;
      myCurveData = occ::down_cast<Geom_Line>(C)->Lin();
    }
    else if (TheType == STANDARD_TYPE(Geom_Ellipse))
    {
      myTypeCurve = GeomAbs_Ellipse;
      myCurveData = occ::down_cast<Geom_Ellipse>(C)->Elips();
    }
    else if (TheType == STANDARD_TYPE(Geom_Parabola))
    {
      myTypeCurve = GeomAbs_Parabola;
      myCurveData = occ::down_cast<Geom_Parabola>(C)->Parab();
    }
    else if (TheType == STANDARD_TYPE(Geom_Hyperbola))
    {
      myTypeCurve = GeomAbs_Hyperbola;
      myCurveData = occ::down_cast<Geom_Hyperbola>(C)->Hypr();
    }
    else if (TheType == STANDARD_TYPE(Geom_BezierCurve))
    {
      myTypeCurve = GeomAbs_BezierCurve;
      BezierData aBezierData;
      aBezierData.Curve   = occ::down_cast<Geom_BezierCurve>(C);
      aBezierData.EvalRep = aBezierData.Curve->EvalRepresentation();
      myCurveData         = std::move(aBezierData);
    }
    else if (TheType == STANDARD_TYPE(Geom_BSplineCurve))
    {
      myTypeCurve = GeomAbs_BSplineCurve;
      BSplineData aBSplineData;
      aBSplineData.Curve   = occ::down_cast<Geom_BSplineCurve>(C);
      aBSplineData.EvalRep = aBSplineData.Curve->EvalRepresentation();
      myCurveData          = std::move(aBSplineData);
    }
    else if (TheType == STANDARD_TYPE(Geom_OffsetCurve))
    {
      myTypeCurve                                 = GeomAbs_OffsetCurve;
      occ::handle<Geom_OffsetCurve> anOffsetCurve = occ::down_cast<Geom_OffsetCurve>(C);
      OffsetData                    anOffsetData;
      anOffsetData.BasisAdaptor = new GeomAdaptor_Curve(anOffsetCurve->BasisCurve());
      anOffsetData.Offset       = anOffsetCurve->Offset();
      anOffsetData.Direction    = anOffsetCurve->Direction();
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
    // Same curve, but need to invalidate cache if bounds changed
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

GeomAbs_Shape GeomAdaptor_Curve::Continuity() const
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
    return LocalContinuity(myFirst, myLast);

  if (myTypeCurve == GeomAbs_OffsetCurve)
  {
    const GeomAbs_Shape S = occ::down_cast<Geom_OffsetCurve>(myCurve)->GetBasisCurveContinuity();
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
        throw Standard_NoSuchObject("GeomAdaptor_Curve::Continuity");
    }
  }
  else if (myTypeCurve == GeomAbs_OtherCurve)
  {
    throw Standard_NoSuchObject("GeomAdaptor_Curve::Contunuity");
  }

  return GeomAbs_CN;
}

//=================================================================================================

int GeomAdaptor_Curve::NbIntervals(const GeomAbs_Shape S) const
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    const auto& aBSpl = std::get<BSplineData>(myCurveData).Curve;
    if ((!aBSpl->IsPeriodic() && S <= Continuity()) || S == GeomAbs_C0)
    {
      return 1;
    }

    int aDegree = aBSpl->Degree();
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
        throw Standard_DomainError("GeomAdaptor_Curve::NbIntervals()");
    }

    double anEps = std::min(Resolution(Precision::Confusion()), Precision::PConfusion());

    return BSplCLib::Intervals(aBSpl->Knots(),
                               aBSpl->Multiplicities(),
                               aDegree,
                               aBSpl->IsPeriodic(),
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
    GeomAdaptor_Curve C(occ::down_cast<Geom_OffsetCurve>(myCurve)->BasisCurve(), myFirst, myLast);
    // akm 05/04/02 (OCC278)  If our curve is trimmed we must recalculate
    //                    the number of intervals obtained from the basis to
    //              vvv   reflect parameter bounds
    int iNbBasisInt = C.NbIntervals(BaseS), iInt;
    if (iNbBasisInt > 1)
    {
      NCollection_Array1<double> rdfInter(1, 1 + iNbBasisInt);
      C.Intervals(rdfInter, BaseS);
      for (iInt = 1; iInt <= iNbBasisInt; iInt++)
        if (rdfInter(iInt) > myFirst && rdfInter(iInt) < myLast)
          myNbIntervals++;
    }
    // akm 05/04/02 ^^^
    return myNbIntervals;
  }

  else
  {
    return 1;
  }
}

//=================================================================================================

void GeomAdaptor_Curve::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape S) const
{
  if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    const auto& aBSpl = std::get<BSplineData>(myCurveData).Curve;
    if ((!aBSpl->IsPeriodic() && S <= Continuity()) || S == GeomAbs_C0)
    {
      T(T.Lower())     = myFirst;
      T(T.Lower() + 1) = myLast;
      return;
    }

    int aDegree = aBSpl->Degree();
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
        throw Standard_DomainError("GeomAdaptor_Curve::Intervals()");
    }

    double anEps = std::min(Resolution(Precision::Confusion()), Precision::PConfusion());

    BSplCLib::Intervals(aBSpl->Knots(),
                        aBSpl->Multiplicities(),
                        aDegree,
                        aBSpl->IsPeriodic(),
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
    GeomAdaptor_Curve C(occ::down_cast<Geom_OffsetCurve>(myCurve)->BasisCurve(), myFirst, myLast);
    // akm 05/04/02 (OCC278)  If our curve is trimmed we must recalculate
    //                    the array of intervals obtained from the basis to
    //              vvv   reflect parameter bounds
    int iNbBasisInt = C.NbIntervals(BaseS), iInt;
    if (iNbBasisInt > 1)
    {
      NCollection_Array1<double> rdfInter(1, 1 + iNbBasisInt);
      C.Intervals(rdfInter, BaseS);
      for (iInt = 1; iInt <= iNbBasisInt; iInt++)
        if (rdfInter(iInt) > myFirst && rdfInter(iInt) < myLast)
          T(++myNbIntervals) = rdfInter(iInt);
    }
    // old - myNbIntervals = C.NbIntervals(BaseS);
    // old - C.Intervals(T, BaseS);
    // akm 05/04/02 ^^^
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

occ::handle<Adaptor3d_Curve> GeomAdaptor_Curve::Trim(const double First,
                                                     const double Last,
                                                     const double /*Tol*/) const
{
  return occ::handle<GeomAdaptor_Curve>(new GeomAdaptor_Curve(myCurve, First, Last));
}

//=================================================================================================

bool GeomAdaptor_Curve::IsClosed() const
{
  if (!Precision::IsPositiveInfinite(myLast) && !Precision::IsNegativeInfinite(myFirst))
  {
    const gp_Pnt Pd = Value(myFirst);
    const gp_Pnt Pf = Value(myLast);
    return (Pd.Distance(Pf) <= Precision::Confusion());
  }
  return false;
}

//=================================================================================================

bool GeomAdaptor_Curve::IsPeriodic() const
{
  return myCurve->IsPeriodic();
}

//=================================================================================================

double GeomAdaptor_Curve::Period() const
{
  return myCurve->LastParameter() - myCurve->FirstParameter();
}

//=================================================================================================

void GeomAdaptor_Curve::RebuildCache(const double theParameter) const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
  {
    // Create cache for Bezier
    auto&                         aCache  = std::get<BezierData>(myCurveData).Cache;
    occ::handle<Geom_BezierCurve> aBezier = occ::down_cast<Geom_BezierCurve>(myCurve);
    int                           aDeg    = aBezier->Degree();
    NCollection_Array1<double>    aFlatKnots(BSplCLib::FlatBezierKnots(aDeg), 1, 2 * (aDeg + 1));
    if (aCache.IsNull())
      aCache = new BSplCLib_Cache(aDeg,
                                  aBezier->IsPeriodic(),
                                  aFlatKnots,
                                  aBezier->Poles(),
                                  aBezier->Weights());
    aCache->BuildCache(theParameter, aFlatKnots, aBezier->Poles(), aBezier->Weights());
  }
  else if (myTypeCurve == GeomAbs_BSplineCurve)
  {
    // Create cache for B-spline
    auto&       aBSplData = std::get<BSplineData>(myCurveData);
    const auto& aBSpl     = aBSplData.Curve;
    auto&       aCache    = aBSplData.Cache;
    if (aCache.IsNull())
      aCache = new BSplCLib_Cache(aBSpl->Degree(),
                                  aBSpl->IsPeriodic(),
                                  aBSpl->KnotSequence(),
                                  aBSpl->Poles(),
                                  aBSpl->Weights());
    aCache->BuildCache(theParameter, aBSpl->KnotSequence(), aBSpl->Poles(), aBSpl->Weights());
  }
}

//=================================================================================================

bool GeomAdaptor_Curve::IsBoundary(const double theU, int& theSpanStart, int& theSpanFinish) const
{
  const auto* aBSplData = std::get_if<BSplineData>(&myCurveData);
  if (aBSplData != nullptr && (theU == myFirst || theU == myLast))
  {
    const auto& aBSpl = aBSplData->Curve;
    if (theU == myFirst)
    {
      aBSpl->LocateU(myFirst, PosTol, theSpanStart, theSpanFinish);
      if (theSpanStart < 1)
        theSpanStart = 1;
      if (theSpanStart >= theSpanFinish)
        theSpanFinish = theSpanStart + 1;
    }
    else if (theU == myLast)
    {
      aBSpl->LocateU(myLast, PosTol, theSpanStart, theSpanFinish);
      if (theSpanFinish > aBSpl->NbKnots())
        theSpanFinish = aBSpl->NbKnots();
      if (theSpanStart >= theSpanFinish)
        theSpanStart = theSpanFinish - 1;
    }
    return true;
  }
  return false;
}

//=================================================================================================

gp_Pnt GeomAdaptor_Curve::Value(const double U) const
{
  gp_Pnt aValue;
  D0(U, aValue);
  return aValue;
}

//=================================================================================================

void GeomAdaptor_Curve::D0(const double U, gp_Pnt& P) const
{
  P = EvalD0(U);
}

//=================================================================================================

gp_Pnt GeomAdaptor_Curve::EvalD0(double U) const
{
  gp_Pnt P;
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      P = ElCLib::Value(U, std::get<gp_Lin>(myCurveData));
      return P;

    case GeomAbs_Circle:
      P = ElCLib::Value(U, std::get<gp_Circ>(myCurveData));
      return P;

    case GeomAbs_Ellipse:
      P = ElCLib::Value(U, std::get<gp_Elips>(myCurveData));
      return P;

    case GeomAbs_Hyperbola:
      P = ElCLib::Value(U, std::get<gp_Hypr>(myCurveData));
      return P;

    case GeomAbs_Parabola:
      P = ElCLib::Value(U, std::get<gp_Parab>(myCurveData));
      return P;

    case GeomAbs_BezierCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD0(U);
      }
      auto& aCache = std::get<BezierData>(myCurveData).Cache;
      if (aCache.IsNull())
        RebuildCache(U);
      aCache->D0(U, P);
      return P;
    }

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD0(U);
      }
      int   aStart = 0, aFinish = 0;
      auto& aBSplData = std::get<BSplineData>(myCurveData);
      if (IsBoundary(U, aStart, aFinish))
      {
        aBSplData.Curve->LocalD0(U, aStart, aFinish, P);
      }
      else
      {
        if (aBSplData.Cache.IsNull() || !aBSplData.Cache->IsCacheValid(U))
          RebuildCache(U);
        aBSplData.Cache->D0(U, P);
      }
      return P;
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD0(U);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      if (!Geom_OffsetCurveUtils::EvaluateD0(U,
                                             anOffsetData.BasisAdaptor.get(),
                                             anOffsetData.Direction,
                                             anOffsetData.Offset,
                                             P))
      {
        throw Geom_UndefinedValue("GeomAdaptor_Curve::EvalD0: evaluation failed");
      }
      return P;
    }

    default:
      return myCurve->EvalD0(U);
  }
}

//=================================================================================================

void GeomAdaptor_Curve::D1(const double U, gp_Pnt& P, gp_Vec& V) const
{
  const Geom_Curve::ResD1 aResult = EvalD1(U);
  P = aResult.Point;
  V = aResult.D1;
}

//=================================================================================================

Geom_Curve::ResD1 GeomAdaptor_Curve::EvalD1(double U) const
{
  Geom_Curve::ResD1 aResult;
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      ElCLib::D1(U, std::get<gp_Lin>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_Circle:
      ElCLib::D1(U, std::get<gp_Circ>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_Ellipse:
      ElCLib::D1(U, std::get<gp_Elips>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_Hyperbola:
      ElCLib::D1(U, std::get<gp_Hypr>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_Parabola:
      ElCLib::D1(U, std::get<gp_Parab>(myCurveData), aResult.Point, aResult.D1);
      return aResult;

    case GeomAbs_BezierCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD1(U);
      }
      auto& aCache = std::get<BezierData>(myCurveData).Cache;
      if (aCache.IsNull())
        RebuildCache(U);
      aCache->D1(U, aResult.Point, aResult.D1);
      return aResult;
    }

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD1(U);
      }
      int   aStart = 0, aFinish = 0;
      auto& aBSplData = std::get<BSplineData>(myCurveData);
      if (IsBoundary(U, aStart, aFinish))
      {
        aBSplData.Curve->LocalD1(U, aStart, aFinish, aResult.Point, aResult.D1);
      }
      else
      {
        if (aBSplData.Cache.IsNull() || !aBSplData.Cache->IsCacheValid(U))
          RebuildCache(U);
        aBSplData.Cache->D1(U, aResult.Point, aResult.D1);
      }
      return aResult;
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD1(U);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      if (!Geom_OffsetCurveUtils::EvaluateD1(U,
                                             anOffsetData.BasisAdaptor.get(),
                                             anOffsetData.Direction,
                                             anOffsetData.Offset,
                                             aResult.Point,
                                             aResult.D1))
      {
        throw Geom_UndefinedDerivative("GeomAdaptor_Curve::EvalD1: evaluation failed");
      }
      return aResult;
    }

    default:
      return myCurve->EvalD1(U);
  }
}

//=================================================================================================

void GeomAdaptor_Curve::D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{
  const Geom_Curve::ResD2 aResult = EvalD2(U);
  P  = aResult.Point;
  V1 = aResult.D1;
  V2 = aResult.D2;
}

//=================================================================================================

Geom_Curve::ResD2 GeomAdaptor_Curve::EvalD2(double U) const
{
  Geom_Curve::ResD2 aResult;
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      ElCLib::D1(U, std::get<gp_Lin>(myCurveData), aResult.Point, aResult.D1);
      aResult.D2.SetCoord(0., 0., 0.);
      return aResult;

    case GeomAbs_Circle:
      ElCLib::D2(U, std::get<gp_Circ>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      return aResult;

    case GeomAbs_Ellipse:
      ElCLib::D2(U, std::get<gp_Elips>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      return aResult;

    case GeomAbs_Hyperbola:
      ElCLib::D2(U, std::get<gp_Hypr>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      return aResult;

    case GeomAbs_Parabola:
      ElCLib::D2(U, std::get<gp_Parab>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      return aResult;

    case GeomAbs_BezierCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD2(U);
      }
      auto& aCache = std::get<BezierData>(myCurveData).Cache;
      if (aCache.IsNull())
        RebuildCache(U);
      aCache->D2(U, aResult.Point, aResult.D1, aResult.D2);
      return aResult;
    }

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD2(U);
      }
      int   aStart = 0, aFinish = 0;
      auto& aBSplData = std::get<BSplineData>(myCurveData);
      if (IsBoundary(U, aStart, aFinish))
      {
        aBSplData.Curve->LocalD2(U, aStart, aFinish, aResult.Point, aResult.D1, aResult.D2);
      }
      else
      {
        if (aBSplData.Cache.IsNull() || !aBSplData.Cache->IsCacheValid(U))
          RebuildCache(U);
        aBSplData.Cache->D2(U, aResult.Point, aResult.D1, aResult.D2);
      }
      return aResult;
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD2(U);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      if (!Geom_OffsetCurveUtils::EvaluateD2(U,
                                             anOffsetData.BasisAdaptor.get(),
                                             anOffsetData.Direction,
                                             anOffsetData.Offset,
                                             aResult.Point,
                                             aResult.D1,
                                             aResult.D2))
      {
        throw Geom_UndefinedDerivative("GeomAdaptor_Curve::EvalD2: evaluation failed");
      }
      return aResult;
    }

    default:
      return myCurve->EvalD2(U);
  }
}

//=================================================================================================

void GeomAdaptor_Curve::D3(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const
{
  const Geom_Curve::ResD3 aResult = EvalD3(U);
  P  = aResult.Point;
  V1 = aResult.D1;
  V2 = aResult.D2;
  V3 = aResult.D3;
}

//=================================================================================================

Geom_Curve::ResD3 GeomAdaptor_Curve::EvalD3(double U) const
{
  Geom_Curve::ResD3 aResult;
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      ElCLib::D1(U, std::get<gp_Lin>(myCurveData), aResult.Point, aResult.D1);
      aResult.D2.SetCoord(0., 0., 0.);
      aResult.D3.SetCoord(0., 0., 0.);
      return aResult;

    case GeomAbs_Circle:
      ElCLib::D3(U,
                 std::get<gp_Circ>(myCurveData),
                 aResult.Point,
                 aResult.D1,
                 aResult.D2,
                 aResult.D3);
      return aResult;

    case GeomAbs_Ellipse:
      ElCLib::D3(U,
                 std::get<gp_Elips>(myCurveData),
                 aResult.Point,
                 aResult.D1,
                 aResult.D2,
                 aResult.D3);
      return aResult;

    case GeomAbs_Hyperbola:
      ElCLib::D3(U,
                 std::get<gp_Hypr>(myCurveData),
                 aResult.Point,
                 aResult.D1,
                 aResult.D2,
                 aResult.D3);
      return aResult;

    case GeomAbs_Parabola:
      ElCLib::D2(U, std::get<gp_Parab>(myCurveData), aResult.Point, aResult.D1, aResult.D2);
      aResult.D3.SetCoord(0., 0., 0.);
      return aResult;

    case GeomAbs_BezierCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD3(U);
      }
      auto& aCache = std::get<BezierData>(myCurveData).Cache;
      if (aCache.IsNull())
        RebuildCache(U);
      aCache->D3(U, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
      return aResult;
    }

    case GeomAbs_BSplineCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD3(U);
      }
      int   aStart = 0, aFinish = 0;
      auto& aBSplData = std::get<BSplineData>(myCurveData);
      if (IsBoundary(U, aStart, aFinish))
      {
        aBSplData.Curve
          ->LocalD3(U, aStart, aFinish, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
      }
      else
      {
        if (aBSplData.Cache.IsNull() || !aBSplData.Cache->IsCacheValid(U))
          RebuildCache(U);
        aBSplData.Cache->D3(U, aResult.Point, aResult.D1, aResult.D2, aResult.D3);
      }
      return aResult;
    }

    case GeomAbs_OffsetCurve: {
      if (hasEvalRep(myCurveData))
      {
        return myCurve->EvalD3(U);
      }
      const auto& anOffsetData = std::get<OffsetData>(myCurveData);
      if (!Geom_OffsetCurveUtils::EvaluateD3(U,
                                             anOffsetData.BasisAdaptor.get(),
                                             anOffsetData.Direction,
                                             anOffsetData.Offset,
                                             aResult.Point,
                                             aResult.D1,
                                             aResult.D2,
                                             aResult.D3))
      {
        throw Geom_UndefinedDerivative("GeomAdaptor_Curve::EvalD3: evaluation failed");
      }
      return aResult;
    }

    default:
      return myCurve->EvalD3(U);
  }
}

//=================================================================================================

gp_Vec GeomAdaptor_Curve::DN(const double U, const int N) const
{
  return EvalDN(U, N);
}

//=================================================================================================

gp_Vec GeomAdaptor_Curve::EvalDN(double U, int N) const
{
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      return ElCLib::DN(U, std::get<gp_Lin>(myCurveData), N);

    case GeomAbs_Circle:
      return ElCLib::DN(U, std::get<gp_Circ>(myCurveData), N);

    case GeomAbs_Ellipse:
      return ElCLib::DN(U, std::get<gp_Elips>(myCurveData), N);

    case GeomAbs_Hyperbola:
      return ElCLib::DN(U, std::get<gp_Hypr>(myCurveData), N);

    case GeomAbs_Parabola:
      return ElCLib::DN(U, std::get<gp_Parab>(myCurveData), N);

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
      gp_Vec      aDN;
      if (!Geom_OffsetCurveUtils::EvaluateDN(U,
                                             anOffsetData.BasisAdaptor.get(),
                                             anOffsetData.Direction,
                                             anOffsetData.Offset,
                                             N,
                                             aDN))
      {
        throw Geom_UndefinedDerivative("GeomAdaptor_Curve::EvalDN: evaluation failed");
      }
      return aDN;
    }

    default:
      break;
  }
  return myCurve->EvalDN(U, N);
}

//=================================================================================================

double GeomAdaptor_Curve::Resolution(const double R3D) const
{
  switch (myTypeCurve)
  {
    case GeomAbs_Line:
      return R3D;
    case GeomAbs_Circle: {
      double R = std::get<gp_Circ>(myCurveData).Radius();
      if (R > R3D / 2.)
        return 2 * std::asin(R3D / (2 * R));
      else
        return 2 * M_PI;
    }
    case GeomAbs_Ellipse: {
      return R3D / std::get<gp_Elips>(myCurveData).MajorRadius();
    }
    case GeomAbs_BezierCurve: {
      double res;
      occ::down_cast<Geom_BezierCurve>(myCurve)->Resolution(R3D, res);
      return res;
    }
    case GeomAbs_BSplineCurve: {
      double res;
      std::get<BSplineData>(myCurveData).Curve->Resolution(R3D, res);
      return res;
    }
    default:
      return Precision::Parametric(R3D);
  }
}

//    --
//    --     The following methods must  be called when GetType returned
//    --     the corresponding type.
//    --

//=================================================================================================

gp_Lin GeomAdaptor_Curve::Line() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Line,
                                 "GeomAdaptor_Curve::Line() - curve is not a Line");
  return std::get<gp_Lin>(myCurveData);
}

//=================================================================================================

gp_Circ GeomAdaptor_Curve::Circle() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Circle,
                                 "GeomAdaptor_Curve::Circle() - curve is not a Circle");
  return std::get<gp_Circ>(myCurveData);
}

//=================================================================================================

gp_Elips GeomAdaptor_Curve::Ellipse() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Ellipse,
                                 "GeomAdaptor_Curve::Ellipse() - curve is not an Ellipse");
  return std::get<gp_Elips>(myCurveData);
}

//=================================================================================================

gp_Hypr GeomAdaptor_Curve::Hyperbola() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Hyperbola,
                                 "GeomAdaptor_Curve::Hyperbola() - curve is not a Hyperbola");
  return std::get<gp_Hypr>(myCurveData);
}

//=================================================================================================

gp_Parab GeomAdaptor_Curve::Parabola() const
{
  Standard_NoSuchObject_Raise_if(myTypeCurve != GeomAbs_Parabola,
                                 "GeomAdaptor_Curve::Parabola() - curve is not a Parabola");
  return std::get<gp_Parab>(myCurveData);
}

//=================================================================================================

int GeomAdaptor_Curve::Degree() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return occ::down_cast<Geom_BezierCurve>(myCurve)->Degree();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return std::get<BSplineData>(myCurveData).Curve->Degree();
  else
    throw Standard_NoSuchObject();
}

//=================================================================================================

bool GeomAdaptor_Curve::IsRational() const
{
  switch (myTypeCurve)
  {
    case GeomAbs_BSplineCurve:
      return std::get<BSplineData>(myCurveData).Curve->IsRational();
    case GeomAbs_BezierCurve:
      return occ::down_cast<Geom_BezierCurve>(myCurve)->IsRational();
    default:
      return false;
  }
}

//=================================================================================================

int GeomAdaptor_Curve::NbPoles() const
{
  if (myTypeCurve == GeomAbs_BezierCurve)
    return occ::down_cast<Geom_BezierCurve>(myCurve)->NbPoles();
  else if (myTypeCurve == GeomAbs_BSplineCurve)
    return std::get<BSplineData>(myCurveData).Curve->NbPoles();
  else
    throw Standard_NoSuchObject();
}

//=================================================================================================

int GeomAdaptor_Curve::NbKnots() const
{
  if (myTypeCurve != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("GeomAdaptor_Curve::NbKnots");
  return std::get<BSplineData>(myCurveData).Curve->NbKnots();
}

//=================================================================================================

occ::handle<Geom_BezierCurve> GeomAdaptor_Curve::Bezier() const
{
  if (myTypeCurve != GeomAbs_BezierCurve)
    throw Standard_NoSuchObject("GeomAdaptor_Curve::Bezier");
  return occ::down_cast<Geom_BezierCurve>(myCurve);
}

//=================================================================================================

occ::handle<Geom_BSplineCurve> GeomAdaptor_Curve::BSpline() const
{
  if (myTypeCurve != GeomAbs_BSplineCurve)
    throw Standard_NoSuchObject("GeomAdaptor_Curve::BSpline");

  return std::get<BSplineData>(myCurveData).Curve;
}

//=================================================================================================

occ::handle<Geom_OffsetCurve> GeomAdaptor_Curve::OffsetCurve() const
{
  if (myTypeCurve != GeomAbs_OffsetCurve)
    throw Standard_NoSuchObject("GeomAdaptor_Curve::OffsetCurve");
  return occ::down_cast<Geom_OffsetCurve>(myCurve);
}
