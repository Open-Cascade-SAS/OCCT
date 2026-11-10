// Created on: 1994-02-17
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BSplCLib.hxx>
#include <Geom_Conic.hxx>
#include <Geom_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_ApproxCurve.hxx>
#include <GeomFill_Profiler.hxx>
#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>

#include <utility>
#include <algorithm>

namespace
{
// Keep section blocks small for profiles with only two curves.
constexpr size_t THE_CURVE_BLOCK_SIZE = 16;

//==================================================================================================

void UnifyByInsertingAllKnots(const NCollection_DynamicArray<occ::handle<Geom_Curve>>& theCurves,
                              const double                                             PTol)
{
  // inserting in the first curve the knot-vector of all the others.
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(theCurves.First());

  for (size_t aCurveIndex = 1; aCurveIndex < theCurves.Size(); ++aCurveIndex)
  {
    occ::handle<Geom_BSplineCurve> Ci = occ::down_cast<Geom_BSplineCurve>(theCurves(aCurveIndex));
    const NCollection_Array1<double>& Ki = Ci->Knots();
    const NCollection_Array1<int>&    Mi = Ci->Multiplicities();

    C->InsertKnots(Ki, Mi, PTol, false);
  }

  const NCollection_Array1<double>& NewKnots = C->Knots();
  const NCollection_Array1<int>&    NewMults = C->Multiplicities();
  for (size_t aCurveIndex = 1; aCurveIndex < theCurves.Size(); ++aCurveIndex)
  {
    occ::handle<Geom_BSplineCurve> Ci = occ::down_cast<Geom_BSplineCurve>(theCurves(aCurveIndex));
    Ci->InsertKnots(NewKnots, NewMults, PTol, false);
  }

  // Normalize the weights of each rational section to mean one.
  for (const occ::handle<Geom_Curve>& aSection : theCurves)
  {
    const occ::handle<Geom_BSplineCurve> aCurve = occ::down_cast<Geom_BSplineCurve>(aSection);
    if (aCurve->IsRational())
    {
      const int aNbPoles      = aCurve->NbPoles();
      double    aMaxWeight    = aCurve->Weight(1);
      double    aMinWeight    = aMaxWeight;
      int       aMaxPoleIndex = 1;
      int       aMinPoleIndex = 1;
      double    aScaledSum    = 0.;
      {
        const NCollection_Array1<double>& aWeights = aCurve->WeightsArray();
        for (int aPoleIndex = 1; aPoleIndex <= aNbPoles; ++aPoleIndex)
        {
          const double aWeight = aWeights.At(static_cast<size_t>(aPoleIndex - 1));
          if (aWeight > aMaxWeight)
          {
            aMaxWeight    = aWeight;
            aMaxPoleIndex = aPoleIndex;
          }
          if (aWeight < aMinWeight)
          {
            aMinWeight    = aWeight;
            aMinPoleIndex = aPoleIndex;
          }
        }
        // Scale before summation so a finite mean does not require a finite sum.
        for (const double aWeight : aWeights)
        {
          aScaledSum += aWeight / aMaxWeight;
        }
      }
      const double aMean = aMaxWeight * (aScaledSum / aNbPoles);
      if (aMean == 1.)
      {
        continue;
      }
      // Keep an extreme weight until last so intermediate weights cannot become uniform.
      // SetWeight replaces a non-rational curve's weights by unit weights.
      const int aLastPoleIndex = aMean > 1. ? aMaxPoleIndex : aMinPoleIndex;
      for (int aPoleIndex = 1; aPoleIndex <= aNbPoles; ++aPoleIndex)
      {
        if (aPoleIndex != aLastPoleIndex)
        {
          aCurve->SetWeight(aPoleIndex, aCurve->Weight(aPoleIndex) / aMean);
        }
      }
      aCurve->SetWeight(aLastPoleIndex, aCurve->Weight(aLastPoleIndex) / aMean);
    }
  }
}

//==================================================================================================

void UnifyBySettingMiddleKnots(const NCollection_DynamicArray<occ::handle<Geom_Curve>>& theCurves)
{
  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(theCurves.First());

  int    NbKnots = C->NbKnots();
  double ULast   = C->Knot(C->LastUKnotIndex());
  double UFirst  = C->Knot(C->FirstUKnotIndex());

  // Set middle values of knots
  NCollection_Array1<double> NewKnots(1, NbKnots);
  NewKnots(1)       = UFirst;
  NewKnots(NbKnots) = ULast;
  for (int aKnotIndex = 2; aKnotIndex < NbKnots; aKnotIndex++)
  {
    double aMidKnot = 0.;
    for (const occ::handle<Geom_Curve>& aSection : theCurves)
    {
      occ::handle<Geom_BSplineCurve> Ctemp = occ::down_cast<Geom_BSplineCurve>(aSection);
      aMidKnot += Ctemp->Knot(aKnotIndex);
    }
    aMidKnot /= static_cast<double>(theCurves.Size());
    NewKnots(aKnotIndex) = aMidKnot;
  }

  for (const occ::handle<Geom_Curve>& aSection : theCurves)
  {
    occ::handle<Geom_BSplineCurve> Cres = occ::down_cast<Geom_BSplineCurve>(aSection);
    Cres->SetKnots(NewKnots);
  }
}

} // namespace

//=================================================================================================

GeomFill_Profiler::GeomFill_Profiler()
    : myCurves(THE_CURVE_BLOCK_SIZE)
{
  myIsDone     = false;
  myIsPeriodic = true;
}

//=======================================================================

GeomFill_Profiler::~GeomFill_Profiler() = default;

//=================================================================================================

void GeomFill_Profiler::AddCurve(const occ::handle<Geom_Curve>& Curve)
{
  occ::handle<Geom_Curve> C;
  //// modified by jgv, 19.01.05 for OCC7354 ////
  occ::handle<Geom_Curve> theCurve = Curve;
  if (theCurve->IsInstance(STANDARD_TYPE(Geom_TrimmedCurve)))
  {
    theCurve = occ::down_cast<Geom_TrimmedCurve>(theCurve)->BasisCurve();
  }
  if (theCurve->IsKind(STANDARD_TYPE(Geom_Conic)))
  {
    GeomConvert_ApproxCurve appr(Curve, Precision::Confusion(), GeomAbs_C1, 16, 14);
    if (appr.HasResult())
    {
      C = appr.Curve();
    }
  }
  if (C.IsNull())
  {
    C = GeomConvert::CurveToBSplineCurve(Curve);
  }
  /*
  if ( Curve->IsKind(STANDARD_TYPE(Geom_BSplineCurve))) {
    C = occ::down_cast<Geom_Curve>(Curve->Copy());
  }
  else {
    C = GeomConvert::CurveToBSplineCurve(Curve,Convert_QuasiAngular);
  }
  */
  ///////////////////////////////////////////////

  myCurves.Append(C);

  if (myIsPeriodic && !C->IsPeriodic())
  {
    myIsPeriodic = false;
  }
}

//=================================================================================================

void GeomFill_Profiler::Perform(const double PTol)
{
  //  int myDegree = 0, myNbPoles = 0;
  int                            myDegree = 0;
  occ::handle<Geom_BSplineCurve> C;
  double                         U1, U2, UFirst = 0, ULast = 0;
  double                         EcartMax = 0.;

  for (const occ::handle<Geom_Curve>& aSection : myCurves)
  {
    C = occ::down_cast<Geom_BSplineCurve>(aSection);

    // If the profile is not periodic, convert periodic curves to non-periodic form.
    // Segment them to give the end knots multiplicity degree + 1.

    U2 = C->Knot(C->LastUKnotIndex());
    U1 = C->Knot(C->FirstUKnotIndex());

    if (!myIsPeriodic && C->IsPeriodic())
    {
      C->SetNotPeriodic();
      C->Segment(U1, U2);
    }

    // evaluate the max degree
    myDegree = std::max(myDegree, C->Degree());

    // Select the largest parameter span among all curves.
    if ((U2 - U1) > EcartMax)
    {
      EcartMax = U2 - U1;
      UFirst   = U1;
      ULast    = U2;
    }
  }

  // increase the degree of the curves to my degree
  // reparametrize them in the range U1, U2.
  NCollection_Array1<double> aKnots;
  for (const occ::handle<Geom_Curve>& aSection : myCurves)
  {
    C = occ::down_cast<Geom_BSplineCurve>(aSection);

    C->IncreaseDegree(myDegree);

    const NCollection_Array1<double>& aSourceKnots = C->Knots();
    aKnots.Resize(aSourceKnots.Lower(), aSourceKnots.Upper(), false);
    aKnots.CopyValues(aSourceKnots);
    BSplCLib::Reparametrize(UFirst, ULast, aKnots);
    C->SetKnots(aKnots);
  }

  NCollection_DynamicArray<occ::handle<Geom_Curve>> aCurves(THE_CURVE_BLOCK_SIZE,
                                                            myCurves.Allocator());
  for (const occ::handle<Geom_Curve>& aSection : myCurves)
  {
    aCurves.Append(occ::down_cast<Geom_Curve>(aSection->Copy()));
  }

  UnifyByInsertingAllKnots(aCurves, PTol);

  bool Unified    = true;
  int  theNbKnots = (occ::down_cast<Geom_BSplineCurve>(aCurves.First()))->NbKnots();
  for (size_t aCurveIndex = 1; aCurveIndex < aCurves.Size(); ++aCurveIndex)
  {
    if ((occ::down_cast<Geom_BSplineCurve>(aCurves(aCurveIndex)))->NbKnots() != theNbKnots)
    {
      Unified = false;
      break;
    }
  }

  if (Unified)
  {
    myCurves = std::move(aCurves);
  }
  else
  {
    UnifyBySettingMiddleKnots(myCurves);
  }

  myIsDone = true;
}

//=================================================================================================

int GeomFill_Profiler::Degree() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_Profiler::Degree");
  }

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(myCurves.First());
  return C->Degree();
}

//=================================================================================================

int GeomFill_Profiler::NbPoles() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_Profiler::Degree");
  }

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(myCurves.First());
  return C->NbPoles();
}

//=================================================================================================

void GeomFill_Profiler::Poles(const int Index, NCollection_Array1<gp_Pnt>& Poles) const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_Profiler::Degree");
  }

  Standard_DomainError_Raise_if(Poles.Length() != NbPoles(), "GeomFill_Profiler::Poles");
  Standard_DomainError_Raise_if(Index < 1 || static_cast<size_t>(Index) > myCurves.Size(),
                                "GeomFill_Profiler::Poles");

  occ::handle<Geom_BSplineCurve> C =
    occ::down_cast<Geom_BSplineCurve>(myCurves.Value(static_cast<size_t>(Index) - 1));

  Poles = C->Poles();
}

//=================================================================================================

void GeomFill_Profiler::Weights(const int Index, NCollection_Array1<double>& Weights) const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_Profiler::Degree");
  }

  Standard_DomainError_Raise_if(Weights.Length() != NbPoles(), "GeomFill_Profiler::Weights");
  Standard_DomainError_Raise_if(Index < 1 || static_cast<size_t>(Index) > myCurves.Size(),
                                "GeomFill_Profiler::Weights");

  occ::handle<Geom_BSplineCurve> C =
    occ::down_cast<Geom_BSplineCurve>(myCurves.Value(static_cast<size_t>(Index) - 1));

  Weights = C->WeightsArray();
}

//=================================================================================================

int GeomFill_Profiler::NbKnots() const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_Profiler::Degree");
  }

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(myCurves.First());

  return C->NbKnots();
}

//=================================================================================================

void GeomFill_Profiler::KnotsAndMults(NCollection_Array1<double>& Knots,
                                      NCollection_Array1<int>&    Mults) const
{
  if (!myIsDone)
  {
    throw StdFail_NotDone("GeomFill_Profiler::Degree");
  }

#ifndef No_Exception
  int n = NbKnots();
#endif
  Standard_DomainError_Raise_if(Knots.Length() != n || Mults.Length() != n,
                                "GeomFill_Profiler::KnotsAndMults");

  occ::handle<Geom_BSplineCurve> C = occ::down_cast<Geom_BSplineCurve>(myCurves.First());

  Knots = C->Knots();
  Mults = C->Multiplicities();
}
