// Created on: 1993-03-25
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
// Juillet 1991 : voir egalement File Geom2d_BSplineCurve_1.cxx
// Juin    1992 : mise a plat des valeurs nodales - amelioration des
//                performances sur calcul du point courant

// RLE Aug 1993  Remove Swaps, Init methods, Remove typedefs
//   14-Mar-96 : xab implemented MovePointAndTangent

// SAMTECH Jan 2002 : add text to Raise()

#include <BSplCLib.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Geometry.hxx>
#include <Geom2d_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_BSplineCurve, Geom2d_BoundedCurve)

//=================================================================================================

static void CheckCurveData(const NCollection_Array1<gp_Pnt2d>& CPoles,
                           const NCollection_Array1<double>&   CKnots,
                           const NCollection_Array1<int>&      CMults,
                           const int                           Degree,
                           const bool                          Periodic)
{
  if (Degree < 1 || Degree > Geom2d_BSplineCurve::MaxDegree())
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

occ::handle<Geom2d_Geometry> Geom2d_BSplineCurve::Copy() const
{
  return new Geom2d_BSplineCurve(*this);
}

//=================================================================================================

Geom2d_BSplineCurve::Geom2d_BSplineCurve(const Geom2d_BSplineCurve& theOther)
    : myData(theOther.myData),
      rational(theOther.rational),
      knotSet(theOther.knotSet),
      smooth(theOther.smooth),
      maxderivinv(theOther.maxderivinv),
      maxderivinvok(false)
{
}

//=================================================================================================

Geom2d_BSplineCurve::Geom2d_BSplineCurve(const NCollection_Array1<gp_Pnt2d>& Poles,
                                         const NCollection_Array1<double>&   Knots,
                                         const NCollection_Array1<int>&      Mults,
                                         const int                           Degree,
                                         const bool                          Periodic)
    : rational(false),
      maxderivinvok(false)
{
  myData.Degree     = Degree;
  myData.IsPeriodic = Periodic;

  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  // copy arrays

  myData.Poles.Resize(1, Poles.Length(), false);
  myData.Poles.Assign(Poles);

  myData.Knots.Resize(1, Knots.Length(), false);
  myData.Knots.Assign(Knots);

  myData.Mults.Resize(1, Mults.Length(), false);
  myData.Mults.Assign(Mults);

  UpdateKnots();
}

//=================================================================================================

Geom2d_BSplineCurve::Geom2d_BSplineCurve(const NCollection_Array1<gp_Pnt2d>& Poles,
                                         const NCollection_Array1<double>&   Weights,
                                         const NCollection_Array1<double>&   Knots,
                                         const NCollection_Array1<int>&      Mults,
                                         const int                           Degree,
                                         const bool                          Periodic)
    : rational(true),
      maxderivinvok(false)

{
  myData.Degree     = Degree;
  myData.IsPeriodic = Periodic;

  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  if (Weights.Length() != Poles.Length())
    throw Standard_ConstructionError("Geom2d_BSplineCurve: Weights and Poles array size mismatch");

  int i;
  for (i = Weights.Lower(); i <= Weights.Upper(); i++)
  {
    if (Weights(i) <= gp::Resolution())
    {
      throw Standard_ConstructionError("Geom2d_BSplineCurve: Weights values too small");
    }
  }

  // check really rational
  rational = Rational(Weights);

  // copy arrays

  myData.Poles.Resize(1, Poles.Length(), false);
  myData.Poles.Assign(Poles);
  if (rational)
  {
    myData.Weights.Resize(1, Weights.Length(), false);
    myData.Weights.Assign(Weights);
  }

  myData.Knots.Resize(1, Knots.Length(), false);
  myData.Knots.Assign(Knots);

  myData.Mults.Resize(1, Mults.Length(), false);
  myData.Mults.Assign(Mults);

  UpdateKnots();
}

//=================================================================================================

int Geom2d_BSplineCurve::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

void Geom2d_BSplineCurve::IncreaseDegree(const int Degree)
{
  if (Degree == myData.Degree)
    return;

  if (Degree < myData.Degree || Degree > Geom2d_BSplineCurve::MaxDegree())
  {
    throw Standard_ConstructionError("BSpline curve: IncreaseDegree: bad degree value");
  }

  int FromK1 = FirstUKnotIndex();
  int ToK2   = LastUKnotIndex();

  int Step = Degree - myData.Degree;

  NCollection_Array1<gp_Pnt2d> npoles(1, myData.Poles.Length() + Step * (ToK2 - FromK1));

  int nbknots = BSplCLib::IncreaseDegreeCountKnots(myData.Degree, Degree, myData.IsPeriodic, myData.Mults);

  NCollection_Array1<double> nknots(1, nbknots);

  NCollection_Array1<int> nmults(1, nbknots);

  NCollection_Array1<double> nweights;

  if (IsRational())
  {
    nweights.Resize(1, npoles.Upper(), false);
  }

  BSplCLib::IncreaseDegree(myData.Degree,
                           Degree,
                           myData.IsPeriodic,
                           myData.Poles,
                           rational ? &myData.Weights : BSplCLib::NoWeights(),
                           myData.Knots,
                           myData.Mults,
                           npoles,
                           rational ? &nweights : BSplCLib::NoWeights(),
                           nknots,
                           nmults);

  myData.Degree  = Degree;
  myData.Poles   = std::move(npoles);
  if (rational)
    myData.Weights = std::move(nweights);
  else
    myData.Weights = NCollection_Array1<double>();
  myData.Knots   = std::move(nknots);
  myData.Mults   = std::move(nmults);
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::IncreaseMultiplicity(const int Index, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = myData.Knots.Value(Index);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - myData.Mults.Value(Index);
  InsertKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom2d_BSplineCurve::IncreaseMultiplicity(const int I1, const int I2, const int M)
{
  NCollection_Array1<double> k(myData.Knots(I1), I1, I2);
  NCollection_Array1<int>    m(I1, I2);
  int                        i;
  for (i = I1; i <= I2; i++)
    m(i) = M - myData.Mults.Value(i);
  InsertKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom2d_BSplineCurve::IncrementMultiplicity(const int I1, const int I2, const int Step)
{
  NCollection_Array1<double> k(myData.Knots(I1), I1, I2);
  NCollection_Array1<int>    m(I1, I2);
  m.Init(Step);
  InsertKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertKnot(const double U, const int M, const double ParametricTolerance)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = U;
  NCollection_Array1<int> m(1, 1);
  m(1) = M;
  InsertKnots(k, m, ParametricTolerance);
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertKnots(const NCollection_Array1<double>& Knots,
                                      const NCollection_Array1<int>&    Mults,
                                      const double                      Epsilon,
                                      const bool                        Add)
{
  // Check and compute new sizes
  int nbpoles, nbknots;

  if (!BSplCLib::PrepareInsertKnots(myData.Degree,
                                    myData.IsPeriodic,
                                    myData.Knots,
                                    myData.Mults,
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    Epsilon,
                                    Add))
    throw Standard_ConstructionError("Geom2d_BSplineCurve::InsertKnots");

  if (nbpoles == myData.Poles.Length())
    return;

  NCollection_Array1<gp_Pnt2d> npoles(1, nbpoles);
  NCollection_Array1<double>   nknots;
  NCollection_Array1<int>      nmults;

  if (nbknots != myData.Knots.Length())
  {
    nknots.Resize(1, nbknots, false);
    nmults.Resize(1, nbknots, false);
  }
  else
  {
    nknots = myData.Knots;
    nmults = myData.Mults;
  }

  NCollection_Array1<double> nweights;
  if (rational)
  {
    nweights.Resize(1, nbpoles, false);
  }

  BSplCLib::InsertKnots(myData.Degree,
                        myData.IsPeriodic,
                        myData.Poles,
                        rational ? &myData.Weights : BSplCLib::NoWeights(),
                        myData.Knots,
                        myData.Mults,
                        Knots,
                        &Mults,
                        npoles,
                        rational ? &nweights : BSplCLib::NoWeights(),
                        nknots,
                        nmults,
                        Epsilon,
                        Add);
  if (rational)
    myData.Weights = std::move(nweights);
  else
    myData.Weights = NCollection_Array1<double>();
  myData.Poles = std::move(npoles);
  myData.Knots = std::move(nknots);
  myData.Mults = std::move(nmults);
  UpdateKnots();
}

//=================================================================================================

bool Geom2d_BSplineCurve::RemoveKnot(const int Index, const int M, const double Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstUKnotIndex();
  int I2 = LastUKnotIndex();

  if (Index < I1 || Index > I2)
  {
    throw Standard_OutOfRange("BSpline curve: RemoveKnot: index out of range");
  }

  const NCollection_Array1<gp_Pnt2d>& oldpoles = myData.Poles;

  int step = myData.Mults.Value(Index) - M;
  if (step <= 0)
    return true;

  NCollection_Array1<gp_Pnt2d> npoles(1, oldpoles.Length() - step);

  NCollection_Array1<double> nknots = myData.Knots;
  NCollection_Array1<int>    nmults = myData.Mults;

  if (M == 0)
  {
    nknots.Resize(1, myData.Knots.Length() - 1, false);
    nmults.Resize(1, myData.Knots.Length() - 1, false);
  }

  NCollection_Array1<double> nweights;
  if (IsRational())
  {
    nweights.Resize(1, npoles.Length(), false);
  }

  if (!BSplCLib::RemoveKnot(Index,
                            M,
                            myData.Degree,
                            myData.IsPeriodic,
                            myData.Poles,
                            rational ? &myData.Weights : BSplCLib::NoWeights(),
                            myData.Knots,
                            myData.Mults,
                            npoles,
                            rational ? &nweights : BSplCLib::NoWeights(),
                            nknots,
                            nmults,
                            Tolerance))
  {
    return false;
  }

  if (rational)
    myData.Weights = std::move(nweights);
  else
    myData.Weights = NCollection_Array1<double>();
  myData.Poles = std::move(npoles);
  myData.Knots = std::move(nknots);
  myData.Mults = std::move(nmults);

  UpdateKnots();
  maxderivinvok = false;
  return true;
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertPoleAfter(const int Index, const gp_Pnt2d& P, const double Weight)
{
  if (Index < 0 || Index > myData.Poles.Length())
    throw Standard_OutOfRange("BSpline curve: InsertPoleAfter: Index and #pole mismatch");

  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("BSpline curve: InsertPoleAfter: Weight too small");

  if (knotSet == GeomAbs_NonUniform || knotSet == GeomAbs_PiecewiseBezier)
  {
    throw Standard_ConstructionError("BSpline curve: InsertPoleAfter: bad knotSet type");
  }

  const NCollection_Array1<double>& cknots  = myData.Knots;
  int                               nbknots = cknots.Length();

  NCollection_Array1<double> nknots(1, nbknots + 1);

  int i;
  for (i = 1; i < nbknots; i++)
  {
    nknots(i) = cknots(i);
  }

  nknots(nbknots + 1) = 2 * nknots(nbknots) - nknots(nbknots - 1);

  NCollection_Array1<int> nmults(1, nbknots + 1);

  const NCollection_Array1<int>& cmults = myData.Mults;

  for (i = 2; i <= nbknots; i++)
    nmults(i) = 1;
  nmults(1)           = cmults(1);
  nmults(nbknots + 1) = cmults(nbknots + 1);

  const NCollection_Array1<gp_Pnt2d>& cpoles  = myData.Poles;
  int                                  nbpoles = cpoles.Length();
  NCollection_Array1<gp_Pnt2d>        npoles(1, nbpoles + 1);

  // insert the pole

  for (i = 1; i <= Index; i++)
    npoles(i) = cpoles(i);

  npoles(Index + 1) = P;

  for (i = Index + 1; i <= nbpoles; i++)
    npoles(i + 1) = cpoles(i);

  // Insert the weight

  NCollection_Array1<double> nweights;
  bool rat = IsRational() || std::abs(Weight - 1.) > gp::Resolution();

  if (rat)
  {
    nweights.Resize(1, nbpoles + 1, false);

    for (i = 1; i <= Index; i++)
      if (IsRational())
        nweights(i) = myData.Weights.Value(i);
      else
        nweights(i) = 1.;

    nweights(Index + 1) = Weight;

    for (i = Index + 1; i <= nbpoles; i++)
      if (IsRational())
        nweights(i + 1) = myData.Weights.Value(i);
      else
        nweights(i + 1) = 1.;
  }

  myData.Poles   = std::move(npoles);
  if (rat)
    myData.Weights = std::move(nweights);
  else
    myData.Weights = NCollection_Array1<double>();
  myData.Knots   = std::move(nknots);
  myData.Mults   = std::move(nmults);
  maxderivinvok  = false;
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertPoleBefore(const int Index, const gp_Pnt2d& P, const double Weight)
{
  InsertPoleAfter(Index - 1, P, Weight);
}

//=================================================================================================

void Geom2d_BSplineCurve::RemovePole(const int Index)
{
  if (Index < 1 || Index > myData.Poles.Length())
    throw Standard_OutOfRange("BSpline curve: RemovePole: Index and #pole mismatch");

  if (myData.Poles.Length() <= 2)
    throw Standard_ConstructionError("BSpline curve: RemovePole: #pole is already minimum");

  if (knotSet == GeomAbs_NonUniform || knotSet == GeomAbs_PiecewiseBezier)
    throw Standard_ConstructionError("BSpline curve: RemovePole: bad knotSet type");

  int i;
  NCollection_Array1<double> nknots(1, myData.Knots.Length() - 1);

  NCollection_Array1<int> nmults(1, myData.Mults.Length() - 1);

  for (i = 1; i < nknots.Length(); i++)
  {
    nknots(i) = myData.Knots.Value(i);
    nmults(i) = 1;
  }
  nmults(1)              = myData.Mults.Value(1);
  nknots(nknots.Upper()) = myData.Knots.Value(myData.Knots.Upper());
  nmults(nmults.Upper()) = myData.Mults.Value(myData.Mults.Upper());

  NCollection_Array1<gp_Pnt2d> npoles(1, myData.Poles.Upper() - 1);

  for (i = 1; i < Index; i++)
    npoles(i) = myData.Poles.Value(i);
  for (i = Index; i < npoles.Length(); i++)
    npoles(i) = myData.Poles.Value(i + 1);

  NCollection_Array1<double> nweights;
  if (IsRational())
  {
    nweights.Resize(1, npoles.Length(), false);
    for (i = 1; i < Index; i++)
      nweights(i) = myData.Weights.Value(i);
    for (i = Index; i < nweights.Length(); i++)
      nweights(i) = myData.Weights.Value(i + 1);
  }

  myData.Poles = std::move(npoles);
  if (IsRational())
    myData.Weights = std::move(nweights);
  else
    myData.Weights = NCollection_Array1<double>();
  myData.Knots = std::move(nknots);
  myData.Mults = std::move(nmults);
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::Reverse()
{
  BSplCLib::Reverse(myData.Knots);
  BSplCLib::Reverse(myData.Mults);
  int last;
  if (myData.IsPeriodic)
    last = myData.FlatKnots.Upper() - myData.Degree - 1;
  else
    last = myData.Poles.Upper();
  BSplCLib::Reverse(myData.Poles, last);
  if (rational)
    BSplCLib::Reverse(myData.Weights, last);
  UpdateKnots();
}

//=================================================================================================

double Geom2d_BSplineCurve::ReversedParameter(const double U) const
{
  return (FirstParameter() + LastParameter() - U);
}

//=================================================================================================

void Geom2d_BSplineCurve::Segment(const double aU1, const double aU2, const double theTolerance)
{
  if (aU2 < aU1)
    throw Standard_DomainError("Geom2d_BSplineCurve::Segment");
  //
  double AbsUMax = std::max(std::abs(FirstParameter()), std::abs(LastParameter()));
  double Eps     = std::max(Epsilon(AbsUMax), theTolerance);
  double NewU1, NewU2;
  double U, DU = 0;
  int    i, k, index;
  //
  // f
  double U1 = aU1, U2 = aU2;
  //
  // Henceforward we use U1, U2 as bounds of the segment
  // t
  //
  NCollection_Array1<double> Knots(1, 2);
  NCollection_Array1<int>    Mults(1, 2);
  //
  // define param distance to keep (eap, Apr 18 2002, occ311)
  if (myData.IsPeriodic)
  {
    double Period = LastParameter() - FirstParameter();
    DU            = U2 - U1;
    if (DU - Period > Precision::PConfusion())
      throw Standard_DomainError("Geom2d_BSplineCurve::Segment");
    if (DU > Period)
      DU = Period;
  }
  //
  index = 0;
  BSplCLib::LocateParameter(myData.Degree,
                            myData.Knots,
                            myData.Mults,
                            U1,
                            myData.IsPeriodic,
                            myData.Knots.Lower(),
                            myData.Knots.Upper(),
                            index,
                            NewU1);
  index = 0;
  BSplCLib::LocateParameter(myData.Degree,
                            myData.Knots,
                            myData.Mults,
                            U2,
                            myData.IsPeriodic,
                            myData.Knots.Lower(),
                            myData.Knots.Upper(),
                            index,
                            NewU2);
  Knots(1) = std::min(NewU1, NewU2);
  Knots(2) = std::max(NewU1, NewU2);
  Mults(1) = Mults(2) = myData.Degree;
  InsertKnots(Knots, Mults, Eps);

  if (myData.IsPeriodic)
  { // set the origine at NewU1
    index = 0;
    BSplCLib::LocateParameter(myData.Degree,
                              myData.Knots,
                              myData.Mults,
                              U1,
                              myData.IsPeriodic,
                              myData.Knots.Lower(),
                              myData.Knots.Upper(),
                              index,
                              U);
    if (std::abs(myData.Knots.Value(index + 1) - U) <= Eps)
    {
      index++;
    }
    SetOrigin(index);
    SetNotPeriodic();
    NewU2 = NewU1 + DU;
  }

  // compute index1 and index2 to set the new knots and mults
  int index1 = 0, index2 = 0;
  int FromU1 = myData.Knots.Lower();
  int ToU2   = myData.Knots.Upper();
  BSplCLib::LocateParameter(myData.Degree,
                            myData.Knots,
                            myData.Mults,
                            NewU1,
                            myData.IsPeriodic,
                            FromU1,
                            ToU2,
                            index1,
                            U);
  if (std::abs(myData.Knots.Value(index1 + 1) - U) <= Eps)
  {
    index1++;
  }
  BSplCLib::LocateParameter(myData.Degree,
                            myData.Knots,
                            myData.Mults,
                            NewU2,
                            myData.IsPeriodic,
                            FromU1,
                            ToU2,
                            index2,
                            U);
  if (std::abs(myData.Knots.Value(index2 + 1) - U) <= Eps || index2 == index1)
  {
    index2++;
  }

  int nbknots = index2 - index1 + 1;

  NCollection_Array1<double> nknots(1, nbknots);
  NCollection_Array1<int>    nmults(1, nbknots);

  // to restore changed U1
  if (DU > 0)
  { // if was periodic
    DU = NewU1 - U1;
  }
  //
  k = 1;
  //
  for (i = index1; i <= index2; i++)
  {
    nknots.SetValue(k, myData.Knots.Value(i) - DU);
    nmults.SetValue(k, myData.Mults.Value(i));
    k++;
  }
  nmults.SetValue(1, myData.Degree + 1);
  nmults.SetValue(nbknots, myData.Degree + 1);

  // compute index1 and index2 to set the new poles and weights
  int pindex1 = BSplCLib::PoleIndex(myData.Degree, index1, myData.IsPeriodic, myData.Mults);
  int pindex2 = BSplCLib::PoleIndex(myData.Degree, index2, myData.IsPeriodic, myData.Mults);

  pindex1++;
  pindex2 = std::min(pindex2 + 1, myData.Poles.Length());

  int nbpoles = pindex2 - pindex1 + 1;

  NCollection_Array1<double>   nweights;
  NCollection_Array1<gp_Pnt2d> npoles(1, nbpoles);

  k = 1;
  if (rational)
  {
    nweights.Resize(1, nbpoles, false);
    for (i = pindex1; i <= pindex2; i++)
    {
      npoles.SetValue(k, myData.Poles.Value(i));
      nweights.SetValue(k, myData.Weights.Value(i));
      k++;
    }
  }
  else
  {
    for (i = pindex1; i <= pindex2; i++)
    {
      npoles.SetValue(k, myData.Poles.Value(i));
      k++;
    }
  }

  myData.Knots = std::move(nknots);
  myData.Mults = std::move(nmults);
  myData.Poles = std::move(npoles);
  if (rational)
  {
    myData.Weights = std::move(nweights);
  }
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetKnot(const int Index, const double K)
{
  if (Index < 1 || Index > myData.Knots.Length())
    throw Standard_OutOfRange("BSpline curve: SetKnot: Index and #knots mismatch");
  double DK = std::abs(Epsilon(K));
  if (Index == 1)
  {
    if (K >= myData.Knots.Value(2) - DK)
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
  }
  else if (Index == myData.Knots.Length())
  {
    if (K <= myData.Knots.Value(myData.Knots.Length() - 1) + DK)
    {
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
    }
  }
  else
  {
    if (K <= myData.Knots.Value(Index - 1) + DK || K >= myData.Knots.Value(Index + 1) - DK)
    {
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
    }
  }
  if (K != myData.Knots.Value(Index))
  {
    myData.Knots.SetValue(Index, K);
    maxderivinvok = false;
    UpdateKnots();
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::SetKnots(const NCollection_Array1<double>& K)
{
  CheckCurveData(myData.Poles, K, myData.Mults, myData.Degree, myData.IsPeriodic);
  myData.Knots  = K;
  maxderivinvok = false;
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetKnot(const int Index, const double K, const int M)
{
  IncreaseMultiplicity(Index, M);
  SetKnot(Index, K);
}

//=================================================================================================

void Geom2d_BSplineCurve::SetPeriodic()
{
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  NCollection_Array1<double> cknots(myData.Knots(first), first, last);
  myData.Knots.Resize(1, cknots.Length(), false);
  myData.Knots.Assign(cknots);

  NCollection_Array1<int> cmults(myData.Mults(first), first, last);
  cmults(first) = cmults(last) = std::min(myData.Degree, std::max(cmults(first), cmults(last)));
  myData.Mults.Resize(1, cmults.Length(), false);
  myData.Mults.Assign(cmults);

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(myData.Degree, true, cmults);

  NCollection_Array1<gp_Pnt2d> cpoles(myData.Poles(1), 1, nbp);
  myData.Poles.Resize(1, nbp, false);
  myData.Poles.Assign(cpoles);

  if (rational)
  {
    NCollection_Array1<double> cweights(myData.Weights(1), 1, nbp);
    myData.Weights.Resize(1, nbp, false);
    myData.Weights.Assign(cweights);
  }

  myData.IsPeriodic = true;

  maxderivinvok = false;
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetOrigin(const int Index)
{
  if (!myData.IsPeriodic)
    throw Standard_NoSuchObject("Geom2d_BSplineCurve::SetOrigin");
  int i, k;
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom2d_BSplineCurve::SetOrigin");

  int nbknots = myData.Knots.Length();
  int nbpoles = myData.Poles.Length();

  NCollection_Array1<double> nknots(1, nbknots);

  NCollection_Array1<int> nmults(1, nbknots);

  // set the knots and mults
  double period = myData.Knots.Value(last) - myData.Knots.Value(first);
  k             = 1;
  for (i = Index; i <= last; i++)
  {
    nknots(k) = myData.Knots.Value(i);
    nmults(k) = myData.Mults.Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    nknots(k) = myData.Knots.Value(i) + period;
    nmults(k) = myData.Mults.Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += myData.Mults.Value(i);

  // set the poles and weights
  NCollection_Array1<gp_Pnt2d> npoles(1, nbpoles);
  NCollection_Array1<double>   nweights(1, nbpoles);
  first = myData.Poles.Lower();
  last  = myData.Poles.Upper();
  if (rational)
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      npoles(k)   = myData.Poles.Value(i);
      nweights(k) = myData.Weights.Value(i);
      k++;
    }
    for (i = first; i < index; i++)
    {
      npoles(k)   = myData.Poles.Value(i);
      nweights(k) = myData.Weights.Value(i);
      k++;
    }
  }
  else
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      npoles(k) = myData.Poles.Value(i);
      k++;
    }
    for (i = first; i < index; i++)
    {
      npoles(k) = myData.Poles.Value(i);
      k++;
    }
  }

  myData.Poles = std::move(npoles);
  myData.Knots = std::move(nknots);
  myData.Mults = std::move(nmults);
  if (rational)
    myData.Weights = std::move(nweights);
  maxderivinvok = false;
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetNotPeriodic()
{
  if (myData.IsPeriodic)
  {
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(myData.Degree, myData.Mults, NbKnots, NbPoles);

    NCollection_Array1<gp_Pnt2d> npoles(1, NbPoles);

    NCollection_Array1<double> nknots(1, NbKnots);

    NCollection_Array1<int> nmults(1, NbKnots);

    NCollection_Array1<double> nweights;
    if (IsRational())
    {
      nweights.Resize(1, NbPoles, false);
    }

    BSplCLib::Unperiodize(myData.Degree,
                          myData.Mults,
                          myData.Knots,
                          myData.Poles,
                          rational ? &myData.Weights : BSplCLib::NoWeights(),
                          nmults,
                          nknots,
                          npoles,
                          rational ? &nweights : BSplCLib::NoWeights());
    myData.Poles = std::move(npoles);
    if (rational)
      myData.Weights = std::move(nweights);
    else
      myData.Weights = NCollection_Array1<double>();
    myData.Mults      = std::move(nmults);
    myData.Knots      = std::move(nknots);
    myData.IsPeriodic = false;
    maxderivinvok     = false;
    UpdateKnots();
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::SetPole(const int Index, const gp_Pnt2d& P)
{
  if (Index < 1 || Index > myData.Poles.Length())
    throw Standard_OutOfRange("BSpline curve: SetPole: index and #pole mismatch");
  myData.Poles.SetValue(Index, P);
  maxderivinvok = false;
}

//=================================================================================================

void Geom2d_BSplineCurve::SetPole(const int Index, const gp_Pnt2d& P, const double W)
{
  SetPole(Index, P);
  SetWeight(Index, W);
}

//=================================================================================================

void Geom2d_BSplineCurve::SetWeight(const int Index, const double W)
{
  if (Index < 1 || Index > myData.Poles.Length())
    throw Standard_OutOfRange("BSpline curve: SetWeight: Index and #pole mismatch");

  if (W <= gp::Resolution())
    throw Standard_ConstructionError("BSpline curve: SetWeight: Weight too small");

  bool rat = IsRational() || (std::abs(W - 1.) > gp::Resolution());

  if (rat)
  {
    if (rat && !IsRational())
    {
      myData.Weights.Resize(1, myData.Poles.Length(), false);
      myData.Weights.Init(1.);
    }

    myData.Weights.SetValue(Index, W);

    if (IsRational())
    {
      rat = Rational(myData.Weights);
      if (!rat)
        myData.Weights = NCollection_Array1<double>();
    }

    rational = myData.Weights.Size() > 0;
  }

  maxderivinvok = false;
}

//=================================================================================================

void Geom2d_BSplineCurve::MovePoint(const double    U,
                                    const gp_Pnt2d& P,
                                    const int       Index1,
                                    const int       Index2,
                                    int&            FirstModifiedPole,
                                    int&            LastmodifiedPole)
{
  if (Index1 < 1 || Index1 > myData.Poles.Length() || Index2 < 1 || Index2 > myData.Poles.Length()
      || Index1 > Index2)
  {
    throw Standard_OutOfRange("BSpline curve: MovePoint: Index and #pole mismatch");
  }
  NCollection_Array1<gp_Pnt2d> npoles(1, myData.Poles.Length());
  gp_Pnt2d                     P0;
  D0(U, P0);
  gp_Vec2d Displ(P0, P);

  BSplCLib::MovePoint(U,
                      Displ,
                      Index1,
                      Index2,
                      myData.Degree,
                      myData.Poles,
                      rational ? &myData.Weights : BSplCLib::NoWeights(),
                      myData.FlatKnots,
                      FirstModifiedPole,
                      LastmodifiedPole,
                      npoles);
  if (FirstModifiedPole)
  {
    myData.Poles  = std::move(npoles);
    maxderivinvok = false;
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::MovePointAndTangent(const double    U,
                                              const gp_Pnt2d& P,
                                              const gp_Vec2d& Tangent,
                                              const double    Tolerance,
                                              const int       StartingCondition,
                                              const int       EndingCondition,
                                              int&            ErrorStatus)
{
  int ii;
  if (IsPeriodic())
  {
    //
    // for the time being do not deal with periodic curves
    //
    SetNotPeriodic();
  }
  NCollection_Array1<gp_Pnt2d> new_poles(1, myData.Poles.Length());
  gp_Pnt2d                     P0;

  gp_Vec2d delta_derivative;
  D1(U, P0, delta_derivative);
  gp_Vec2d delta(P0, P);
  for (ii = 1; ii <= 2; ii++)
  {
    delta_derivative.SetCoord(ii, Tangent.Coord(ii) - delta_derivative.Coord(ii));
  }

  BSplCLib::MovePointAndTangent(U,
                                delta,
                                delta_derivative,
                                Tolerance,
                                myData.Degree,
                                StartingCondition,
                                EndingCondition,
                                myData.Poles,
                                rational ? &myData.Weights : BSplCLib::NoWeights(),
                                myData.FlatKnots,
                                new_poles,
                                ErrorStatus);
  if (!ErrorStatus)
  {
    myData.Poles  = std::move(new_poles);
    maxderivinvok = false;
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::UpdateKnots()
{

  rational = myData.Weights.Size() > 0;

  int MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(myData.Degree, myData.IsPeriodic, myData.Knots, myData.Mults, knotSet, MaxKnotMult);

  if (knotSet == GeomAbs_Uniform && !myData.IsPeriodic)
  {
    myData.FlatKnots.Resize(myData.Knots.Lower(), myData.Knots.Upper(), false);
    myData.FlatKnots.Assign(myData.Knots);
  }
  else
  {
    myData.FlatKnots.Resize(1,
                            BSplCLib::KnotSequenceLength(myData.Mults, myData.Degree, myData.IsPeriodic),
                            false);

    BSplCLib::KnotSequence(myData.Knots,
                           myData.Mults,
                           myData.Degree,
                           myData.IsPeriodic,
                           myData.FlatKnots);
  }

  if (MaxKnotMult == 0)
    smooth = GeomAbs_CN;
  else
  {
    switch (myData.Degree - MaxKnotMult)
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

//=================================================================================================

void Geom2d_BSplineCurve::PeriodicNormalization(double& Parameter) const
{
  double Period;

  if (myData.IsPeriodic)
  {
    Period = myData.FlatKnots.Value(myData.FlatKnots.Upper() - myData.Degree)
           - myData.FlatKnots.Value(myData.Degree + 1);
    while (Parameter > myData.FlatKnots.Value(myData.FlatKnots.Upper() - myData.Degree))
    {
      Parameter -= Period;
    }
    while (Parameter < myData.FlatKnots.Value((myData.Degree + 1)))
    {
      Parameter += Period;
    }
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, rational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.IsPeriodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, knotSet)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, smooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Degree)
  if (myData.Poles.Size() > 0)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Poles.Size())

  if (rational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Weights.Size())
  if (myData.FlatKnots.Size() > 0)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.FlatKnots.Size())
  if (myData.Knots.Size() > 0)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Knots.Size())
  if (myData.Mults.Size() > 0)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Mults.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
