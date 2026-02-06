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

#define POLES (myData.Poles)
#define KNOTS (myData.Knots)
#define FKNOTS (myData.FlatKnots)
#define FMULTS (BSplCLib::NoMults())

//=================================================================================================

bool Geom_BSplineCurve::IsCN(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, "Geom_BSplineCurve::IsCN");

  switch (smooth)
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
      return N <= 3 ? true
                    : N <= myData.Degree
                             - BSplCLib::MaxKnotMult(myData.Mults,
                                                     myData.Mults.Lower() + 1,
                                                     myData.Mults.Upper() - 1);
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
  D1(Knot(FirstUKnotIndex()), aP, aV1);
  D1(Knot(LastUKnotIndex()), aP, aV2);

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
  return myData.IsPeriodic;
}

//=================================================================================================

GeomAbs_Shape Geom_BSplineCurve::Continuity() const
{
  return smooth;
}

//=================================================================================================

int Geom_BSplineCurve::Degree() const
{
  return myData.Degree;
}

//=================================================================================================

void Geom_BSplineCurve::D0(const double U, gp_Pnt& P) const
{
  int    aSpanIndex = 0;
  double aNewU(U);
  PeriodicNormalization(aNewU);
  BSplCLib::LocateParameter(myData.Degree, myData.Knots, &myData.Mults, U, myData.IsPeriodic, aSpanIndex, aNewU);
  if (aNewU < myData.Knots(aSpanIndex))
    aSpanIndex--;

  BSplCLib::D0(aNewU,
               aSpanIndex,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               myData.Knots,
               &myData.Mults,
               P);
}

//=================================================================================================

void Geom_BSplineCurve::D1(const double U, gp_Pnt& P, gp_Vec& V1) const
{
  int    aSpanIndex = 0;
  double aNewU(U);
  PeriodicNormalization(aNewU);
  BSplCLib::LocateParameter(myData.Degree, myData.Knots, &myData.Mults, U, myData.IsPeriodic, aSpanIndex, aNewU);
  if (aNewU < myData.Knots(aSpanIndex))
    aSpanIndex--;

  BSplCLib::D1(aNewU,
               aSpanIndex,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               myData.Knots,
               &myData.Mults,
               P,
               V1);
}

//=================================================================================================

void Geom_BSplineCurve::D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{
  int    aSpanIndex = 0;
  double aNewU(U);
  PeriodicNormalization(aNewU);
  BSplCLib::LocateParameter(myData.Degree, myData.Knots, &myData.Mults, U, myData.IsPeriodic, aSpanIndex, aNewU);
  if (aNewU < myData.Knots(aSpanIndex))
    aSpanIndex--;

  BSplCLib::D2(aNewU,
               aSpanIndex,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               myData.Knots,
               &myData.Mults,
               P,
               V1,
               V2);
}

//=================================================================================================

void Geom_BSplineCurve::D3(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const
{
  int    aSpanIndex = 0;
  double aNewU(U);
  PeriodicNormalization(aNewU);
  BSplCLib::LocateParameter(myData.Degree, myData.Knots, &myData.Mults, U, myData.IsPeriodic, aSpanIndex, aNewU);
  if (aNewU < myData.Knots(aSpanIndex))
    aSpanIndex--;

  BSplCLib::D3(aNewU,
               aSpanIndex,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               myData.Knots,
               &myData.Mults,
               P,
               V1,
               V2,
               V3);
}

//=================================================================================================

gp_Vec Geom_BSplineCurve::DN(const double U, const int N) const
{
  gp_Vec V;
  BSplCLib::DN(U,
               N,
               0,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               FKNOTS,
               FMULTS,
               V);
  return V;
}

//=================================================================================================

gp_Pnt Geom_BSplineCurve::EndPoint() const
{
  if (myData.Mults(myData.Knots.Upper()) == myData.Degree + 1)
    return myData.Poles(myData.Poles.Upper());
  else
    return Value(LastParameter());
}

//=================================================================================================

int Geom_BSplineCurve::FirstUKnotIndex() const
{
  if (myData.IsPeriodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(myData.Degree, myData.Mults);
}

//=================================================================================================

double Geom_BSplineCurve::FirstParameter() const
{
  return myData.FlatKnots(myData.Degree + 1);
}

//=================================================================================================

double Geom_BSplineCurve::Knot(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myData.Knots.Length(), "Geom_BSplineCurve::Knot");
  return myData.Knots(Index);
}

//=================================================================================================

GeomAbs_BSplKnotDistribution Geom_BSplineCurve::KnotDistribution() const
{
  return knotSet;
}

//=================================================================================================

void Geom_BSplineCurve::Knots(NCollection_Array1<double>& K) const
{
  Standard_DomainError_Raise_if(K.Lower() < myData.Knots.Lower() || K.Upper() > myData.Knots.Upper(),
                                "Geom_BSplineCurve::Knots");
  for (int anIdx = K.Lower(); anIdx <= K.Upper(); anIdx++)
    K(anIdx) = myData.Knots(anIdx);
}

const NCollection_Array1<double>& Geom_BSplineCurve::Knots() const
{
  return myData.Knots;
}

//=================================================================================================

void Geom_BSplineCurve::KnotSequence(NCollection_Array1<double>& K) const
{
  Standard_DomainError_Raise_if(K.Lower() < myData.FlatKnots.Lower() || K.Upper() > myData.FlatKnots.Upper(),
                                "Geom_BSplineCurve::KnotSequence");
  for (int anIdx = K.Lower(); anIdx <= K.Upper(); anIdx++)
    K(anIdx) = myData.FlatKnots(anIdx);
}

const NCollection_Array1<double>& Geom_BSplineCurve::KnotSequence() const
{
  return myData.FlatKnots;
}

//=================================================================================================

int Geom_BSplineCurve::LastUKnotIndex() const
{
  if (myData.IsPeriodic)
    return myData.Knots.Length();
  else
    return BSplCLib::LastUKnotIndex(myData.Degree, myData.Mults);
}

//=================================================================================================

double Geom_BSplineCurve::LastParameter() const
{
  return myData.FlatKnots(myData.FlatKnots.Upper() - myData.Degree);
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
  BSplCLib::LocateParameter(myData.Degree, FKNOTS, U, myData.IsPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myData.Degree, index, myData.Mults, myData.IsPeriodic);
  BSplCLib::D0(u,
               index,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               FKNOTS,
               FMULTS,
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
  BSplCLib::LocateParameter(myData.Degree, FKNOTS, U, myData.IsPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myData.Degree, index, myData.Mults, myData.IsPeriodic);
  BSplCLib::D1(u,
               index,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               FKNOTS,
               FMULTS,
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
  BSplCLib::LocateParameter(myData.Degree, FKNOTS, U, myData.IsPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myData.Degree, index, myData.Mults, myData.IsPeriodic);
  BSplCLib::D2(u,
               index,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               FKNOTS,
               FMULTS,
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
  BSplCLib::LocateParameter(myData.Degree, FKNOTS, U, myData.IsPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myData.Degree, index, myData.Mults, myData.IsPeriodic);
  BSplCLib::D3(u,
               index,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               FKNOTS,
               FMULTS,
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
  BSplCLib::LocateParameter(myData.Degree, FKNOTS, U, myData.IsPeriodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(myData.Degree, index, myData.Mults, myData.IsPeriodic);

  gp_Vec V;
  BSplCLib::DN(u,
               N,
               index,
               myData.Degree,
               myData.IsPeriodic,
               POLES,
               rational ? &myData.Weights : BSplCLib::NoWeights(),
               FKNOTS,
               FMULTS,
               V);
  return V;
}

//=================================================================================================

int Geom_BSplineCurve::Multiplicity(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myData.Mults.Length(),
                               "Geom_BSplineCurve::Multiplicity");
  return myData.Mults(Index);
}

//=================================================================================================

void Geom_BSplineCurve::Multiplicities(NCollection_Array1<int>& M) const
{
  Standard_DimensionError_Raise_if(M.Length() != myData.Mults.Length(),
                                   "Geom_BSplineCurve::Multiplicities");
  M = myData.Mults;
}

const NCollection_Array1<int>& Geom_BSplineCurve::Multiplicities() const
{
  return myData.Mults;
}

//=================================================================================================

int Geom_BSplineCurve::NbKnots() const
{
  return myData.Knots.Length();
}

//=================================================================================================

int Geom_BSplineCurve::NbPoles() const
{
  return myData.Poles.Length();
}

//=================================================================================================

const gp_Pnt& Geom_BSplineCurve::Pole(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myData.Poles.Length(), "Geom_BSplineCurve::Pole");
  return myData.Poles(Index);
}

//=================================================================================================

void Geom_BSplineCurve::Poles(NCollection_Array1<gp_Pnt>& P) const
{
  Standard_DimensionError_Raise_if(P.Length() != myData.Poles.Length(), "Geom_BSplineCurve::Poles");
  P = myData.Poles;
}

const NCollection_Array1<gp_Pnt>& Geom_BSplineCurve::Poles() const
{
  return myData.Poles;
}

//=================================================================================================

gp_Pnt Geom_BSplineCurve::StartPoint() const
{
  if (myData.Mults(1) == myData.Degree + 1)
    return myData.Poles(1);
  else
    return Value(FirstParameter());
}

//=================================================================================================

double Geom_BSplineCurve::Weight(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myData.Poles.Length(), "Geom_BSplineCurve::Weight");
  if (IsRational())
    return myData.Weights(Index);
  else
    return 1.;
}

//=================================================================================================

void Geom_BSplineCurve::Weights(NCollection_Array1<double>& W) const
{
  Standard_DimensionError_Raise_if(W.Length() != myData.Poles.Length(), "Geom_BSplineCurve::Weights");
  if (IsRational())
    W = myData.Weights;
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
    return &myData.Weights;
  return BSplCLib::NoWeights();
}

//=================================================================================================

bool Geom_BSplineCurve::IsRational() const
{
  return rational;
}

//=================================================================================================

const NCollection_Array1<double>* Geom_BSplineCurve::InternalWeights() const
{
  if (rational)
    return &myData.Weights;
  return BSplCLib::NoWeights();
}

//=================================================================================================

void Geom_BSplineCurve::Transform(const gp_Trsf& T)
{
  for (int I = 1; I <= myData.Poles.Length(); I++)
    myData.Poles.ChangeValue(I).Transform(T);
  maxderivinvok = false;
}

//=================================================================================================

void Geom_BSplineCurve::LocateU(const double U,
                                const double ParametricTolerance,
                                int&         I1,
                                int&         I2,
                                const bool   WithKnotRepetition) const
{
  double                             NewU = U;
  const NCollection_Array1<double>& CKnots = WithKnotRepetition ? myData.FlatKnots : myData.Knots;

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
  if (!maxderivinvok)
  {
    if (myData.IsPeriodic)
    {
      int NbKnots, NbPoles;
      BSplCLib::PrepareUnperiodize(myData.Degree, myData.Mults, NbKnots, NbPoles);
      NCollection_Array1<gp_Pnt> new_poles(1, NbPoles);
      NCollection_Array1<double> new_weights(1, NbPoles);
      for (int ii = 1; ii <= NbPoles; ii++)
      {
        new_poles(ii) = myData.Poles((ii - 1) % myData.Poles.Length() + 1);
      }
      if (rational)
      {
        for (int ii = 1; ii <= NbPoles; ii++)
        {
          new_weights(ii) = myData.Weights((ii - 1) % myData.Poles.Length() + 1);
        }
      }
      BSplCLib::Resolution(new_poles,
                           rational ? &new_weights : BSplCLib::NoWeights(),
                           new_poles.Length(),
                           myData.FlatKnots,
                           myData.Degree,
                           1.,
                           maxderivinv);
    }
    else
    {
      BSplCLib::Resolution(myData.Poles,
                           rational ? &myData.Weights : BSplCLib::NoWeights(),
                           myData.Poles.Length(),
                           myData.FlatKnots,
                           myData.Degree,
                           1.,
                           maxderivinv);
    }
    maxderivinvok = true;
  }
  UTolerance = Tolerance3D * maxderivinv;
}

//=================================================================================================

bool Geom_BSplineCurve::IsEqual(const occ::handle<Geom_BSplineCurve>& theOther,
                                const double                          thePreci) const
{
  if (myData.Knots.IsEmpty() || myData.Poles.IsEmpty() || myData.Mults.IsEmpty())
    return false;
  if (myData.Degree != theOther->Degree())
    return false;
  if (myData.Knots.Length() != theOther->NbKnots() || myData.Poles.Length() != theOther->NbPoles())
    return false;

  int i = 1;
  for (i = 1; i <= myData.Poles.Length(); i++)
  {
    const gp_Pnt& aPole1 = myData.Poles(i);
    const gp_Pnt& aPole2 = theOther->Pole(i);
    if (fabs(aPole1.X() - aPole2.X()) > thePreci || fabs(aPole1.Y() - aPole2.Y()) > thePreci
        || fabs(aPole1.Z() - aPole2.Z()) > thePreci)
      return false;
  }

  for (; i <= myData.Knots.Length(); i++)
  {
    if (fabs(myData.Knots(i) - theOther->Knot(i)) > Precision::Parametric(thePreci))
      return false;
  }

  for (i = 1; i <= myData.Mults.Length(); i++)
  {
    if (myData.Mults(i) != theOther->Multiplicity(i))
      return false;
  }

  if (rational != theOther->IsRational())
    return false;

  if (!rational)
    return true;

  for (i = 1; i <= myData.Weights.Length(); i++)
  {
    if (fabs(double(myData.Weights(i) - theOther->Weight(i))) > Epsilon(myData.Weights(i)))
      return false;
  }
  return true;
}
