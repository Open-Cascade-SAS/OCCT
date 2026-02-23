// Created on: 1991-07-05
// Created by: JCV
// Copyright (c) 1991-1999 Matra Datavision
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
#include <Geom_BSplineCurve.hxx>
#include <GeomEval_RepCurveDesc.hxx>
#include <GeomEval_RepUtils.pxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>

//=================================================================================================

bool Geom_BSplineCurve::IsCN(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, "Geom_BSplineCurve::IsCN");

  switch (mySmooth)
  {
    case GeomAbs_CN:
      return true;
    case GeomAbs_C0:
      return N <= 0;
    case GeomAbs_G1:
      return N <= 0;
    case GeomAbs_C1:
      return N <= 1;
    case GeomAbs_G2:
      return N <= 1;
    case GeomAbs_C2:
      return N <= 2;
    case GeomAbs_C3:
      return N <= 3
               ? true
               : N <= myDeg
                        - BSplCLib::MaxKnotMult(myMults, myMults.Lower() + 1, myMults.Upper() - 1);
    default:
      return false;
  }
}

//=================================================================================================

bool Geom_BSplineCurve::IsG1(const double theTf, const double theTl, const double theAngTol) const
{
  if (IsCN(1))
  {
    return true;
  }

  int start = FirstUKnotIndex() + 1, finish = LastUKnotIndex() - 1;
  int aDeg = Degree();
  for (int aNKnot = start; aNKnot <= finish; aNKnot++)
  {
    const double aTpar = Knot(aNKnot);

    if (aTpar < theTf)
      continue;
    if (aTpar > theTl)
      break;

    int mult = Multiplicity(aNKnot);
    if (mult < aDeg)
      continue;

    gp_Pnt aP1, aP2;
    gp_Vec aV1, aV2;
    LocalD1(aTpar, aNKnot - 1, aNKnot, aP1, aV1);
    LocalD1(aTpar, aNKnot, aNKnot + 1, aP2, aV2);

    if ((aV1.SquareMagnitude() <= gp::Resolution()) || aV2.SquareMagnitude() <= gp::Resolution())
    {
      return false;
    }

    if (std::abs(aV1.Angle(aV2)) > theAngTol)
      return false;
  }

  if (!IsPeriodic())
    return true;

  const double aFirstParam = FirstParameter(), aLastParam = LastParameter();

  if (((aFirstParam - theTf) * (theTl - aFirstParam) < 0.0)
      && ((aLastParam - theTf) * (theTl - aLastParam) < 0.0))
  {
    // Range [theTf, theTl] does not intersect curve boundaries
    return true;
  }

  // Curve is closed or periodic and range [theTf, theTl]
  // intersect curve boundary. Therefore, it is necessary to
  // check if curve is smooth in its first and last point.

  gp_Pnt aP;
  gp_Vec aV1, aV2;
  Geom_Curve::D1(Knot(FirstUKnotIndex()), aP, aV1);
  Geom_Curve::D1(Knot(LastUKnotIndex()), aP, aV2);

  if ((aV1.SquareMagnitude() <= gp::Resolution()) || aV2.SquareMagnitude() <= gp::Resolution())
  {
    return false;
  }

  if (std::abs(aV1.Angle(aV2)) > theAngTol)
    return false;

  return true;
}

//=================================================================================================

bool Geom_BSplineCurve::IsClosed() const
//-- { return (StartPoint().Distance (EndPoint())) <= gp::Resolution (); }
{
  return (StartPoint().SquareDistance(EndPoint())) <= 1e-16;
}

//=================================================================================================

bool Geom_BSplineCurve::IsPeriodic() const
{
  return myPeriodic;
}

//=================================================================================================

GeomAbs_Shape Geom_BSplineCurve::Continuity() const
{
  return mySmooth;
}

//=================================================================================================

int Geom_BSplineCurve::Degree() const
{
  return myDeg;
}

//=================================================================================================

gp_Pnt Geom_BSplineCurve::EvalD0(const double U) const
{
  gp_Pnt aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalCurveD0(myEvalRep, U, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  gp_Pnt P;
  int    aSpanIndex = 0;
  double aNewU(U);
  PeriodicNormalization(aNewU);
  BSplCLib::LocateParameter(myDeg, myKnots, &myMults, U, myPeriodic, aSpanIndex, aNewU);
  if (aNewU < myKnots(aSpanIndex))
    aSpanIndex--;

  BSplCLib::D0(aNewU, aSpanIndex, myDeg, myPeriodic, myPoles, Weights(), myKnots, &myMults, P);
  return P;
}

//=================================================================================================

Geom_Curve::ResD1 Geom_BSplineCurve::EvalD1(const double U) const
{
  Geom_Curve::ResD1 aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalCurveD1(myEvalRep, U, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  Geom_Curve::ResD1 aResult;
  int               aSpanIndex = 0;
  double            aNewU(U);
  PeriodicNormalization(aNewU);
  BSplCLib::LocateParameter(myDeg, myKnots, &myMults, U, myPeriodic, aSpanIndex, aNewU);
  if (aNewU < myKnots(aSpanIndex))
    aSpanIndex--;

  BSplCLib::D1(aNewU,
               aSpanIndex,
               myDeg,
               myPeriodic,
               myPoles,
               Weights(),
               myKnots,
               &myMults,
               aResult.Point,
               aResult.D1);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD2 Geom_BSplineCurve::EvalD2(const double U) const
{
  Geom_Curve::ResD2 aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalCurveD2(myEvalRep, U, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  Geom_Curve::ResD2 aResult;
  int               aSpanIndex = 0;
  double            aNewU(U);
  PeriodicNormalization(aNewU);
  BSplCLib::LocateParameter(myDeg, myKnots, &myMults, U, myPeriodic, aSpanIndex, aNewU);
  if (aNewU < myKnots(aSpanIndex))
    aSpanIndex--;

  BSplCLib::D2(aNewU,
               aSpanIndex,
               myDeg,
               myPeriodic,
               myPoles,
               Weights(),
               myKnots,
               &myMults,
               aResult.Point,
               aResult.D1,
               aResult.D2);
  return aResult;
}

//=================================================================================================

Geom_Curve::ResD3 Geom_BSplineCurve::EvalD3(const double U) const
{
  Geom_Curve::ResD3 aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalCurveD3(myEvalRep, U, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  Geom_Curve::ResD3 aResult;
  int               aSpanIndex = 0;
  double            aNewU(U);
  PeriodicNormalization(aNewU);
  BSplCLib::LocateParameter(myDeg, myKnots, &myMults, U, myPeriodic, aSpanIndex, aNewU);
  if (aNewU < myKnots(aSpanIndex))
    aSpanIndex--;

  BSplCLib::D3(aNewU,
               aSpanIndex,
               myDeg,
               myPeriodic,
               myPoles,
               Weights(),
               myKnots,
               &myMults,
               aResult.Point,
               aResult.D1,
               aResult.D2,
               aResult.D3);
  return aResult;
}

//=================================================================================================

gp_Vec Geom_BSplineCurve::EvalDN(const double U, const int N) const
{
  if (N < 1)
    throw Geom_UndefinedDerivative();

  gp_Vec aEvalRepResult;
  if (GeomEval_RepUtils::TryEvalCurveDN(myEvalRep, U, N, aEvalRepResult))
  {
    return aEvalRepResult;
  }

  gp_Vec V;
  BSplCLib::DN(U, N, 0, myDeg, myPeriodic, myPoles, Weights(), myFlatKnots, BSplCLib::NoMults(), V);
  return V;
}

//=================================================================================================

gp_Pnt Geom_BSplineCurve::EndPoint() const
{
  if (myMults(myKnots.Upper()) == myDeg + 1)
    return myPoles(myPoles.Upper());
  else
    return Value(LastParameter());
}

//=================================================================================================

int Geom_BSplineCurve::FirstUKnotIndex() const
{
  if (myPeriodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(myDeg, myMults);
}

//=================================================================================================

double Geom_BSplineCurve::FirstParameter() const
{
  return myFlatKnots(myDeg + 1);
}

//=================================================================================================

double Geom_BSplineCurve::Knot(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myKnots.Length(), "Geom_BSplineCurve::Knot");
  return myKnots(Index);
}

//=================================================================================================

GeomAbs_BSplKnotDistribution Geom_BSplineCurve::KnotDistribution() const
{
  return myKnotSet;
}

//=================================================================================================

void Geom_BSplineCurve::Knots(NCollection_Array1<double>& K) const
{
  Standard_DomainError_Raise_if(K.Lower() < myKnots.Lower() || K.Upper() > myKnots.Upper(),
                                "Geom_BSplineCurve::Knots");
  for (int anIdx = K.Lower(); anIdx <= K.Upper(); anIdx++)
    K(anIdx) = myKnots(anIdx);
}

const NCollection_Array1<double>& Geom_BSplineCurve::Knots() const
{
  return myKnots;
}

//=================================================================================================

void Geom_BSplineCurve::KnotSequence(NCollection_Array1<double>& K) const
{
  Standard_DomainError_Raise_if(K.Lower() < myFlatKnots.Lower() || K.Upper() > myFlatKnots.Upper(),
                                "Geom_BSplineCurve::KnotSequence");
  for (int anIdx = K.Lower(); anIdx <= K.Upper(); anIdx++)
    K(anIdx) = myFlatKnots(anIdx);
}

const NCollection_Array1<double>& Geom_BSplineCurve::KnotSequence() const
{
  return myFlatKnots;
}

//=================================================================================================

int Geom_BSplineCurve::LastUKnotIndex() const
{
  if (myPeriodic)
    return myKnots.Length();
  else
    return BSplCLib::LastUKnotIndex(myDeg, myMults);
}

//=================================================================================================

double Geom_BSplineCurve::LastParameter() const
{
  return myFlatKnots(myFlatKnots.Upper() - myDeg);
}

//=================================================================================================

gp_Pnt Geom_BSplineCurve::LocalValue(const double U, const int FromK1, const int ToK2) const
{
  gp_Pnt P;
  LocalD0(U, FromK1, ToK2, P);
  return P;
}

//=================================================================================================

void Geom_BSplineCurve::LocalD0(const double U, const int FromK1, const int ToK2, gp_Pnt& P) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Geom_BSplineCurve::LocalValue");

  double u     = U;
  int    index = 0;
  BSplCLib::LocateParameter(myDeg, myFlatKnots, U, myPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myDeg, index, myMults, myPeriodic);
  BSplCLib::D0(u,
               index,
               myDeg,
               myPeriodic,
               myPoles,
               Weights(),
               myFlatKnots,
               BSplCLib::NoMults(),
               P);
}

//=================================================================================================

void Geom_BSplineCurve::LocalD1(const double U,
                                const int    FromK1,
                                const int    ToK2,
                                gp_Pnt&      P,
                                gp_Vec&      V1) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Geom_BSplineCurve::LocalD1");

  double u     = U;
  int    index = 0;
  BSplCLib::LocateParameter(myDeg, myFlatKnots, U, myPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myDeg, index, myMults, myPeriodic);
  BSplCLib::D1(u,
               index,
               myDeg,
               myPeriodic,
               myPoles,
               Weights(),
               myFlatKnots,
               BSplCLib::NoMults(),
               P,
               V1);
}

//=================================================================================================

void Geom_BSplineCurve::LocalD2(const double U,
                                const int    FromK1,
                                const int    ToK2,
                                gp_Pnt&      P,
                                gp_Vec&      V1,
                                gp_Vec&      V2) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Geom_BSplineCurve::LocalD2");

  double u     = U;
  int    index = 0;
  BSplCLib::LocateParameter(myDeg, myFlatKnots, U, myPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myDeg, index, myMults, myPeriodic);
  BSplCLib::D2(u,
               index,
               myDeg,
               myPeriodic,
               myPoles,
               Weights(),
               myFlatKnots,
               BSplCLib::NoMults(),
               P,
               V1,
               V2);
}

//=================================================================================================

void Geom_BSplineCurve::LocalD3(const double U,
                                const int    FromK1,
                                const int    ToK2,
                                gp_Pnt&      P,
                                gp_Vec&      V1,
                                gp_Vec&      V2,
                                gp_Vec&      V3) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Geom_BSplineCurve::LocalD3");

  double u     = U;
  int    index = 0;
  BSplCLib::LocateParameter(myDeg, myFlatKnots, U, myPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myDeg, index, myMults, myPeriodic);
  BSplCLib::D3(u,
               index,
               myDeg,
               myPeriodic,
               myPoles,
               Weights(),
               myFlatKnots,
               BSplCLib::NoMults(),
               P,
               V1,
               V2,
               V3);
}

//=================================================================================================

gp_Vec Geom_BSplineCurve::LocalDN(const double U,
                                  const int    FromK1,
                                  const int    ToK2,
                                  const int    N) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Geom_BSplineCurve::LocalD3");

  double u     = U;
  int    index = 0;
  BSplCLib::LocateParameter(myDeg, myFlatKnots, U, myPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myDeg, index, myMults, myPeriodic);

  gp_Vec V;
  BSplCLib::DN(u,
               N,
               index,
               myDeg,
               myPeriodic,
               myPoles,
               Weights(),
               myFlatKnots,
               BSplCLib::NoMults(),
               V);
  return V;
}

//=================================================================================================

int Geom_BSplineCurve::Multiplicity(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myMults.Length(),
                               "Geom_BSplineCurve::Multiplicity");
  return myMults(Index);
}

//=================================================================================================

void Geom_BSplineCurve::Multiplicities(NCollection_Array1<int>& M) const
{
  Standard_DimensionError_Raise_if(M.Length() != myMults.Length(),
                                   "Geom_BSplineCurve::Multiplicities");
  M = myMults;
}

const NCollection_Array1<int>& Geom_BSplineCurve::Multiplicities() const
{
  return myMults;
}

//=================================================================================================

int Geom_BSplineCurve::NbKnots() const
{
  return myKnots.Length();
}

//=================================================================================================

int Geom_BSplineCurve::NbPoles() const
{
  return myPoles.Length();
}

//=================================================================================================

const gp_Pnt& Geom_BSplineCurve::Pole(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myPoles.Length(), "Geom_BSplineCurve::Pole");
  return myPoles(Index);
}

//=================================================================================================

void Geom_BSplineCurve::Poles(NCollection_Array1<gp_Pnt>& P) const
{
  Standard_DimensionError_Raise_if(P.Length() != myPoles.Length(), "Geom_BSplineCurve::Poles");
  P = myPoles;
}

const NCollection_Array1<gp_Pnt>& Geom_BSplineCurve::Poles() const
{
  return myPoles;
}

//=================================================================================================

gp_Pnt Geom_BSplineCurve::StartPoint() const
{
  if (myMults(1) == myDeg + 1)
    return myPoles(1);
  else
    return Value(FirstParameter());
}

//=================================================================================================

double Geom_BSplineCurve::Weight(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myPoles.Length(), "Geom_BSplineCurve::Weight");
  if (IsRational())
    return myWeights(Index);
  else
    return 1.;
}

//=================================================================================================

void Geom_BSplineCurve::Weights(NCollection_Array1<double>& W) const
{
  Standard_DimensionError_Raise_if(W.Length() != myPoles.Length(), "Geom_BSplineCurve::Weights");
  if (IsRational())
    W = myWeights;
  else
  {
    int i;

    for (i = W.Lower(); i <= W.Upper(); i++)
      W(i) = 1.;
  }
}

const NCollection_Array1<double>* Geom_BSplineCurve::Weights() const
{
  if (IsRational())
    return &myWeights;
  return BSplCLib::NoWeights();
}

//=================================================================================================

bool Geom_BSplineCurve::IsRational() const
{
  return myRational;
}

//=================================================================================================

void Geom_BSplineCurve::Transform(const gp_Trsf& T)
{
  ClearEvalRepresentation();
  for (int I = 1; I <= myPoles.Length(); I++)
    myPoles.ChangeValue(I).Transform(T);
  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom_BSplineCurve::LocateU(const double U,
                                const double ParametricTolerance,
                                int&         I1,
                                int&         I2,
                                const bool   WithKnotRepetition) const
{
  double                            NewU   = U;
  const NCollection_Array1<double>& CKnots = WithKnotRepetition ? myFlatKnots : myKnots;

  PeriodicNormalization(NewU); // Attention a la periode

  double UFirst               = CKnots(1);
  double ULast                = CKnots(CKnots.Length());
  double PParametricTolerance = std::abs(ParametricTolerance);
  if (std::abs(NewU - UFirst) <= PParametricTolerance)
  {
    I1 = I2 = 1;
  }
  else if (std::abs(NewU - ULast) <= PParametricTolerance)
  {
    I1 = I2 = CKnots.Length();
  }
  else if (NewU < UFirst)
  {
    I2 = 1;
    I1 = 0;
  }
  else if (NewU > ULast)
  {
    I1 = CKnots.Length();
    I2 = I1 + 1;
  }
  else
  {
    I1 = 1;
    BSplCLib::Hunt(CKnots, NewU, I1);
    I1 = std::max(std::min(I1, CKnots.Upper()), CKnots.Lower());
    while (I1 + 1 <= CKnots.Upper() && std::abs(CKnots(I1 + 1) - NewU) <= PParametricTolerance)
    {
      I1++;
    }
    if (std::abs(CKnots(I1) - NewU) <= PParametricTolerance)
    {
      I2 = I1;
    }
    else
    {
      I2 = I1 + 1;
    }
  }
}

//=================================================================================================

void Geom_BSplineCurve::Resolution(const double Tolerance3D, double& UTolerance)
{
  if (!myMaxDerivInvOk)
  {
    if (myPeriodic)
    {
      int NbKnots, NbPoles;
      BSplCLib::PrepareUnperiodize(myDeg, myMults, NbKnots, NbPoles);
      NCollection_Array1<gp_Pnt> new_poles(1, NbPoles);
      NCollection_Array1<double> new_weights(1, NbPoles);
      for (int ii = 1; ii <= NbPoles; ii++)
      {
        new_poles(ii) = myPoles((ii - 1) % myPoles.Length() + 1);
      }
      if (myRational)
      {
        for (int ii = 1; ii <= NbPoles; ii++)
        {
          new_weights(ii) = myWeights((ii - 1) % myPoles.Length() + 1);
        }
      }
      BSplCLib::Resolution(new_poles,
                           myRational ? &new_weights : BSplCLib::NoWeights(),
                           new_poles.Length(),
                           myFlatKnots,
                           myDeg,
                           1.,
                           myMaxDerivInv);
    }
    else
    {
      BSplCLib::Resolution(myPoles,
                           Weights(),
                           myPoles.Length(),
                           myFlatKnots,
                           myDeg,
                           1.,
                           myMaxDerivInv);
    }
    myMaxDerivInvOk = true;
  }
  UTolerance = Tolerance3D * myMaxDerivInv;
}

//=================================================================================================

bool Geom_BSplineCurve::IsEqual(const occ::handle<Geom_BSplineCurve>& theOther,
                                const double                          thePreci) const
{
  if (myKnots.IsEmpty() || myPoles.IsEmpty() || myMults.IsEmpty() || myDeg != theOther->Degree()
      || myRational != theOther->IsRational() || myKnots.Length() != theOther->NbKnots()
      || myPoles.Length() != theOther->NbPoles())
  {
    return false;
  }

  for (int aPoleIter = 1; aPoleIter <= myPoles.Length(); ++aPoleIter)
  {
    const gp_Pnt& aPole1 = myPoles(aPoleIter);
    const gp_Pnt& aPole2 = theOther->Pole(aPoleIter);
    if (fabs(aPole1.X() - aPole2.X()) > thePreci || fabs(aPole1.Y() - aPole2.Y()) > thePreci
        || fabs(aPole1.Z() - aPole2.Z()) > thePreci)
    {
      return false;
    }
  }

  for (int aKnotIter = 1; aKnotIter <= myKnots.Length(); ++aKnotIter)
  {
    if (fabs(myKnots(aKnotIter) - theOther->Knot(aKnotIter)) > Precision::Parametric(thePreci))
    {
      return false;
    }
  }

  for (int aMultIter = 1; aMultIter <= myMults.Length(); ++aMultIter)
  {
    if (myMults(aMultIter) != theOther->Multiplicity(aMultIter))
    {
      return false;
    }
  }

  if (!myRational)
    return true;

  for (int aWeightIter = 1; aWeightIter <= myWeights.Length(); ++aWeightIter)
  {
    if (fabs(double(myWeights(aWeightIter) - theOther->Weight(aWeightIter)))
        > Epsilon(myWeights(aWeightIter)))
    {
      return false;
    }
  }
  return true;
}
