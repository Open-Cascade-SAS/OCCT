// Created on: 1993-03-09
// Created by: JCV
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

// Avril 1991 : constructeurs + methodes de lecture.
// Mai 1991   : revue des specifs + debut de realisation des classes tool =>
//              implementation des methodes Set et calcul du point courant.
// Juillet 1991 : voir egalement File Geom_BSplineCurve_1.cxx
// Juin    1992 : mise a plat des valeurs nodales - amelioration des
//                performances sur calcul du point courant

// RLE Aug 1993  Remove Swaps, Remove typedefs, Update BSplCLib
//               debug periodic, IncreaseDegree
//  21-Mar-95 : xab implemented cache
//  14-Mar-96 : xab implemented MovePointAndTangent
//  13-Oct-96 : pmn Bug dans SetPeriodic (PRO6088) et Segment (PRO6250)

#define No_Standard_OutOfRange

#include <BSplCLib.hxx>
#include <ElCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_BSplineCurve, Geom_BoundedCurve)

//=================================================================================================

static void CheckCurveData(const NCollection_Array1<gp_Pnt>& CPoles,
                           const NCollection_Array1<double>& CKnots,
                           const NCollection_Array1<int>&    CMults,
                           const int                         Degree,
                           const bool                        Periodic)
{
  if (Degree < 1 || Degree > Geom_BSplineCurve::MaxDegree())
  {
    throw Standard_ConstructionError("BSpline curve: invalid degree");
  }

  if (CPoles.Length() < 2)
    throw Standard_ConstructionError("BSpline curve: at least 2 poles required");
  if (CKnots.Length() != CMults.Length())
    throw Standard_ConstructionError("BSpline curve: Knot and Mult array size mismatch");

  for (int I = CKnots.Lower(); I < CKnots.Upper(); I++)
  {
    if (CKnots(I + 1) - CKnots(I) <= Epsilon(std::abs(CKnots(I))))
    {
      throw Standard_ConstructionError("BSpline curve: Knots interval values too close");
    }
  }

  if (CPoles.Length() != BSplCLib::NbPoles(Degree, Periodic, CMults))
    throw Standard_ConstructionError("BSpline curve: # Poles and degree mismatch");
}

//! Check rationality of an array of weights
static bool Rational(const NCollection_Array1<double>& theWeights)
{
  for (int i = theWeights.Lower(); i < theWeights.Upper(); i++)
  {
    if (std::abs(theWeights[i] - theWeights[i + 1]) > gp::Resolution())
    {
      return true;
    }
  }
  return false;
}

//=================================================================================================

occ::handle<Geom_Geometry> Geom_BSplineCurve::Copy() const
{
  return new Geom_BSplineCurve(*this);
}

//=================================================================================================

Geom_BSplineCurve::Geom_BSplineCurve(const Geom_BSplineCurve& theOther)
    : rational(theOther.rational),
      periodic(theOther.periodic),
      knotSet(theOther.knotSet),
      smooth(theOther.smooth),
      deg(theOther.deg),
      maxderivinv(theOther.maxderivinv),
      maxderivinvok(false)
{
  // Deep copy all data arrays without validation
  poles = new NCollection_HArray1<gp_Pnt>(theOther.poles->Lower(), theOther.poles->Upper());
  poles->ChangeArray1() = theOther.poles->Array1();

  knots = new NCollection_HArray1<double>(theOther.knots->Lower(), theOther.knots->Upper());
  knots->ChangeArray1() = theOther.knots->Array1();

  mults = new NCollection_HArray1<int>(theOther.mults->Lower(), theOther.mults->Upper());
  mults->ChangeArray1() = theOther.mults->Array1();

  if (!theOther.flatknots.IsNull())
  {
    flatknots =
      new NCollection_HArray1<double>(theOther.flatknots->Lower(), theOther.flatknots->Upper());
    flatknots->ChangeArray1() = theOther.flatknots->Array1();
  }

  if (!theOther.weights.IsNull())
  {
    weights = new NCollection_HArray1<double>(theOther.weights->Lower(), theOther.weights->Upper());
    weights->ChangeArray1() = theOther.weights->Array1();
  }
}

//=================================================================================================

Geom_BSplineCurve::Geom_BSplineCurve(const NCollection_Array1<gp_Pnt>& Poles,
                                     const NCollection_Array1<double>& Knots,
                                     const NCollection_Array1<int>&    Mults,
                                     const int                         Degree,
                                     const bool                        Periodic)
    : rational(false),
      periodic(Periodic),
      deg(Degree),
      maxderivinvok(false)
{
  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  // copy arrays

  poles                 = new NCollection_HArray1<gp_Pnt>(1, Poles.Length());
  poles->ChangeArray1() = Poles;

  knots                 = new NCollection_HArray1<double>(1, Knots.Length());
  knots->ChangeArray1() = Knots;

  mults                 = new NCollection_HArray1<int>(1, Mults.Length());
  mults->ChangeArray1() = Mults;

  UpdateKnots();
}

//=================================================================================================

Geom_BSplineCurve::Geom_BSplineCurve(const NCollection_Array1<gp_Pnt>& Poles,
                                     const NCollection_Array1<double>& Weights,
                                     const NCollection_Array1<double>& Knots,
                                     const NCollection_Array1<int>&    Mults,
                                     const int                         Degree,
                                     const bool                        Periodic,
                                     const bool                        CheckRational)
    : rational(true),
      periodic(Periodic),
      deg(Degree),
      maxderivinvok(false)

{

  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  if (Weights.Length() != Poles.Length())
    throw Standard_ConstructionError("Geom_BSplineCurve: Weights and Poles array size mismatch");

  int i;
  for (i = Weights.Lower(); i <= Weights.Upper(); i++)
  {
    if (Weights(i) <= gp::Resolution())
      throw Standard_ConstructionError("Geom_BSplineCurve: Weights values too small");
  }

  // check really rational
  if (CheckRational)
    rational = Rational(Weights);

  // copy arrays

  poles                 = new NCollection_HArray1<gp_Pnt>(1, Poles.Length());
  poles->ChangeArray1() = Poles;
  if (rational)
  {
    weights                 = new NCollection_HArray1<double>(1, Weights.Length());
    weights->ChangeArray1() = Weights;
  }

  knots                 = new NCollection_HArray1<double>(1, Knots.Length());
  knots->ChangeArray1() = Knots;

  mults                 = new NCollection_HArray1<int>(1, Mults.Length());
  mults->ChangeArray1() = Mults;

  UpdateKnots();
}

//=================================================================================================

int Geom_BSplineCurve::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

void Geom_BSplineCurve::IncreaseDegree(const int Degree)
{
  if (Degree == deg)
    return;

  if (Degree < deg || Degree > Geom_BSplineCurve::MaxDegree())
  {
    throw Standard_ConstructionError("BSpline curve: IncreaseDegree: bad degree value");
  }
  int FromK1 = FirstUKnotIndex();
  int ToK2   = LastUKnotIndex();

  int Step = Degree - deg;

  occ::handle<NCollection_HArray1<gp_Pnt>> npoles =
    new NCollection_HArray1<gp_Pnt>(1, poles->Length() + Step * (ToK2 - FromK1));

  int nbknots = BSplCLib::IncreaseDegreeCountKnots(deg, Degree, periodic, mults->Array1());

  occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, nbknots);

  occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, nbknots);

  occ::handle<NCollection_HArray1<double>> nweights;
  if (IsRational())
  {
    nweights = new NCollection_HArray1<double>(1, npoles->Upper());
  }
  BSplCLib::IncreaseDegree(deg,
                           Degree,
                           periodic,
                           poles->Array1(),
                           !nweights.IsNull() ? &weights->Array1() : BSplCLib::NoWeights(),
                           knots->Array1(),
                           mults->Array1(),
                           npoles->ChangeArray1(),
                           !nweights.IsNull() ? &nweights->ChangeArray1() : BSplCLib::NoWeights(),
                           nknots->ChangeArray1(),
                           nmults->ChangeArray1());
  deg     = Degree;
  poles   = npoles;
  weights = nweights;
  knots   = nknots;
  mults   = nmults;
  UpdateKnots();
}

//=================================================================================================

void Geom_BSplineCurve::IncreaseMultiplicity(const int Index, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = knots->Value(Index);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - mults->Value(Index);
  InsertKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineCurve::IncreaseMultiplicity(const int I1, const int I2, const int M)
{
  occ::handle<NCollection_HArray1<double>> tk = knots;
  NCollection_Array1<double>               k((knots->Array1())(I1), I1, I2);
  NCollection_Array1<int>                  m(I1, I2);
  int                                      i;
  for (i = I1; i <= I2; i++)
    m(i) = M - mults->Value(i);
  InsertKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineCurve::IncrementMultiplicity(const int I1, const int I2, const int Step)
{
  occ::handle<NCollection_HArray1<double>> tk = knots;
  NCollection_Array1<double>               k((knots->Array1())(I1), I1, I2);
  NCollection_Array1<int>                  m(I1, I2);
  m.Init(Step);
  InsertKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom_BSplineCurve::InsertKnot(const double U,
                                   const int    M,
                                   const double ParametricTolerance,
                                   const bool   Add)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = U;
  NCollection_Array1<int> m(1, 1);
  m(1) = M;
  InsertKnots(k, m, ParametricTolerance, Add);
}

//=================================================================================================

void Geom_BSplineCurve::InsertKnots(const NCollection_Array1<double>& Knots,
                                    const NCollection_Array1<int>&    Mults,
                                    const double                      Epsilon,
                                    const bool                        Add)
{
  // Check and compute new sizes
  int nbpoles, nbknots;

  if (!BSplCLib::PrepareInsertKnots(deg,
                                    periodic,
                                    knots->Array1(),
                                    mults->Array1(),
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    Epsilon,
                                    Add))
    throw Standard_ConstructionError("Geom_BSplineCurve::InsertKnots");

  if (nbpoles == poles->Length())
    return;

  occ::handle<NCollection_HArray1<gp_Pnt>> npoles = new NCollection_HArray1<gp_Pnt>(1, nbpoles);
  occ::handle<NCollection_HArray1<double>> nknots = knots;
  occ::handle<NCollection_HArray1<int>>    nmults = mults;

  if (nbknots != knots->Length())
  {
    nknots = new NCollection_HArray1<double>(1, nbknots);
    nmults = new NCollection_HArray1<int>(1, nbknots);
  }

  occ::handle<NCollection_HArray1<double>> nweights;
  if (rational)
  {
    nweights = new NCollection_HArray1<double>(1, nbpoles);
  }

  BSplCLib::InsertKnots(deg,
                        periodic,
                        poles->Array1(),
                        !nweights.IsNull() ? &weights->Array1() : BSplCLib::NoWeights(),
                        knots->Array1(),
                        mults->Array1(),
                        Knots,
                        &Mults,
                        npoles->ChangeArray1(),
                        !nweights.IsNull() ? &nweights->ChangeArray1() : BSplCLib::NoWeights(),
                        nknots->ChangeArray1(),
                        nmults->ChangeArray1(),
                        Epsilon,
                        Add);
  weights = nweights;
  poles   = npoles;
  knots   = nknots;
  mults   = nmults;
  UpdateKnots();
}

//=================================================================================================

bool Geom_BSplineCurve::RemoveKnot(const int Index, const int M, const double Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstUKnotIndex();
  int I2 = LastUKnotIndex();

  if (!periodic && (Index <= I1 || Index >= I2))
  {
    throw Standard_OutOfRange("BSpline curve: RemoveKnot: index out of range");
  }
  else if (periodic && (Index < I1 || Index > I2))
  {
    throw Standard_OutOfRange("BSpline curve: RemoveKnot: index out of range");
  }

  const NCollection_Array1<gp_Pnt>& oldpoles = poles->Array1();

  int step = mults->Value(Index) - M;
  if (step <= 0)
    return true;

  occ::handle<NCollection_HArray1<gp_Pnt>> npoles =
    new NCollection_HArray1<gp_Pnt>(1, oldpoles.Length() - step);

  occ::handle<NCollection_HArray1<double>> nknots = knots;
  occ::handle<NCollection_HArray1<int>>    nmults = mults;

  if (M == 0)
  {
    nknots = new NCollection_HArray1<double>(1, knots->Length() - 1);
    nmults = new NCollection_HArray1<int>(1, knots->Length() - 1);
  }

  occ::handle<NCollection_HArray1<double>> nweights;
  if (IsRational())
  {
    nweights = new NCollection_HArray1<double>(1, npoles->Length());
  }

  if (!BSplCLib::RemoveKnot(Index,
                            M,
                            deg,
                            periodic,
                            poles->Array1(),
                            !nweights.IsNull() ? &weights->Array1() : BSplCLib::NoWeights(),
                            knots->Array1(),
                            mults->Array1(),
                            npoles->ChangeArray1(),
                            !nweights.IsNull() ? &nweights->ChangeArray1() : BSplCLib::NoWeights(),
                            nknots->ChangeArray1(),
                            nmults->ChangeArray1(),
                            Tolerance))
  {
    return false;
  }

  weights = nweights;
  poles   = npoles;
  knots   = nknots;
  mults   = nmults;

  UpdateKnots();
  maxderivinvok = 0;
  return true;
}

//=================================================================================================

void Geom_BSplineCurve::Reverse()
{
  BSplCLib::Reverse(knots->ChangeArray1());
  BSplCLib::Reverse(mults->ChangeArray1());
  int last;
  if (periodic)
    last = flatknots->Upper() - deg - 1;
  else
    last = poles->Upper();
  BSplCLib::Reverse(poles->ChangeArray1(), last);
  if (rational)
    BSplCLib::Reverse(weights->ChangeArray1(), last);
  UpdateKnots();
}

//=================================================================================================

double Geom_BSplineCurve::ReversedParameter(const double U) const
{
  return (FirstParameter() + LastParameter() - U);
}

//=================================================================================================

void Geom_BSplineCurve::Segment(const double U1, const double U2, const double theTolerance)
{
  if (U2 < U1)
    throw Standard_DomainError("Geom_BSplineCurve::Segment");

  double NewU1, NewU2;
  double U, DU = 0, aDDU = 0;
  int    index;
  bool   wasPeriodic = periodic;

  NCollection_Array1<double> Knots(1, 2);
  NCollection_Array1<int>    Mults(1, 2);

  // define param distance to keep (eap, Apr 18 2002, occ311)
  if (periodic)
  {
    double Period = LastParameter() - FirstParameter();
    DU            = U2 - U1;
    if (DU - Period > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineCurve::Segment");
    if (DU > Period)
      DU = Period;
    aDDU = DU;
  }

  index = 0;
  BSplCLib::LocateParameter(deg,
                            knots->Array1(),
                            mults->Array1(),
                            U1,
                            periodic,
                            knots->Lower(),
                            knots->Upper(),
                            index,
                            NewU1);
  index = 0;
  BSplCLib::LocateParameter(deg,
                            knots->Array1(),
                            mults->Array1(),
                            U2,
                            periodic,
                            knots->Lower(),
                            knots->Upper(),
                            index,
                            NewU2);

  //-- DBB
  double aNu2 = NewU2;
  //-- DBB

  Knots(1) = std::min(NewU1, NewU2);
  Knots(2) = std::max(NewU1, NewU2);
  Mults(1) = Mults(2) = deg;

  double AbsUMax = std::max(std::abs(NewU1), std::abs(NewU2));

  //  Modified by Sergey KHROMOV - Fri Apr 11 12:15:40 2003 Begin
  AbsUMax = std::max(AbsUMax, std::max(std::abs(FirstParameter()), std::abs(LastParameter())));
  //  Modified by Sergey KHROMOV - Fri Apr 11 12:15:40 2003 End

  double Eps = std::max(Epsilon(AbsUMax), theTolerance);

  InsertKnots(Knots, Mults, Eps);

  if (periodic)
  { // set the origine at NewU1
    index = 0;
    BSplCLib::LocateParameter(deg,
                              knots->Array1(),
                              mults->Array1(),
                              U1,
                              periodic,
                              knots->Lower(),
                              knots->Upper(),
                              index,
                              U);
    // Test si l'insertion est Ok et decalage sinon.
    if (std::abs(knots->Value(index + 1) - U) <= Eps) // <= pour etre homogene a InsertKnots
      index++;
    SetOrigin(index);
    SetNotPeriodic();
    NewU2 = NewU1 + DU;
  }

  // compute index1 and index2 to set the new knots and mults
  int index1 = 0, index2 = 0;
  int FromU1 = knots->Lower();
  int ToU2   = knots->Upper();
  BSplCLib::LocateParameter(deg,
                            knots->Array1(),
                            mults->Array1(),
                            NewU1,
                            periodic,
                            FromU1,
                            ToU2,
                            index1,
                            U);
  if (std::abs(knots->Value(index1 + 1) - U) <= Eps)
    index1++;

  BSplCLib::LocateParameter(deg,
                            knots->Array1(),
                            mults->Array1(),
                            NewU2,
                            periodic,
                            FromU1,
                            ToU2,
                            index2,
                            U);
  if (std::abs(knots->Value(index2 + 1) - U) <= Eps || index2 == index1)
    index2++;

  int nbknots = index2 - index1 + 1;

  occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, nbknots);
  occ::handle<NCollection_HArray1<int>>    nmults = new NCollection_HArray1<int>(1, nbknots);

  // to restore changed U1
  if (DU > 0) // if was periodic
    DU = NewU1 - U1;

  int i, k = 1;
  for (i = index1; i <= index2; i++)
  {
    nknots->SetValue(k, knots->Value(i) - DU);
    nmults->SetValue(k, mults->Value(i));
    k++;
  }
  nmults->SetValue(1, deg + 1);
  nmults->SetValue(nbknots, deg + 1);

  // compute index1 and index2 to set the new poles and weights
  int pindex1 = BSplCLib::PoleIndex(deg, index1, periodic, mults->Array1());
  int pindex2 = BSplCLib::PoleIndex(deg, index2, periodic, mults->Array1());

  pindex1++;
  pindex2 = std::min(pindex2 + 1, poles->Length());

  int nbpoles = pindex2 - pindex1 + 1;

  occ::handle<NCollection_HArray1<double>> nweights = new NCollection_HArray1<double>(1, nbpoles);
  occ::handle<NCollection_HArray1<gp_Pnt>> npoles   = new NCollection_HArray1<gp_Pnt>(1, nbpoles);

  k = 1;
  if (rational)
  {
    nweights = new NCollection_HArray1<double>(1, nbpoles);
    for (i = pindex1; i <= pindex2; i++)
    {
      npoles->SetValue(k, poles->Value(i));
      nweights->SetValue(k, weights->Value(i));
      k++;
    }
  }
  else
  {
    for (i = pindex1; i <= pindex2; i++)
    {
      npoles->SetValue(k, poles->Value(i));
      k++;
    }
  }

  //-- DBB
  if (wasPeriodic)
  {
    nknots->ChangeValue(nknots->Lower()) = U1;
    if (aNu2 < U2)
    {
      nknots->ChangeValue(nknots->Upper()) = U1 + aDDU;
    }
  }
  //-- DBB

  knots = nknots;
  mults = nmults;
  poles = npoles;
  if (rational)
    weights = nweights;

  maxderivinvok = 0;
  UpdateKnots();
}

//=================================================================================================

void Geom_BSplineCurve::SetKnot(const int Index, const double K)
{
  if (Index < 1 || Index > knots->Length())
    throw Standard_OutOfRange("BSpline curve: SetKnot: Index and #knots mismatch");
  double DK = std::abs(Epsilon(K));
  if (Index == 1)
  {
    if (K >= knots->Value(2) - DK)
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
  }
  else if (Index == knots->Length())
  {
    if (K <= knots->Value(knots->Length() - 1) + DK)
    {
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
    }
  }
  else
  {
    if (K <= knots->Value(Index - 1) + DK || K >= knots->Value(Index + 1) - DK)
    {
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
    }
  }
  if (K != knots->Value(Index))
  {
    knots->SetValue(Index, K);
    maxderivinvok = 0;
    UpdateKnots();
  }
}

//=================================================================================================

void Geom_BSplineCurve::SetKnots(const NCollection_Array1<double>& K)
{
  CheckCurveData(poles->Array1(), K, mults->Array1(), deg, periodic);
  knots->ChangeArray1() = K;
  maxderivinvok         = 0;
  UpdateKnots();
}

//=================================================================================================

void Geom_BSplineCurve::SetKnot(const int Index, const double K, const int M)
{
  IncreaseMultiplicity(Index, M);
  SetKnot(Index, K);
}

//=================================================================================================

void Geom_BSplineCurve::SetPeriodic()
{
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  occ::handle<NCollection_HArray1<double>> tk = knots;
  NCollection_Array1<double>               cknots((knots->Array1())(first), first, last);
  knots                 = new NCollection_HArray1<double>(1, cknots.Length());
  knots->ChangeArray1() = cknots;

  occ::handle<NCollection_HArray1<int>> tm = mults;
  NCollection_Array1<int>               cmults((mults->Array1())(first), first, last);
  cmults(first) = cmults(last) = std::min(deg, std::max(cmults(first), cmults(last)));
  mults                        = new NCollection_HArray1<int>(1, cmults.Length());
  mults->ChangeArray1()        = cmults;

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(deg, true, cmults);

  occ::handle<NCollection_HArray1<gp_Pnt>> tp = poles;
  NCollection_Array1<gp_Pnt>               cpoles((poles->Array1())(1), 1, nbp);
  poles                 = new NCollection_HArray1<gp_Pnt>(1, nbp);
  poles->ChangeArray1() = cpoles;

  if (rational)
  {
    occ::handle<NCollection_HArray1<double>> tw = weights;
    NCollection_Array1<double>               cweights((weights->Array1())(1), 1, nbp);
    weights                 = new NCollection_HArray1<double>(1, nbp);
    weights->ChangeArray1() = cweights;
  }

  periodic = true;

  maxderivinvok = 0;
  UpdateKnots();
}

//=================================================================================================

void Geom_BSplineCurve::SetOrigin(const int Index)
{
  if (!periodic)
    throw Standard_NoSuchObject("Geom_BSplineCurve::SetOrigin");

  int i, k;
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom_BSplineCurve::SetOrigin");

  int nbknots = knots->Length();
  int nbpoles = poles->Length();

  occ::handle<NCollection_HArray1<double>> nknots   = new NCollection_HArray1<double>(1, nbknots);
  NCollection_Array1<double>&              newknots = nknots->ChangeArray1();

  occ::handle<NCollection_HArray1<int>> nmults   = new NCollection_HArray1<int>(1, nbknots);
  NCollection_Array1<int>&              newmults = nmults->ChangeArray1();

  // set the knots and mults
  double period = knots->Value(last) - knots->Value(first);
  k             = 1;
  for (i = Index; i <= last; i++)
  {
    newknots(k) = knots->Value(i);
    newmults(k) = mults->Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    newknots(k) = knots->Value(i) + period;
    newmults(k) = mults->Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += mults->Value(i);

  // set the poles and weights
  occ::handle<NCollection_HArray1<gp_Pnt>> npoles     = new NCollection_HArray1<gp_Pnt>(1, nbpoles);
  occ::handle<NCollection_HArray1<double>> nweights   = new NCollection_HArray1<double>(1, nbpoles);
  NCollection_Array1<gp_Pnt>&              newpoles   = npoles->ChangeArray1();
  NCollection_Array1<double>&              newweights = nweights->ChangeArray1();
  first                                               = poles->Lower();
  last                                                = poles->Upper();
  if (rational)
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      newpoles(k)   = poles->Value(i);
      newweights(k) = weights->Value(i);
      k++;
    }
    for (i = first; i < index; i++)
    {
      newpoles(k)   = poles->Value(i);
      newweights(k) = weights->Value(i);
      k++;
    }
  }
  else
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      newpoles(k) = poles->Value(i);
      k++;
    }
    for (i = first; i < index; i++)
    {
      newpoles(k) = poles->Value(i);
      k++;
    }
  }

  poles = npoles;
  knots = nknots;
  mults = nmults;
  if (rational)
    weights = nweights;
  maxderivinvok = 0;
  UpdateKnots();
}

//=================================================================================================

void Geom_BSplineCurve::SetOrigin(const double U, const double Tol)
{
  if (!periodic)
    throw Standard_NoSuchObject("Geom_BSplineCurve::SetOrigin");
  // U est il dans la period.
  double uf = FirstParameter(), ul = LastParameter();
  double u = U, period = ul - uf;
  while (Tol < (uf - u))
    u += period;
  while (Tol > (ul - u))
    u -= period;

  if (std::abs(U - u) > Tol)
  { // On reparametre la courbe
    double delta = U - u;
    uf += delta;
    ul += delta;
    NCollection_Array1<double>& kn = knots->ChangeArray1();
    int                         fk = kn.Lower(), lk = kn.Upper();
    for (int i = fk; i <= lk; i++)
    {
      kn.ChangeValue(i) += delta;
    }
    UpdateKnots();
  }
  // For periodic curve, uf and ul represent the same point
  if (std::abs(U - uf) < Tol || std::abs(U - ul) < Tol)
    return;

  NCollection_Array1<double>& kn = knots->ChangeArray1();
  int                         fk = kn.Lower(), lk = kn.Upper(), ik = 0;
  double                      delta = RealLast();
  for (int i = fk; i <= lk; i++)
  {
    double dki = kn.Value(i) - U;
    if (std::abs(dki) < std::abs(delta))
    {
      ik    = i;
      delta = dki;
    }
  }
  if (std::abs(delta) > Tol)
  {
    InsertKnot(U);
    if (delta < 0.)
      ik++;
  }
  SetOrigin(ik);
}

//=================================================================================================

void Geom_BSplineCurve::SetNotPeriodic()
{
  if (periodic)
  {
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(deg, mults->Array1(), NbKnots, NbPoles);

    occ::handle<NCollection_HArray1<gp_Pnt>> npoles = new NCollection_HArray1<gp_Pnt>(1, NbPoles);

    occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, NbKnots);

    occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, NbKnots);

    occ::handle<NCollection_HArray1<double>> nweights;
    if (IsRational())
    {
      nweights = new NCollection_HArray1<double>(1, NbPoles);
    }

    BSplCLib::Unperiodize(deg,
                          mults->Array1(),
                          knots->Array1(),
                          poles->Array1(),
                          !nweights.IsNull() ? &weights->Array1() : BSplCLib::NoWeights(),
                          nmults->ChangeArray1(),
                          nknots->ChangeArray1(),
                          npoles->ChangeArray1(),
                          !nweights.IsNull() ? &nweights->ChangeArray1() : BSplCLib::NoWeights());
    poles    = npoles;
    weights  = nweights;
    mults    = nmults;
    knots    = nknots;
    periodic = false;

    maxderivinvok = 0;
    UpdateKnots();
  }
}

//=================================================================================================

void Geom_BSplineCurve::SetPole(const int Index, const gp_Pnt& P)
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange("BSpline curve: SetPole: index and #pole mismatch");
  poles->SetValue(Index, P);
  maxderivinvok = 0;
}

//=================================================================================================

void Geom_BSplineCurve::SetPole(const int Index, const gp_Pnt& P, const double W)
{
  SetPole(Index, P);
  SetWeight(Index, W);
}

//=================================================================================================

void Geom_BSplineCurve::SetWeight(const int Index, const double W)
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange("BSpline curve: SetWeight: Index and #pole mismatch");

  if (W <= gp::Resolution())
    throw Standard_ConstructionError("BSpline curve: SetWeight: Weight too small");

  bool rat = IsRational() || (std::abs(W - 1.) > gp::Resolution());

  if (rat)
  {
    if (rat && !IsRational())
    {
      weights = new NCollection_HArray1<double>(1, poles->Length());
      weights->Init(1.);
    }

    weights->SetValue(Index, W);

    if (IsRational())
    {
      rat = Rational(weights->Array1());
      if (!rat)
        weights.Nullify();
    }

    rational = !weights.IsNull();
  }
  maxderivinvok = 0;
}

//=================================================================================================

void Geom_BSplineCurve::MovePoint(const double  U,
                                  const gp_Pnt& P,
                                  const int     Index1,
                                  const int     Index2,
                                  int&          FirstModifiedPole,
                                  int&          LastmodifiedPole)
{
  if (Index1 < 1 || Index1 > poles->Length() || Index2 < 1 || Index2 > poles->Length()
      || Index1 > Index2)
  {
    throw Standard_OutOfRange("BSpline curve: MovePoint: Index and #pole mismatch");
  }
  NCollection_Array1<gp_Pnt> npoles(1, poles->Length());
  gp_Pnt                     P0;
  D0(U, P0);
  gp_Vec Displ(P0, P);
  BSplCLib::MovePoint(U,
                      Displ,
                      Index1,
                      Index2,
                      deg,
                      poles->Array1(),
                      rational ? &weights->Array1() : BSplCLib::NoWeights(),
                      flatknots->Array1(),
                      FirstModifiedPole,
                      LastmodifiedPole,
                      npoles);
  if (FirstModifiedPole)
  {
    poles->ChangeArray1() = npoles;
    maxderivinvok         = 0;
  }
}

//=================================================================================================

void Geom_BSplineCurve::MovePointAndTangent(const double  U,
                                            const gp_Pnt& P,
                                            const gp_Vec& Tangent,
                                            const double  Tolerance,
                                            const int     StartingCondition,
                                            const int     EndingCondition,
                                            int&          ErrorStatus)
{
  int ii;
  if (IsPeriodic())
  {
    //
    // for the time being do not deal with periodic curves
    //
    SetNotPeriodic();
  }
  NCollection_Array1<gp_Pnt> new_poles(1, poles->Length());
  gp_Pnt                     P0;

  gp_Vec delta_derivative;
  D1(U, P0, delta_derivative);
  gp_Vec delta(P0, P);
  for (ii = 1; ii <= 3; ii++)
  {
    delta_derivative.SetCoord(ii, Tangent.Coord(ii) - delta_derivative.Coord(ii));
  }
  BSplCLib::MovePointAndTangent(U,
                                delta,
                                delta_derivative,
                                Tolerance,
                                deg,
                                StartingCondition,
                                EndingCondition,
                                poles->Array1(),
                                rational ? &weights->Array1() : BSplCLib::NoWeights(),
                                flatknots->Array1(),
                                new_poles,
                                ErrorStatus);
  if (!ErrorStatus)
  {
    poles->ChangeArray1() = new_poles;
    maxderivinvok         = 0;
  }
}

//=================================================================================================

void Geom_BSplineCurve::UpdateKnots()
{
  rational = !weights.IsNull();

  int MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(deg, periodic, knots->Array1(), mults->Array1(), knotSet, MaxKnotMult);

  if (knotSet == GeomAbs_Uniform && !periodic)
  {
    flatknots = knots;
  }
  else
  {
    flatknots =
      new NCollection_HArray1<double>(1,
                                      BSplCLib::KnotSequenceLength(mults->Array1(), deg, periodic));

    BSplCLib::KnotSequence(knots->Array1(),
                           mults->Array1(),
                           deg,
                           periodic,
                           flatknots->ChangeArray1());
  }

  if (MaxKnotMult == 0)
    smooth = GeomAbs_CN;
  else
  {
    switch (deg - MaxKnotMult)
    {
      case 0:
        smooth = GeomAbs_C0;
        break;
      case 1:
        smooth = GeomAbs_C1;
        break;
      case 2:
        smooth = GeomAbs_C2;
        break;
      case 3:
        smooth = GeomAbs_C3;
        break;
      default:
        smooth = GeomAbs_C3;
        break;
    }
  }
}

//=======================================================================
// function : Normalizes the parameters if the curve is periodic
// purpose  : that is compute the cache so that it is valid
//=======================================================================

void Geom_BSplineCurve::PeriodicNormalization(double& Parameter) const
{
  double Period;

  if (periodic)
  {
    Period = flatknots->Value(flatknots->Upper() - deg) - flatknots->Value(deg + 1);
    while (Parameter > flatknots->Value(flatknots->Upper() - deg))
    {
      Parameter -= Period;
    }
    while (Parameter < flatknots->Value((deg + 1)))
    {
      Parameter += Period;
    }
  }
}

//=================================================================================================

void Geom_BSplineCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, rational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, periodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, knotSet)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, smooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, deg)
  if (!poles.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, poles->Size())

  if (!weights.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, weights->Size())
  if (!flatknots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, flatknots->Size())
  if (!knots.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, knots->Size())
  if (!mults.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mults->Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
