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

#define No_Standard_OutOfRange

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

//=======================================================================
// function : CheckCurveData
// purpose  : Internal use only
//=======================================================================
static void CheckCurveData(const TColgp_Array1OfPnt2d&    CPoles,
                           const TColStd_Array1OfReal&    CKnots,
                           const TColStd_Array1OfInteger& CMults,
                           const Standard_Integer         Degree,
                           const Standard_Boolean         Periodic)
{
  if (Degree < 1 || Degree > Geom2d_BSplineCurve::MaxDegree())
  {
    throw Standard_ConstructionError("BSpline curve: invalid degree");
  }

  if (CPoles.Length() < 2)
    throw Standard_ConstructionError("BSpline curve: at least 2 poles required");
  if (CKnots.Length() != CMults.Length())
    throw Standard_ConstructionError("BSpline curve: Knot and Mult array size mismatch");

  for (Standard_Integer I = CKnots.Lower(); I < CKnots.Upper(); I++)
  {
    if (CKnots(I + 1) - CKnots(I) <= Epsilon(Abs(CKnots(I))))
    {
      throw Standard_ConstructionError("BSpline curve: Knots interval values too close");
    }
  }

  if (CPoles.Length() != BSplCLib::NbPoles(Degree, Periodic, CMults))
    throw Standard_ConstructionError("BSpline curve: # Poles and degree mismatch");
}

//! Check rationality of an array of weights
static Standard_Boolean Rational(const TColStd_Array1OfReal& theWeights)
{
  for (Standard_Integer i = theWeights.Lower(); i < theWeights.Upper(); i++)
  {
    if (Abs(theWeights[i] - theWeights[i + 1]) > gp::Resolution())
    {
      return Standard_True;
    }
  }
  return Standard_False;
}

//=================================================================================================

Handle(Geom2d_Geometry) Geom2d_BSplineCurve::Copy() const
{
  Handle(Geom2d_BSplineCurve) C;
  if (IsRational())
    C = new Geom2d_BSplineCurve(poles->Array1(),
                                weights->Array1(),
                                knots->Array1(),
                                mults->Array1(),
                                deg,
                                periodic);
  else
    C = new Geom2d_BSplineCurve(poles->Array1(), knots->Array1(), mults->Array1(), deg, periodic);
  return C;
}

//=================================================================================================

Geom2d_BSplineCurve::Geom2d_BSplineCurve(const TColgp_Array1OfPnt2d&    Poles,
                                         const TColStd_Array1OfReal&    Knots,
                                         const TColStd_Array1OfInteger& Mults,
                                         const Standard_Integer         Degree,
                                         const Standard_Boolean         Periodic)
    : rational(Standard_False),
      periodic(Periodic),
      deg(Degree),
      maxderivinvok(Standard_False)
{
  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  // copy arrays

  poles                 = new TColgp_HArray1OfPnt2d(1, Poles.Length());
  poles->ChangeArray1() = Poles;

  knots                 = new TColStd_HArray1OfReal(1, Knots.Length());
  knots->ChangeArray1() = Knots;

  mults                 = new TColStd_HArray1OfInteger(1, Mults.Length());
  mults->ChangeArray1() = Mults;

  UpdateKnots();
}

//=================================================================================================

Geom2d_BSplineCurve::Geom2d_BSplineCurve(const TColgp_Array1OfPnt2d&    Poles,
                                         const TColStd_Array1OfReal&    Weights,
                                         const TColStd_Array1OfReal&    Knots,
                                         const TColStd_Array1OfInteger& Mults,
                                         const Standard_Integer         Degree,
                                         const Standard_Boolean         Periodic)
    : rational(Standard_True),
      periodic(Periodic),
      deg(Degree),
      maxderivinvok(Standard_False)

{

  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  if (Weights.Length() != Poles.Length())
    throw Standard_ConstructionError("Geom2d_BSplineCurve: Weights and Poles array size mismatch");

  Standard_Integer i;
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

  poles                 = new TColgp_HArray1OfPnt2d(1, Poles.Length());
  poles->ChangeArray1() = Poles;
  if (rational)
  {
    weights                 = new TColStd_HArray1OfReal(1, Weights.Length());
    weights->ChangeArray1() = Weights;
  }

  knots                 = new TColStd_HArray1OfReal(1, Knots.Length());
  knots->ChangeArray1() = Knots;

  mults                 = new TColStd_HArray1OfInteger(1, Mults.Length());
  mults->ChangeArray1() = Mults;

  UpdateKnots();
}

//=================================================================================================

Standard_Integer Geom2d_BSplineCurve::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

void Geom2d_BSplineCurve::IncreaseDegree(const Standard_Integer Degree)
{
  if (Degree == deg)
    return;

  if (Degree < deg || Degree > Geom2d_BSplineCurve::MaxDegree())
  {
    throw Standard_ConstructionError("BSpline curve: IncreaseDegree: bad degree value");
  }

  Standard_Integer FromK1 = FirstUKnotIndex();
  Standard_Integer ToK2   = LastUKnotIndex();

  Standard_Integer Step = Degree - deg;

  Handle(TColgp_HArray1OfPnt2d) npoles =
    new TColgp_HArray1OfPnt2d(1, poles->Length() + Step * (ToK2 - FromK1));

  Standard_Integer nbknots =
    BSplCLib::IncreaseDegreeCountKnots(deg, Degree, periodic, mults->Array1());

  Handle(TColStd_HArray1OfReal) nknots = new TColStd_HArray1OfReal(1, nbknots);

  Handle(TColStd_HArray1OfInteger) nmults = new TColStd_HArray1OfInteger(1, nbknots);

  Handle(TColStd_HArray1OfReal) nweights;

  if (IsRational())
  {
    nweights = new TColStd_HArray1OfReal(1, npoles->Upper());
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

void Geom2d_BSplineCurve::IncreaseMultiplicity(const Standard_Integer Index,
                                               const Standard_Integer M)
{
  TColStd_Array1OfReal k(1, 1);
  k(1) = knots->Value(Index);
  TColStd_Array1OfInteger m(1, 1);
  m(1) = M - mults->Value(Index);
  InsertKnots(k, m, Epsilon(1.), Standard_True);
}

//=================================================================================================

void Geom2d_BSplineCurve::IncreaseMultiplicity(const Standard_Integer I1,
                                               const Standard_Integer I2,
                                               const Standard_Integer M)
{
  Handle(TColStd_HArray1OfReal) tk = knots;
  TColStd_Array1OfReal          k((knots->Array1())(I1), I1, I2);
  TColStd_Array1OfInteger       m(I1, I2);
  Standard_Integer              i;
  for (i = I1; i <= I2; i++)
    m(i) = M - mults->Value(i);
  InsertKnots(k, m, Epsilon(1.), Standard_True);
}

//=================================================================================================

void Geom2d_BSplineCurve::IncrementMultiplicity(const Standard_Integer I1,
                                                const Standard_Integer I2,
                                                const Standard_Integer Step)
{
  Handle(TColStd_HArray1OfReal) tk = knots;
  TColStd_Array1OfReal          k((knots->Array1())(I1), I1, I2);
  TColStd_Array1OfInteger       m(I1, I2);
  m.Init(Step);
  InsertKnots(k, m, Epsilon(1.), Standard_True);
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertKnot(const Standard_Real    U,
                                     const Standard_Integer M,
                                     const Standard_Real    ParametricTolerance)
{
  TColStd_Array1OfReal k(1, 1);
  k(1) = U;
  TColStd_Array1OfInteger m(1, 1);
  m(1) = M;
  InsertKnots(k, m, ParametricTolerance);
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertKnots(const TColStd_Array1OfReal&    Knots,
                                      const TColStd_Array1OfInteger& Mults,
                                      const Standard_Real            Epsilon,
                                      const Standard_Boolean         Add)
{
  // Check and compute new sizes
  Standard_Integer nbpoles, nbknots;

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
    throw Standard_ConstructionError("Geom2d_BSplineCurve::InsertKnots");

  if (nbpoles == poles->Length())
    return;

  Handle(TColgp_HArray1OfPnt2d)    npoles = new TColgp_HArray1OfPnt2d(1, nbpoles);
  Handle(TColStd_HArray1OfReal)    nknots = knots;
  Handle(TColStd_HArray1OfInteger) nmults = mults;

  if (nbknots != knots->Length())
  {
    nknots = new TColStd_HArray1OfReal(1, nbknots);
    nmults = new TColStd_HArray1OfInteger(1, nbknots);
  }

  Handle(TColStd_HArray1OfReal) nweights;
  if (rational)
  {
    nweights = new TColStd_HArray1OfReal(1, nbpoles);
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

Standard_Boolean Geom2d_BSplineCurve::RemoveKnot(const Standard_Integer Index,
                                                 const Standard_Integer M,
                                                 const Standard_Real    Tolerance)
{
  if (M < 0)
    return Standard_True;

  Standard_Integer I1 = FirstUKnotIndex();
  Standard_Integer I2 = LastUKnotIndex();

  if (Index < I1 || Index > I2)
  {
    throw Standard_OutOfRange("BSpline curve: RemoveKnot: index out of range");
  }

  const TColgp_Array1OfPnt2d& oldpoles = poles->Array1();

  Standard_Integer step = mults->Value(Index) - M;
  if (step <= 0)
    return Standard_True;

  Handle(TColgp_HArray1OfPnt2d) npoles = new TColgp_HArray1OfPnt2d(1, oldpoles.Length() - step);

  Handle(TColStd_HArray1OfReal)    nknots = knots;
  Handle(TColStd_HArray1OfInteger) nmults = mults;

  if (M == 0)
  {
    nknots = new TColStd_HArray1OfReal(1, knots->Length() - 1);
    nmults = new TColStd_HArray1OfInteger(1, knots->Length() - 1);
  }

  Handle(TColStd_HArray1OfReal) nweights;
  if (IsRational())
  {
    nweights = new TColStd_HArray1OfReal(1, npoles->Length());
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
    return Standard_False;
  }

  weights = nweights;
  poles   = npoles;
  knots   = nknots;
  mults   = nmults;

  UpdateKnots();
  maxderivinvok = 0;
  return Standard_True;
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertPoleAfter(const Standard_Integer Index,
                                          const gp_Pnt2d&        P,
                                          const Standard_Real    Weight)
{
  if (Index < 0 || Index > poles->Length())
    throw Standard_OutOfRange("BSpline curve: InsertPoleAfter: Index and #pole mismatch");

  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("BSpline curve: InsertPoleAfter: Weight too small");

  if (knotSet == GeomAbs_NonUniform || knotSet == GeomAbs_PiecewiseBezier)
  {
    throw Standard_ConstructionError("BSpline curve: InsertPoleAfter: bad knotSet type");
  }

  const TColStd_Array1OfReal& cknots  = knots->Array1();
  Standard_Integer            nbknots = cknots.Length();

  Handle(TColStd_HArray1OfReal) nknots = new TColStd_HArray1OfReal(1, nbknots + 1);

  TColStd_Array1OfReal& newknots = nknots->ChangeArray1();

  Standard_Integer i;
  for (i = 1; i < nbknots; i++)
  {
    newknots(i) = cknots(i);
  }

  newknots(nbknots + 1) = 2 * newknots(nbknots) - newknots(nbknots - 1);

  Handle(TColStd_HArray1OfInteger) nmults = new TColStd_HArray1OfInteger(1, nbknots + 1);

  TColStd_Array1OfInteger&       newmults = nmults->ChangeArray1();
  const TColStd_Array1OfInteger& cmults   = mults->Array1();

  for (i = 2; i <= nbknots; i++)
    newmults(i) = 1;
  newmults(1)           = cmults(1);
  newmults(nbknots + 1) = cmults(nbknots + 1);

  const TColgp_Array1OfPnt2d&   cpoles   = poles->Array1();
  Standard_Integer              nbpoles  = cpoles.Length();
  Handle(TColgp_HArray1OfPnt2d) npoles   = new TColgp_HArray1OfPnt2d(1, nbpoles + 1);
  TColgp_Array1OfPnt2d&         newpoles = npoles->ChangeArray1();

  // insert the pole

  for (i = 1; i <= Index; i++)
    newpoles(i) = cpoles(i);

  newpoles(Index + 1) = P;

  for (i = Index + 1; i <= nbpoles; i++)
    newpoles(i + 1) = cpoles(i);

  // Insert the weight

  Handle(TColStd_HArray1OfReal) nweights;
  Standard_Boolean              rat = IsRational() || Abs(Weight - 1.) > gp::Resolution();

  if (rat)
  {
    nweights                         = new TColStd_HArray1OfReal(1, nbpoles + 1);
    TColStd_Array1OfReal& newweights = nweights->ChangeArray1();

    for (i = 1; i <= Index; i++)
      if (IsRational())
        newweights(i) = weights->Value(i);
      else
        newweights(i) = 1.;

    newweights(Index + 1) = Weight;

    for (i = Index + 1; i <= nbpoles; i++)
      if (IsRational())
        newweights(i + 1) = weights->Value(i);
      else
        newweights(i + 1) = 1.;
  }

  poles         = npoles;
  weights       = nweights;
  knots         = nknots;
  mults         = nmults;
  maxderivinvok = 0;
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::InsertPoleBefore(const Standard_Integer Index,
                                           const gp_Pnt2d&        P,
                                           const Standard_Real    Weight)
{
  InsertPoleAfter(Index - 1, P, Weight);
}

//=================================================================================================

void Geom2d_BSplineCurve::RemovePole(const Standard_Integer Index)
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange("BSpline curve: RemovePole: Index and #pole mismatch");

  if (poles->Length() <= 2)
    throw Standard_ConstructionError("BSpline curve: RemovePole: #pole is already minimum");

  if (knotSet == GeomAbs_NonUniform || knotSet == GeomAbs_PiecewiseBezier)
    throw Standard_ConstructionError("BSpline curve: RemovePole: bad knotSet type");

  Standard_Integer              i;
  Handle(TColStd_HArray1OfReal) nknots   = new TColStd_HArray1OfReal(1, knots->Length() - 1);
  TColStd_Array1OfReal&         newknots = nknots->ChangeArray1();

  Handle(TColStd_HArray1OfInteger) nmults   = new TColStd_HArray1OfInteger(1, mults->Length() - 1);
  TColStd_Array1OfInteger&         newmults = nmults->ChangeArray1();

  for (i = 1; i < newknots.Length(); i++)
  {
    newknots(i) = knots->Value(i);
    newmults(i) = 1;
  }
  newmults(1)                = mults->Value(1);
  newknots(newknots.Upper()) = knots->Value(knots->Upper());
  newmults(newmults.Upper()) = mults->Value(mults->Upper());

  Handle(TColgp_HArray1OfPnt2d) npoles   = new TColgp_HArray1OfPnt2d(1, poles->Upper() - 1);
  TColgp_Array1OfPnt2d&         newpoles = npoles->ChangeArray1();

  for (i = 1; i < Index; i++)
    newpoles(i) = poles->Value(i);
  for (i = Index; i < newpoles.Length(); i++)
    newpoles(i) = poles->Value(i + 1);

  Handle(TColStd_HArray1OfReal) nweights;
  if (IsRational())
  {
    nweights                         = new TColStd_HArray1OfReal(1, newpoles.Length());
    TColStd_Array1OfReal& newweights = nweights->ChangeArray1();
    for (i = 1; i < Index; i++)
      newweights(i) = weights->Value(i);
    for (i = Index; i < newweights.Length(); i++)
      newweights(i) = weights->Value(i + 1);
  }

  poles   = npoles;
  weights = nweights;
  knots   = nknots;
  mults   = nmults;
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::Reverse()
{
  BSplCLib::Reverse(knots->ChangeArray1());
  BSplCLib::Reverse(mults->ChangeArray1());
  Standard_Integer last;
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

Standard_Real Geom2d_BSplineCurve::ReversedParameter(const Standard_Real U) const
{
  return (FirstParameter() + LastParameter() - U);
}

//=================================================================================================

void Geom2d_BSplineCurve::Segment(const Standard_Real aU1,
                                  const Standard_Real aU2,
                                  const Standard_Real theTolerance)
{
  if (aU2 < aU1)
    throw Standard_DomainError("Geom2d_BSplineCurve::Segment");
  //
  Standard_Real    AbsUMax = Max(Abs(FirstParameter()), Abs(LastParameter()));
  Standard_Real    Eps     = Max(Epsilon(AbsUMax), theTolerance);
  Standard_Real    NewU1, NewU2;
  Standard_Real    U, DU = 0;
  Standard_Integer i, k, index;
  //
  // f
  Standard_Real U1 = aU1, U2 = aU2;
  //
  // Henceforward we use U1, U2 as bounds of the segment
  // t
  //
  TColStd_Array1OfReal    Knots(1, 2);
  TColStd_Array1OfInteger Mults(1, 2);
  //
  // define param distance to keep (eap, Apr 18 2002, occ311)
  if (periodic)
  {
    Standard_Real Period = LastParameter() - FirstParameter();
    DU                   = U2 - U1;
    if (DU - Period > Precision::PConfusion())
      throw Standard_DomainError("Geom2d_BSplineCurve::Segment");
    if (DU > Period)
      DU = Period;
  }
  //
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
  Knots(1) = Min(NewU1, NewU2);
  Knots(2) = Max(NewU1, NewU2);
  Mults(1) = Mults(2) = deg;
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
    // Eps = Epsilon(knots->Value(index+1));
    if (Abs(knots->Value(index + 1) - U) <= Eps)
    {
      index++;
    }
    SetOrigin(index);
    SetNotPeriodic();
    NewU2 = NewU1 + DU;
  }

  // compute index1 and index2 to set the new knots and mults
  Standard_Integer index1 = 0, index2 = 0;
  Standard_Integer FromU1 = knots->Lower();
  Standard_Integer ToU2   = knots->Upper();
  BSplCLib::LocateParameter(deg,
                            knots->Array1(),
                            mults->Array1(),
                            NewU1,
                            periodic,
                            FromU1,
                            ToU2,
                            index1,
                            U);
  if (Abs(knots->Value(index1 + 1) - U) <= Eps)
  {
    index1++;
  }
  BSplCLib::LocateParameter(deg,
                            knots->Array1(),
                            mults->Array1(),
                            NewU2,
                            periodic,
                            FromU1,
                            ToU2,
                            index2,
                            U);
  // Eps = Epsilon(knots->Value(index2+1));
  if (Abs(knots->Value(index2 + 1) - U) <= Eps || index2 == index1)
  {
    index2++;
  }

  Standard_Integer nbknots = index2 - index1 + 1;

  Handle(TColStd_HArray1OfReal)    nknots = new TColStd_HArray1OfReal(1, nbknots);
  Handle(TColStd_HArray1OfInteger) nmults = new TColStd_HArray1OfInteger(1, nbknots);

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
    nknots->SetValue(k, knots->Value(i) - DU);
    nmults->SetValue(k, mults->Value(i));
    k++;
  }
  nmults->SetValue(1, deg + 1);
  nmults->SetValue(nbknots, deg + 1);

  // compute index1 and index2 to set the new poles and weights
  Standard_Integer pindex1 = BSplCLib::PoleIndex(deg, index1, periodic, mults->Array1());
  Standard_Integer pindex2 = BSplCLib::PoleIndex(deg, index2, periodic, mults->Array1());

  pindex1++;
  pindex2 = Min(pindex2 + 1, poles->Length());

  Standard_Integer nbpoles = pindex2 - pindex1 + 1;

  Handle(TColStd_HArray1OfReal) nweights = new TColStd_HArray1OfReal(1, nbpoles);
  Handle(TColgp_HArray1OfPnt2d) npoles   = new TColgp_HArray1OfPnt2d(1, nbpoles);

  k = 1;
  if (rational)
  {
    nweights = new TColStd_HArray1OfReal(1, nbpoles);
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

  knots = nknots;
  mults = nmults;
  poles = npoles;
  if (rational)
  {
    weights = nweights;
  }
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetKnot(const Standard_Integer Index, const Standard_Real K)
{
  if (Index < 1 || Index > knots->Length())
    throw Standard_OutOfRange("BSpline curve: SetKnot: Index and #knots mismatch");
  Standard_Real DK = Abs(Epsilon(K));
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

void Geom2d_BSplineCurve::SetKnots(const TColStd_Array1OfReal& K)
{
  CheckCurveData(poles->Array1(), K, mults->Array1(), deg, periodic);
  knots->ChangeArray1() = K;
  maxderivinvok         = 0;
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetKnot(const Standard_Integer Index,
                                  const Standard_Real    K,
                                  const Standard_Integer M)
{
  IncreaseMultiplicity(Index, M);
  SetKnot(Index, K);
}

//=================================================================================================

void Geom2d_BSplineCurve::SetPeriodic()
{
  Standard_Integer first = FirstUKnotIndex();
  Standard_Integer last  = LastUKnotIndex();

  Handle(TColStd_HArray1OfReal) tk = knots;
  TColStd_Array1OfReal          cknots((knots->Array1())(first), first, last);
  knots                 = new TColStd_HArray1OfReal(1, cknots.Length());
  knots->ChangeArray1() = cknots;

  Handle(TColStd_HArray1OfInteger) tm = mults;
  TColStd_Array1OfInteger          cmults((mults->Array1())(first), first, last);
  cmults(first) = cmults(last) = Min(deg, Max(cmults(first), cmults(last)));
  mults                        = new TColStd_HArray1OfInteger(1, cmults.Length());
  mults->ChangeArray1()        = cmults;

  // compute new number of poles;
  Standard_Integer nbp = BSplCLib::NbPoles(deg, Standard_True, cmults);

  Handle(TColgp_HArray1OfPnt2d) tp = poles;
  TColgp_Array1OfPnt2d          cpoles((poles->Array1())(1), 1, nbp);
  poles                 = new TColgp_HArray1OfPnt2d(1, nbp);
  poles->ChangeArray1() = cpoles;

  if (rational)
  {
    Handle(TColStd_HArray1OfReal) tw = weights;
    TColStd_Array1OfReal          cweights((weights->Array1())(1), 1, nbp);
    weights                 = new TColStd_HArray1OfReal(1, nbp);
    weights->ChangeArray1() = cweights;
  }

  periodic = Standard_True;

  maxderivinvok = 0;
  UpdateKnots();
}

//=================================================================================================

void Geom2d_BSplineCurve::SetOrigin(const Standard_Integer Index)
{
  if (!periodic)
    throw Standard_NoSuchObject("Geom2d_BSplineCurve::SetOrigin");
  Standard_Integer i, k;
  Standard_Integer first = FirstUKnotIndex();
  Standard_Integer last  = LastUKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom2d_BSplineCurve::SetOrigin");

  Standard_Integer nbknots = knots->Length();
  Standard_Integer nbpoles = poles->Length();

  Handle(TColStd_HArray1OfReal) nknots   = new TColStd_HArray1OfReal(1, nbknots);
  TColStd_Array1OfReal&         newknots = nknots->ChangeArray1();

  Handle(TColStd_HArray1OfInteger) nmults   = new TColStd_HArray1OfInteger(1, nbknots);
  TColStd_Array1OfInteger&         newmults = nmults->ChangeArray1();

  // set the knots and mults
  Standard_Real period = knots->Value(last) - knots->Value(first);
  k                    = 1;
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

  Standard_Integer index = 1;
  for (i = first + 1; i <= Index; i++)
    index += mults->Value(i);

  // set the poles and weights
  Handle(TColgp_HArray1OfPnt2d) npoles     = new TColgp_HArray1OfPnt2d(1, nbpoles);
  Handle(TColStd_HArray1OfReal) nweights   = new TColStd_HArray1OfReal(1, nbpoles);
  TColgp_Array1OfPnt2d&         newpoles   = npoles->ChangeArray1();
  TColStd_Array1OfReal&         newweights = nweights->ChangeArray1();
  first                                    = poles->Lower();
  last                                     = poles->Upper();
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

void Geom2d_BSplineCurve::SetNotPeriodic()
{
  if (periodic)
  {
    Standard_Integer NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(deg, mults->Array1(), NbKnots, NbPoles);

    Handle(TColgp_HArray1OfPnt2d) npoles = new TColgp_HArray1OfPnt2d(1, NbPoles);

    Handle(TColStd_HArray1OfReal) nknots = new TColStd_HArray1OfReal(1, NbKnots);

    Handle(TColStd_HArray1OfInteger) nmults = new TColStd_HArray1OfInteger(1, NbKnots);

    Handle(TColStd_HArray1OfReal) nweights;
    if (IsRational())
    {
      nweights = new TColStd_HArray1OfReal(1, NbPoles);
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
    poles         = npoles;
    weights       = nweights;
    mults         = nmults;
    knots         = nknots;
    periodic      = Standard_False;
    maxderivinvok = 0;
    UpdateKnots();
  }
}

//=================================================================================================

void Geom2d_BSplineCurve::SetPole(const Standard_Integer Index, const gp_Pnt2d& P)
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange("BSpline curve: SetPole: index and #pole mismatch");
  poles->SetValue(Index, P);
  maxderivinvok = 0;
}

//=================================================================================================

void Geom2d_BSplineCurve::SetPole(const Standard_Integer Index,
                                  const gp_Pnt2d&        P,
                                  const Standard_Real    W)
{
  SetPole(Index, P);
  SetWeight(Index, W);
}

//=================================================================================================

void Geom2d_BSplineCurve::SetWeight(const Standard_Integer Index, const Standard_Real W)
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange("BSpline curve: SetWeight: Index and #pole mismatch");

  if (W <= gp::Resolution())
    throw Standard_ConstructionError("BSpline curve: SetWeight: Weight too small");

  Standard_Boolean rat = IsRational() || (Abs(W - 1.) > gp::Resolution());

  if (rat)
  {
    if (rat && !IsRational())
    {
      weights = new TColStd_HArray1OfReal(1, poles->Length());
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

void Geom2d_BSplineCurve::MovePoint(const Standard_Real    U,
                                    const gp_Pnt2d&        P,
                                    const Standard_Integer Index1,
                                    const Standard_Integer Index2,
                                    Standard_Integer&      FirstModifiedPole,
                                    Standard_Integer&      LastmodifiedPole)
{
  if (Index1 < 1 || Index1 > poles->Length() || Index2 < 1 || Index2 > poles->Length()
      || Index1 > Index2)
  {
    throw Standard_OutOfRange("BSpline curve: MovePoint: Index and #pole mismatch");
  }
  TColgp_Array1OfPnt2d npoles(1, poles->Length());
  gp_Pnt2d             P0;
  D0(U, P0);
  gp_Vec2d Displ(P0, P);

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

void Geom2d_BSplineCurve::MovePointAndTangent(const Standard_Real    U,
                                              const gp_Pnt2d&        P,
                                              const gp_Vec2d&        Tangent,
                                              const Standard_Real    Tolerance,
                                              const Standard_Integer StartingCondition,
                                              const Standard_Integer EndingCondition,
                                              Standard_Integer&      ErrorStatus)
{
  Standard_Integer ii;
  if (IsPeriodic())
  {
    //
    // for the time being do not deal with periodic curves
    //
    SetNotPeriodic();
  }
  TColgp_Array1OfPnt2d new_poles(1, poles->Length());
  gp_Pnt2d             P0;

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

void Geom2d_BSplineCurve::UpdateKnots()
{

  rational = !weights.IsNull();

  Standard_Integer MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(deg, periodic, knots->Array1(), mults->Array1(), knotSet, MaxKnotMult);

  if (knotSet == GeomAbs_Uniform && !periodic)
  {
    flatknots = knots;
  }
  else
  {
    flatknots =
      new TColStd_HArray1OfReal(1, BSplCLib::KnotSequenceLength(mults->Array1(), deg, periodic));

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

void Geom2d_BSplineCurve::PeriodicNormalization(Standard_Real& Parameter) const
{
  Standard_Real Period;

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

void Geom2d_BSplineCurve::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_BoundedCurve)

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
