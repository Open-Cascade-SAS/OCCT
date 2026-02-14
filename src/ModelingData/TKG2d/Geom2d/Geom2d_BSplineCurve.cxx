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
#include "Geom2d_EvalRepCurveDesc.hxx"
#include "Geom2d_EvalRepUtils.pxx"
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
#include <Standard_ProgramError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_BSplineCurve, Geom2d_BoundedCurve)

//=================================================================================================

void Geom2d_BSplineCurve::SetEvalRepresentation(
  const occ::handle<Geom2d_EvalRepCurveDesc::Base>& theDesc)
{
  Geom2d_EvalRepUtils::ValidateCurveDesc(theDesc, this);
  myEvalRep = theDesc;
}

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
    : myPoles(theOther.myPoles),
      myWeights(theOther.myRational ? NCollection_Array1<double>(theOther.myWeights)
                                    : BSplCLib::UnitWeights(theOther.myPoles.Length())),
      myKnots(theOther.myKnots),
      myFlatKnots(theOther.myFlatKnots),
      myMults(theOther.myMults),
      myEvalRep(Geom2d_EvalRepUtils::CloneCurveDesc(theOther.myEvalRep)),
      myDeg(theOther.myDeg),
      myPeriodic(theOther.myPeriodic),
      myRational(theOther.myRational),
      myKnotSet(theOther.myKnotSet),
      mySmooth(theOther.mySmooth),
      myMaxDerivInv(theOther.myMaxDerivInv),
      myMaxDerivInvOk(false)
{
}

//=================================================================================================

Geom2d_BSplineCurve::Geom2d_BSplineCurve(const NCollection_Array1<gp_Pnt2d>& Poles,
                                         const NCollection_Array1<double>&   Knots,
                                         const NCollection_Array1<int>&      Mults,
                                         const int                           Degree,
                                         const bool                          Periodic)
    : myRational(false),
      myMaxDerivInv(0.0),
      myMaxDerivInvOk(false)
{
  myDeg      = Degree;
  myPeriodic = Periodic;

  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  // copy arrays

  myPoles.Resize(1, Poles.Length(), false);
  myPoles.Assign(Poles);

  myWeights = BSplCLib::UnitWeights(Poles.Length());

  myKnots.Resize(1, Knots.Length(), false);
  myKnots.Assign(Knots);

  myMults.Resize(1, Mults.Length(), false);
  myMults.Assign(Mults);

  updateKnots();
}

//=================================================================================================

Geom2d_BSplineCurve::Geom2d_BSplineCurve(const NCollection_Array1<gp_Pnt2d>& Poles,
                                         const NCollection_Array1<double>&   Weights,
                                         const NCollection_Array1<double>&   Knots,
                                         const NCollection_Array1<int>&      Mults,
                                         const int                           Degree,
                                         const bool                          Periodic)
    : myRational(true),
      myMaxDerivInv(0.0),
      myMaxDerivInvOk(false)
{
  myDeg      = Degree;
  myPeriodic = Periodic;

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
  myRational = Rational(Weights);

  // copy arrays

  myPoles.Resize(1, Poles.Length(), false);
  myPoles.Assign(Poles);
  if (myRational)
  {
    myWeights.Resize(1, Weights.Length(), false);
    myWeights.Assign(Weights);
  }
  else
  {
    myWeights = BSplCLib::UnitWeights(Poles.Length());
  }

  myKnots.Resize(1, Knots.Length(), false);
  myKnots.Assign(Knots);

  myMults.Resize(1, Mults.Length(), false);
  myMults.Assign(Mults);

  updateKnots();
}

//=================================================================================================

int Geom2d_BSplineCurve::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

void Geom2d_BSplineCurve::IncreaseDegree(const int Degree)
{
  if (Degree == myDeg)
    return;

  if (Degree < myDeg || Degree > Geom2d_BSplineCurve::MaxDegree())
  {
    throw Standard_ConstructionError("BSpline curve: IncreaseDegree: bad degree value");
  }

  int FromK1 = FirstUKnotIndex();
  int ToK2   = LastUKnotIndex();

  int Step = Degree - myDeg;

  NCollection_Array1<gp_Pnt2d> npoles(1, myPoles.Length() + Step * (ToK2 - FromK1));

  int nbknots = BSplCLib::IncreaseDegreeCountKnots(myDeg, Degree, myPeriodic, myMults);

  NCollection_Array1<double> nknots(1, nbknots);

  NCollection_Array1<int> nmults(1, nbknots);

  NCollection_Array1<double> nweights;

  if (IsRational())
  {
    nweights.Resize(1, npoles.Upper(), false);
  }

  BSplCLib::IncreaseDegree(myDeg,
                           Degree,
                           myPeriodic,
                           myPoles,
                           Weights(),
                           myKnots,
                           myMults,
                           npoles,
                           myRational ? &nweights : BSplCLib::NoWeights(),
                           nknots,
                           nmults);

  myDeg   = Degree;
  myPoles = std::move(npoles);
  if (IsRational())
    myWeights = std::move(nweights);
  else
    myWeights = BSplCLib::UnitWeights(myPoles.Length());
  myKnots = std::move(nknots);
  myMults = std::move(nmults);
  updateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::IncreaseMultiplicity(const int Index, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = myKnots.Value(Index);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - myMults.Value(Index);
  InsertKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom2d_BSplineCurve::IncreaseMultiplicity(const int I1, const int I2, const int M)
{
  NCollection_Array1<double> k(myKnots(I1), I1, I2);
  NCollection_Array1<int>    m(I1, I2);
  int                        i;
  for (i = I1; i <= I2; i++)
    m(i) = M - myMults.Value(i);
  InsertKnots(k, m, Epsilon(1.), true);
}

//=================================================================================================

void Geom2d_BSplineCurve::IncrementMultiplicity(const int I1, const int I2, const int Step)
{
  NCollection_Array1<double> k(myKnots(I1), I1, I2);
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

  if (!BSplCLib::PrepareInsertKnots(myDeg,
                                    myPeriodic,
                                    myKnots,
                                    myMults,
                                    Knots,
                                    &Mults,
                                    nbpoles,
                                    nbknots,
                                    Epsilon,
                                    Add))
    throw Standard_ConstructionError("Geom2d_BSplineCurve::InsertKnots");

  if (nbpoles == myPoles.Length())
    return;

  NCollection_Array1<gp_Pnt2d> npoles(1, nbpoles);
  NCollection_Array1<double>   nknots(1, nbknots);
  NCollection_Array1<int>      nmults(1, nbknots);

  NCollection_Array1<double> nweights;
  if (myRational)
  {
    nweights.Resize(1, nbpoles, false);
  }

  BSplCLib::InsertKnots(myDeg,
                        myPeriodic,
                        myPoles,
                        Weights(),
                        myKnots,
                        myMults,
                        Knots,
                        &Mults,
                        npoles,
                        myRational ? &nweights : BSplCLib::NoWeights(),
                        nknots,
                        nmults,
                        Epsilon,
                        Add);
  if (myRational)
    myWeights = std::move(nweights);
  else
    myWeights = BSplCLib::UnitWeights(npoles.Length());
  myPoles = std::move(npoles);
  myKnots = std::move(nknots);
  myMults = std::move(nmults);
  updateKnots();
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

  const NCollection_Array1<gp_Pnt2d>& oldpoles = myPoles;

  int step = myMults.Value(Index) - M;
  if (step <= 0)
    return true;

  NCollection_Array1<gp_Pnt2d> npoles(1, oldpoles.Length() - step);

  NCollection_Array1<double> nknots(1, myKnots.Length() - (M == 0 ? 1 : 0));
  NCollection_Array1<int>    nmults(1, myMults.Length() - (M == 0 ? 1 : 0));

  NCollection_Array1<double> nweights;
  if (IsRational())
  {
    nweights.Resize(1, npoles.Length(), false);
  }

  if (!BSplCLib::RemoveKnot(Index,
                            M,
                            myDeg,
                            myPeriodic,
                            myPoles,
                            Weights(),
                            myKnots,
                            myMults,
                            npoles,
                            myRational ? &nweights : BSplCLib::NoWeights(),
                            nknots,
                            nmults,
                            Tolerance))
  {
    return false;
  }

  if (IsRational())
    myWeights = std::move(nweights);
  else
    myWeights = BSplCLib::UnitWeights(npoles.Length());
  myPoles = std::move(npoles);
  myKnots = std::move(nknots);
  myMults = std::move(nmults);

  updateKnots();
  myMaxDerivInvOk = false;
  return true;
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertPoleAfter(const int Index, const gp_Pnt2d& P, const double Weight)
{
  if (Index < 0 || Index > myPoles.Length())
    throw Standard_OutOfRange("BSpline curve: InsertPoleAfter: Index and #pole mismatch");

  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("BSpline curve: InsertPoleAfter: Weight too small");

  if (myKnotSet == GeomAbs_NonUniform || myKnotSet == GeomAbs_PiecewiseBezier)
  {
    throw Standard_ConstructionError("BSpline curve: InsertPoleAfter: bad knotSet type");
  }

  const NCollection_Array1<double>& cknots  = myKnots;
  int                               nbknots = cknots.Length();

  NCollection_Array1<double> nknots(1, nbknots + 1);

  int i;
  for (i = 1; i <= nbknots; i++)
  {
    nknots(i) = cknots(i);
  }

  nknots(nbknots + 1) = 2 * nknots(nbknots) - nknots(nbknots - 1);

  NCollection_Array1<int> nmults(1, nbknots + 1);

  const NCollection_Array1<int>& cmults = myMults;

  for (i = 2; i <= nbknots; i++)
    nmults(i) = 1;
  nmults(1)           = cmults(1);
  nmults(nbknots + 1) = cmults(nbknots);

  const NCollection_Array1<gp_Pnt2d>& cpoles  = myPoles;
  int                                 nbpoles = cpoles.Length();
  NCollection_Array1<gp_Pnt2d>        npoles(1, nbpoles + 1);

  // insert the pole

  for (i = 1; i <= Index; i++)
    npoles(i) = cpoles(i);

  npoles(Index + 1) = P;

  for (i = Index + 1; i <= nbpoles; i++)
    npoles(i + 1) = cpoles(i);

  // Insert the weight

  NCollection_Array1<double> nweights;
  bool                       rat = IsRational() || std::abs(Weight - 1.) > gp::Resolution();

  if (rat)
  {
    nweights.Resize(1, nbpoles + 1, false);

    for (i = 1; i <= Index; i++)
      if (IsRational())
        nweights(i) = myWeights.Value(i);
      else
        nweights(i) = 1.;

    nweights(Index + 1) = Weight;

    for (i = Index + 1; i <= nbpoles; i++)
      if (IsRational())
        nweights(i + 1) = myWeights.Value(i);
      else
        nweights(i + 1) = 1.;
  }

  myPoles = std::move(npoles);
  if (rat)
    myWeights = std::move(nweights);
  else
    myWeights = BSplCLib::UnitWeights(myPoles.Length());
  myRational      = rat;
  myKnots         = std::move(nknots);
  myMults         = std::move(nmults);
  myMaxDerivInvOk = false;
  updateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertPoleBefore(const int Index, const gp_Pnt2d& P, const double Weight)
{
  InsertPoleAfter(Index - 1, P, Weight);
}

//=================================================================================================

void Geom2d_BSplineCurve::RemovePole(const int Index)
{
  if (Index < 1 || Index > myPoles.Length())
    throw Standard_OutOfRange("BSpline curve: RemovePole: Index and #pole mismatch");

  if (myPoles.Length() <= 2)
    throw Standard_ConstructionError("BSpline curve: RemovePole: #pole is already minimum");

  if (myKnotSet == GeomAbs_NonUniform || myKnotSet == GeomAbs_PiecewiseBezier)
    throw Standard_ConstructionError("BSpline curve: RemovePole: bad knotSet type");

  int                        i;
  NCollection_Array1<double> nknots(1, myKnots.Length() - 1);

  NCollection_Array1<int> nmults(1, myMults.Length() - 1);

  for (i = 1; i < nknots.Length(); i++)
  {
    nknots(i) = myKnots.Value(i);
    nmults(i) = 1;
  }
  nmults(1)              = myMults.Value(1);
  nknots(nknots.Upper()) = myKnots.Value(myKnots.Upper());
  nmults(nmults.Upper()) = myMults.Value(myMults.Upper());

  NCollection_Array1<gp_Pnt2d> npoles(1, myPoles.Upper() - 1);

  for (i = 1; i < Index; i++)
    npoles(i) = myPoles.Value(i);
  for (i = Index; i <= npoles.Length(); i++)
    npoles(i) = myPoles.Value(i + 1);

  NCollection_Array1<double> nweights;
  if (IsRational())
  {
    nweights.Resize(1, npoles.Length(), false);
    for (i = 1; i < Index; i++)
      nweights(i) = myWeights.Value(i);
    for (i = Index; i <= nweights.Length(); i++)
      nweights(i) = myWeights.Value(i + 1);
  }

  myPoles = std::move(npoles);
  if (IsRational())
  {
    myWeights  = std::move(nweights);
    myRational = Rational(myWeights);
    if (!myRational)
      myWeights = BSplCLib::UnitWeights(myPoles.Length());
  }
  else
  {
    myWeights = BSplCLib::UnitWeights(myPoles.Length());
  }
  myKnots = std::move(nknots);
  myMults = std::move(nmults);
  updateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::Reverse()
{
  BSplCLib::Reverse(myKnots);
  BSplCLib::Reverse(myMults);
  int last;
  if (myPeriodic)
    last = myFlatKnots.Upper() - myDeg - 1;
  else
    last = myPoles.Upper();
  BSplCLib::Reverse(myPoles, last);
  if (myRational)
    BSplCLib::Reverse(myWeights, last);
  updateKnots();
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

  double NewU1, NewU2;
  double U, DU = 0, aDDU = 0;
  int    i, k, index;
  bool   wasPeriodic = myPeriodic;

  double U1 = aU1, U2 = aU2;

  NCollection_Array1<double> Knots(1, 2);
  NCollection_Array1<int>    Mults(1, 2);

  // define param distance to keep (eap, Apr 18 2002, occ311)
  if (myPeriodic)
  {
    double Period = LastParameter() - FirstParameter();
    DU            = U2 - U1;
    if (DU - Period > Precision::PConfusion())
      throw Standard_DomainError("Geom2d_BSplineCurve::Segment");
    if (DU > Period)
      DU = Period;
    aDDU = DU;
  }

  index = 0;
  BSplCLib::LocateParameter(myDeg,
                            myKnots,
                            myMults,
                            U1,
                            myPeriodic,
                            myKnots.Lower(),
                            myKnots.Upper(),
                            index,
                            NewU1);
  index = 0;
  BSplCLib::LocateParameter(myDeg,
                            myKnots,
                            myMults,
                            U2,
                            myPeriodic,
                            myKnots.Lower(),
                            myKnots.Upper(),
                            index,
                            NewU2);

  //-- DBB
  double aNu2 = NewU2;
  //-- DBB

  double AbsUMax = std::max(std::abs(NewU1), std::abs(NewU2));
  AbsUMax    = std::max(AbsUMax, std::max(std::abs(FirstParameter()), std::abs(LastParameter())));
  double Eps = std::max(Epsilon(AbsUMax), theTolerance);

  Knots(1) = std::min(NewU1, NewU2);
  Knots(2) = std::max(NewU1, NewU2);
  Mults(1) = Mults(2) = myDeg;
  InsertKnots(Knots, Mults, Eps);

  if (myPeriodic)
  { // set the origine at NewU1
    index = 0;
    BSplCLib::LocateParameter(myDeg,
                              myKnots,
                              myMults,
                              U1,
                              myPeriodic,
                              myKnots.Lower(),
                              myKnots.Upper(),
                              index,
                              U);
    if (std::abs(myKnots.Value(index + 1) - U) <= Eps)
    {
      index++;
    }
    SetOrigin(index);
    SetNotPeriodic();
    NewU2 = NewU1 + DU;
  }

  // compute index1 and index2 to set the new knots and mults
  int index1 = 0, index2 = 0;
  int FromU1 = myKnots.Lower();
  int ToU2   = myKnots.Upper();
  BSplCLib::LocateParameter(myDeg, myKnots, myMults, NewU1, myPeriodic, FromU1, ToU2, index1, U);
  if (std::abs(myKnots.Value(index1 + 1) - U) <= Eps)
  {
    index1++;
  }
  BSplCLib::LocateParameter(myDeg, myKnots, myMults, NewU2, myPeriodic, FromU1, ToU2, index2, U);
  if (std::abs(myKnots.Value(index2 + 1) - U) <= Eps || index2 == index1)
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
    nknots.SetValue(k, myKnots.Value(i) - DU);
    nmults.SetValue(k, myMults.Value(i));
    k++;
  }
  nmults.SetValue(1, myDeg + 1);
  nmults.SetValue(nbknots, myDeg + 1);

  // compute index1 and index2 to set the new poles and weights
  int pindex1 = BSplCLib::PoleIndex(myDeg, index1, myPeriodic, myMults);
  int pindex2 = BSplCLib::PoleIndex(myDeg, index2, myPeriodic, myMults);

  pindex1++;
  pindex2 = std::min(pindex2 + 1, myPoles.Length());

  int nbpoles = pindex2 - pindex1 + 1;

  NCollection_Array1<double>   nweights;
  NCollection_Array1<gp_Pnt2d> npoles(1, nbpoles);

  k = 1;
  if (myRational)
  {
    nweights.Resize(1, nbpoles, false);
    for (i = pindex1; i <= pindex2; i++)
    {
      npoles.SetValue(k, myPoles.Value(i));
      nweights.SetValue(k, myWeights.Value(i));
      k++;
    }
  }
  else
  {
    for (i = pindex1; i <= pindex2; i++)
    {
      npoles.SetValue(k, myPoles.Value(i));
      k++;
    }
  }

  //-- DBB
  if (wasPeriodic)
  {
    nknots.ChangeValue(nknots.Lower()) = U1;
    if (aNu2 < U2)
    {
      nknots.ChangeValue(nknots.Upper()) = U1 + aDDU;
    }
  }
  //-- DBB

  myKnots = std::move(nknots);
  myMults = std::move(nmults);
  myPoles = std::move(npoles);
  if (myRational)
    myWeights = std::move(nweights);
  else
    myWeights = BSplCLib::UnitWeights(myPoles.Length());
  myMaxDerivInvOk = false;
  updateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetKnot(const int Index, const double K)
{
  if (Index < 1 || Index > myKnots.Length())
    throw Standard_OutOfRange("BSpline curve: SetKnot: Index and #knots mismatch");
  double DK = std::abs(Epsilon(K));
  if (Index == 1)
  {
    if (K >= myKnots.Value(2) - DK)
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
  }
  else if (Index == myKnots.Length())
  {
    if (K <= myKnots.Value(myKnots.Length() - 1) + DK)
    {
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
    }
  }
  else
  {
    if (K <= myKnots.Value(Index - 1) + DK || K >= myKnots.Value(Index + 1) - DK)
    {
      throw Standard_ConstructionError("BSpline curve: SetKnot: K out of range");
    }
  }
  if (K != myKnots.Value(Index))
  {
    myKnots.SetValue(Index, K);
    myMaxDerivInvOk = false;
    updateKnots();
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::SetKnots(const NCollection_Array1<double>& K)
{
  CheckCurveData(myPoles, K, myMults, myDeg, myPeriodic);
  myKnots         = K;
  myMaxDerivInvOk = false;
  updateKnots();
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

  NCollection_Array1<double> cknots(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cknots(k - first + 1) = myKnots(k);
  myKnots = std::move(cknots);

  NCollection_Array1<int> cmults(1, last - first + 1);
  for (int k = first; k <= last; k++)
    cmults(k - first + 1) = myMults(k);
  cmults(1) = cmults(cmults.Upper()) = std::min(myDeg, std::max(cmults(1), cmults(cmults.Upper())));
  myMults                            = std::move(cmults);

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(myDeg, true, myMults);

  myPoles.Resize(1, nbp, true);
  if (myRational)
    myWeights.Resize(1, nbp, true);
  else
    myWeights = BSplCLib::UnitWeights(nbp);

  myPeriodic = true;

  myMaxDerivInvOk = false;
  updateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetOrigin(const int Index)
{
  if (!myPeriodic)
    throw Standard_NoSuchObject("Geom2d_BSplineCurve::SetOrigin");
  int i, k;
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom2d_BSplineCurve::SetOrigin");

  int nbknots = myKnots.Length();
  int nbpoles = myPoles.Length();

  NCollection_Array1<double> nknots(1, nbknots);

  NCollection_Array1<int> nmults(1, nbknots);

  // set the knots and mults
  double period = myKnots.Value(last) - myKnots.Value(first);
  k             = 1;
  for (i = Index; i <= last; i++)
  {
    nknots(k) = myKnots.Value(i);
    nmults(k) = myMults.Value(i);
    k++;
  }
  for (i = first + 1; i <= Index; i++)
  {
    nknots(k) = myKnots.Value(i) + period;
    nmults(k) = myMults.Value(i);
    k++;
  }

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += myMults.Value(i);

  // set the poles and weights
  NCollection_Array1<gp_Pnt2d> npoles(1, nbpoles);
  NCollection_Array1<double>   nweights;
  first = myPoles.Lower();
  last  = myPoles.Upper();
  if (myRational)
  {
    nweights.Resize(1, nbpoles, false);
    k = 1;
    for (i = index; i <= last; i++)
    {
      npoles(k)   = myPoles.Value(i);
      nweights(k) = myWeights.Value(i);
      k++;
    }
    for (i = first; i < index; i++)
    {
      npoles(k)   = myPoles.Value(i);
      nweights(k) = myWeights.Value(i);
      k++;
    }
  }
  else
  {
    k = 1;
    for (i = index; i <= last; i++)
    {
      npoles(k) = myPoles.Value(i);
      k++;
    }
    for (i = first; i < index; i++)
    {
      npoles(k) = myPoles.Value(i);
      k++;
    }
  }

  myPoles = std::move(npoles);
  myKnots = std::move(nknots);
  myMults = std::move(nmults);
  if (myRational)
    myWeights = std::move(nweights);
  else
    myWeights = BSplCLib::UnitWeights(nbpoles);
  myMaxDerivInvOk = false;
  updateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetNotPeriodic()
{
  if (myPeriodic)
  {
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(myDeg, myMults, NbKnots, NbPoles);

    NCollection_Array1<gp_Pnt2d> npoles(1, NbPoles);

    NCollection_Array1<double> nknots(1, NbKnots);

    NCollection_Array1<int> nmults(1, NbKnots);

    NCollection_Array1<double> nweights;
    if (IsRational())
    {
      nweights.Resize(1, NbPoles, false);
    }

    BSplCLib::Unperiodize(myDeg,
                          myMults,
                          myKnots,
                          myPoles,
                          Weights(),
                          nmults,
                          nknots,
                          npoles,
                          myRational ? &nweights : BSplCLib::NoWeights());
    myPoles = std::move(npoles);
    if (IsRational())
      myWeights = std::move(nweights);
    else
      myWeights = BSplCLib::UnitWeights(myPoles.Length());
    myMults         = std::move(nmults);
    myKnots         = std::move(nknots);
    myPeriodic      = false;
    myMaxDerivInvOk = false;
    updateKnots();
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::SetPole(const int Index, const gp_Pnt2d& P)
{
  if (Index < 1 || Index > myPoles.Length())
    throw Standard_OutOfRange("BSpline curve: SetPole: index and #pole mismatch");
  myPoles.SetValue(Index, P);
  myMaxDerivInvOk = false;
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
  if (Index < 1 || Index > myPoles.Length())
    throw Standard_OutOfRange("BSpline curve: SetWeight: Index and #pole mismatch");

  if (W <= gp::Resolution())
    throw Standard_ConstructionError("BSpline curve: SetWeight: Weight too small");

  bool rat = IsRational() || (std::abs(W - 1.) > gp::Resolution());

  if (rat)
  {
    // Becoming rational from non-rational: copy non-owning view to owned array.
    if (!IsRational())
    {
      myWeights = NCollection_Array1<double>(myWeights);
    }

    myWeights.SetValue(Index, W);

    if (IsRational())
    {
      rat = Rational(myWeights);
      if (!rat)
        myWeights = BSplCLib::UnitWeights(myPoles.Length());
    }

    myRational = rat;
  }

  myMaxDerivInvOk = false;
}

//=================================================================================================

void Geom2d_BSplineCurve::MovePoint(const double    U,
                                    const gp_Pnt2d& P,
                                    const int       Index1,
                                    const int       Index2,
                                    int&            FirstModifiedPole,
                                    int&            LastmodifiedPole)
{
  if (Index1 < 1 || Index1 > myPoles.Length() || Index2 < 1 || Index2 > myPoles.Length()
      || Index1 > Index2)
  {
    throw Standard_OutOfRange("BSpline curve: MovePoint: Index and #pole mismatch");
  }
  NCollection_Array1<gp_Pnt2d> npoles(1, myPoles.Length());
  gp_Pnt2d                     P0;
  Geom2d_Curve::D0(U, P0);
  gp_Vec2d Displ(P0, P);

  BSplCLib::MovePoint(U,
                      Displ,
                      Index1,
                      Index2,
                      myDeg,
                      myPoles,
                      Weights(),
                      myFlatKnots,
                      FirstModifiedPole,
                      LastmodifiedPole,
                      npoles);
  if (FirstModifiedPole)
  {
    myPoles         = std::move(npoles);
    myMaxDerivInvOk = false;
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
  NCollection_Array1<gp_Pnt2d> new_poles(1, myPoles.Length());
  gp_Pnt2d                     P0;

  gp_Vec2d delta_derivative;
  Geom2d_Curve::D1(U, P0, delta_derivative);
  gp_Vec2d delta(P0, P);
  for (ii = 1; ii <= 2; ii++)
  {
    delta_derivative.SetCoord(ii, Tangent.Coord(ii) - delta_derivative.Coord(ii));
  }

  BSplCLib::MovePointAndTangent(U,
                                delta,
                                delta_derivative,
                                Tolerance,
                                myDeg,
                                StartingCondition,
                                EndingCondition,
                                myPoles,
                                Weights(),
                                myFlatKnots,
                                new_poles,
                                ErrorStatus);
  if (!ErrorStatus)
  {
    myPoles         = std::move(new_poles);
    myMaxDerivInvOk = false;
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::updateKnots()
{
  myMaxDerivInvOk = false;

  int MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(myDeg, myPeriodic, myKnots, myMults, myKnotSet, MaxKnotMult);

  if (myKnotSet == GeomAbs_Uniform && !myPeriodic)
  {
    myFlatKnots.Resize(myKnots.Lower(), myKnots.Upper(), false);
    myFlatKnots.Assign(myKnots);
  }
  else
  {
    myFlatKnots.Resize(1, BSplCLib::KnotSequenceLength(myMults, myDeg, myPeriodic), false);

    BSplCLib::KnotSequence(myKnots, myMults, myDeg, myPeriodic, myFlatKnots);
  }

  if (MaxKnotMult == 0)
    mySmooth = GeomAbs_CN;
  else
  {
    switch (myDeg - MaxKnotMult)
    {
      case 0:
        mySmooth = GeomAbs_C0;
        break;
      case 1:
        mySmooth = GeomAbs_C1;
        break;
      case 2:
        mySmooth = GeomAbs_C2;
        break;
      case 3:
        mySmooth = GeomAbs_C3;
        break;
      default:
        mySmooth = GeomAbs_C3;
        break;
    }
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::PeriodicNormalization(double& Parameter) const
{
  double Period;

  if (myPeriodic)
  {
    Period = myFlatKnots.Value(myFlatKnots.Upper() - myDeg) - myFlatKnots.Value(myDeg + 1);
    while (Parameter > myFlatKnots.Value(myFlatKnots.Upper() - myDeg))
    {
      Parameter -= Period;
    }
    while (Parameter < myFlatKnots.Value((myDeg + 1)))
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

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myRational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPeriodic)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myKnotSet)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, mySmooth)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDeg)
  if (myPoles.Size() > 0)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.Size())

  if (myRational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myWeights.Size())
  if (myFlatKnots.Size() > 0)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myFlatKnots.Size())
  if (myKnots.Size() > 0)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myKnots.Size())
  if (myMults.Size() > 0)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMults.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxDerivInv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myMaxDerivInvOk)
}
