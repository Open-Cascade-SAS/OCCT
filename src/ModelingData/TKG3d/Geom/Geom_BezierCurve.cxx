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

// Passage en classe persistante - 23/01/91
// Modif suite a la deuxieme revue de projet toolkit Geometry -23/01/91
// Infos :
// Actuellement pour les champs de la courbe le tableau des poles est
// declare de 1 a NbPoles et le tableau des poids est declare de 1 a NbPoles

// Revised RLE  Aug 19 1993
// Suppressed Swaps, added Init, removed typedefs



#include <Geom_BezierCurve.hxx>
#include <Geom_Geometry.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

#include <array>

IMPLEMENT_STANDARD_RTTIEXT(Geom_BezierCurve, Geom_BoundedCurve)

//=======================================================================
// function : Rational
// purpose  : check rationality of an array of weights
//=======================================================================
static bool Rational(const NCollection_Array1<double>& W)
{
  int  i, n = W.Length();
  bool rat = false;
  for (i = 1; i < n; i++)
  {
    rat = std::abs(W(i) - W(i + 1)) > gp::Resolution();
    if (rat)
      break;
  }
  return rat;
}

//=================================================================================================

Geom_BezierCurve::Geom_BezierCurve(const Geom_BezierCurve& theOther)
    : myPoles(theOther.myPoles),
      myWeights(theOther.myWeights),
      rational(theOther.rational),
      closed(theOther.closed),
      maxderivinvok(false)
{
}

//=================================================================================================

Geom_BezierCurve::Geom_BezierCurve(const NCollection_Array1<gp_Pnt>& Poles)
{
  int nbpoles = Poles.Length();
  if (nbpoles < 2 || nbpoles > (Geom_BezierCurve::MaxDegree() + 1))
    throw Standard_ConstructionError();

  // Init non rational
  Init(Poles, nullptr);
}

//=================================================================================================

Geom_BezierCurve::Geom_BezierCurve(const NCollection_Array1<gp_Pnt>& Poles,
                                   const NCollection_Array1<double>& Weights)
{
  // copy the poles
  int nbpoles = Poles.Length();
  if (nbpoles < 2 || nbpoles > (Geom_BezierCurve::MaxDegree() + 1))
    throw Standard_ConstructionError();

  // check the weights
  if (Weights.Length() != nbpoles)
    throw Standard_ConstructionError();

  for (int i = 1; i <= nbpoles; i++)
  {
    if (Weights(i) <= gp::Resolution())
    {
      throw Standard_ConstructionError();
    }
  }

  // check really rational
  if (Rational(Weights))
  {
    Init(Poles, &Weights);
  }
  else
  {
    Init(Poles, nullptr);
  }
}

//=================================================================================================

void Geom_BezierCurve::Increase(const int Deg)
{
  if (Deg == Degree())
    return;

  if (Deg < Degree() || Deg > Geom_BezierCurve::MaxDegree())
    throw Standard_ConstructionError("Geom_BezierCurve::Increase");

  const int aDeg = myPoles.Size() - 1;

  NCollection_Array1<gp_Pnt> npoles(1, Deg + 1);
  double                     aKnotsBuf[2];
  int                        aMultsBuf[2];
  NCollection_Array1<double> nknots(aKnotsBuf[0], 1, 2);
  NCollection_Array1<int>    nmults(aMultsBuf[0], 1, 2);

  if (IsRational())
  {
    NCollection_Array1<double> nweights(1, Deg + 1);
    BSplCLib::IncreaseDegree(aDeg,
                             Deg,
                             false,
                             myPoles,
                             &myWeights,
                             BezierKnots(),
                             BezierMults(),
                             npoles,
                             &nweights,
                             nknots,
                             nmults);
    Init(npoles, &nweights);
  }
  else
  {
    BSplCLib::IncreaseDegree(aDeg,
                             Deg,
                             false,
                             myPoles,
                             BSplCLib::NoWeights(),
                             BezierKnots(),
                             BezierMults(),
                             npoles,
                             BSplCLib::NoWeights(),
                             nknots,
                             nmults);
    Init(npoles, nullptr);
  }
}

//=================================================================================================

int Geom_BezierCurve::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

void Geom_BezierCurve::InsertPoleAfter(const int Index, const gp_Pnt& P)
{
  InsertPoleAfter(Index, P, 1.);
}

//=================================================================================================

void Geom_BezierCurve::InsertPoleAfter(const int Index, const gp_Pnt& P, const double Weight)
{
  int nbpoles = NbPoles();

  if (nbpoles >= Geom_BezierCurve::MaxDegree() || Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierCurve::InsertPoleAfter");

  if (Index < 0 || Index > nbpoles)
    throw Standard_OutOfRange("Geom_BezierCurve::InsertPoleAfter");

  int i;

  // Insert the pole
  NCollection_Array1<gp_Pnt> npoles(1, nbpoles + 1);

  for (i = 1; i <= Index; i++)
    npoles(i) = myPoles(i);

  npoles(Index + 1) = P;

  for (i = Index + 1; i <= nbpoles; i++)
    npoles(i + 1) = myPoles(i);

  // Insert the weight
  bool rat = IsRational() || std::abs(Weight - 1.) > gp::Resolution();

  if (rat)
  {
    NCollection_Array1<double> nweights(1, nbpoles + 1);

    for (i = 1; i <= Index; i++)
      if (IsRational())
        nweights(i) = myWeights(i);
      else
        nweights(i) = 1.;

    nweights(Index + 1) = Weight;

    for (i = Index + 1; i <= nbpoles; i++)
      if (IsRational())
        nweights(i + 1) = myWeights(i);
      else
        nweights(i + 1) = 1.;

    Init(npoles, &nweights);
  }
  else
  {
    Init(npoles, nullptr);
  }
}

//=================================================================================================

void Geom_BezierCurve::InsertPoleBefore(const int Index, const gp_Pnt& P)
{
  InsertPoleAfter(Index - 1, P);
}

//=================================================================================================

void Geom_BezierCurve::InsertPoleBefore(const int Index, const gp_Pnt& P, const double Weight)
{
  InsertPoleAfter(Index - 1, P, Weight);
}

//=================================================================================================

void Geom_BezierCurve::RemovePole(const int Index)
{
  int nbpoles = NbPoles();

  if (nbpoles <= 2)
    throw Standard_ConstructionError("Geom_BezierCurve::RemovePole");

  if (Index < 1 || Index > nbpoles)
    throw Standard_OutOfRange("Geom_BezierCurve::RemovePole");

  int i;

  // Remove the pole
  NCollection_Array1<gp_Pnt> npoles(1, nbpoles - 1);

  for (i = 1; i < Index; i++)
    npoles(i) = myPoles(i);

  for (i = Index + 1; i <= nbpoles; i++)
    npoles(i - 1) = myPoles(i);

  // Remove the weight
  if (IsRational())
  {
    NCollection_Array1<double> nweights(1, nbpoles - 1);

    for (i = 1; i < Index; i++)
      nweights(i) = myWeights(i);

    for (i = Index + 1; i <= nbpoles; i++)
      nweights(i - 1) = myWeights(i);

    Init(npoles, &nweights);
  }
  else
  {
    Init(npoles, nullptr);
  }
}

//=================================================================================================

void Geom_BezierCurve::Reverse()
{
  gp_Pnt P;
  int    i, nbpoles = NbPoles();

  // reverse poles
  for (i = 1; i <= nbpoles / 2; i++)
  {
    P                            = myPoles(i);
    myPoles(i)               = myPoles(nbpoles - i + 1);
    myPoles(nbpoles - i + 1) = P;
  }

  // reverse weights
  if (IsRational())
  {
    double w;
    for (i = 1; i <= nbpoles / 2; i++)
    {
      w                              = myWeights(i);
      myWeights(i)               = myWeights(nbpoles - i + 1);
      myWeights(nbpoles - i + 1) = w;
    }
  }
}

//=================================================================================================

double Geom_BezierCurve::ReversedParameter(const double U) const
{
  return (1. - U);
}

//=================================================================================================

void Geom_BezierCurve::Segment(const double U1, const double U2)
{
  closed = (std::abs(Value(U1).Distance(Value(U2))) <= Precision::Confusion());

  const int aDeg = myPoles.Size() - 1;

  NCollection_Array1<gp_Pnt> coeffs(1, myPoles.Size());
  if (IsRational())
  {
    NCollection_Array1<double> wcoeffs(1, myPoles.Size());
    BSplCLib::BuildCache(0.0,
                         1.0,
                         false,
                         aDeg,
                         BezierFlatKnots(),
                         myPoles,
                         &myWeights,
                         coeffs,
                         &wcoeffs);
    PLib::Trimming(U1, U2, coeffs, &wcoeffs);
    PLib::CoefficientsPoles(coeffs, &wcoeffs, myPoles, &myWeights);
  }
  else
  {
    BSplCLib::BuildCache(0.0,
                         1.0,
                         false,
                         aDeg,
                         BezierFlatKnots(),
                         myPoles,
                         BSplCLib::NoWeights(),
                         coeffs,
                         BSplCLib::NoWeights());
    PLib::Trimming(U1, U2, coeffs, PLib::NoWeights());
    PLib::CoefficientsPoles(coeffs, PLib::NoWeights(), myPoles, PLib::NoWeights());
  }
}

//=================================================================================================

void Geom_BezierCurve::SetPole(const int Index, const gp_Pnt& P)
{
  if (Index < 1 || Index > NbPoles())
    throw Standard_OutOfRange("Geom_BezierCurve::SetPole");

  myPoles(Index) = P;

  if (Index == 1 || Index == myPoles.Length())
  {
    closed = (myPoles(1).Distance(myPoles(NbPoles())) <= Precision::Confusion());
  }
}

//=================================================================================================

void Geom_BezierCurve::SetPole(const int Index, const gp_Pnt& P, const double Weight)
{
  SetPole(Index, P);
  SetWeight(Index, Weight);
}

//=================================================================================================

void Geom_BezierCurve::SetWeight(const int Index, const double Weight)
{
  int nbpoles = NbPoles();

  if (Index < 1 || Index > nbpoles)
    throw Standard_OutOfRange("Geom_BezierCurve::SetWeight");
  if (Weight <= gp::Resolution())
    throw Standard_ConstructionError("Geom_BezierCurve::SetWeight");

  // compute new rationality
  bool wasrat = IsRational();
  if (!wasrat)
  {
    // a weight of 1. does not turn to rational
    if (std::abs(Weight - 1.) <= gp::Resolution())
      return;

    // set weights of 1.
    myWeights.Resize(1, nbpoles, false);
    myWeights.Init(1.);
  }

  myWeights(Index) = Weight;

  // is it turning into non rational
  if (wasrat && !Rational(myWeights))
  {
    rational = false;
    myWeights = NCollection_Array1<double>();
  }
  else
    rational = true;
}

//=================================================================================================

bool Geom_BezierCurve::IsClosed() const
{
  return closed;
}

//=================================================================================================

bool Geom_BezierCurve::IsCN(const int) const
{
  return true;
}

//=================================================================================================

bool Geom_BezierCurve::IsPeriodic() const
{
  return false;
}

//=================================================================================================

bool Geom_BezierCurve::IsRational() const
{
  return rational;
}

//=================================================================================================

GeomAbs_Shape Geom_BezierCurve::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

int Geom_BezierCurve::Degree() const
{
  return myPoles.Size() - 1;
}

//=================================================================================================

void Geom_BezierCurve::D0(const double U, gp_Pnt& P) const
{
  BSplCLib::D0(U, Poles(), Weights(), P);
}

//=================================================================================================

void Geom_BezierCurve::D1(const double U, gp_Pnt& P, gp_Vec& V1) const
{
  BSplCLib::D1(U, Poles(), Weights(), P, V1);
}

//=================================================================================================

void Geom_BezierCurve::D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{
  BSplCLib::D2(U, Poles(), Weights(), P, V1, V2);
}

//=================================================================================================

void Geom_BezierCurve::D3(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const
{
  BSplCLib::D3(U, Poles(), Weights(), P, V1, V2, V3);
}

//=================================================================================================

gp_Vec Geom_BezierCurve::DN(const double U, const int N) const
{
  if (N < 1)
    throw Standard_RangeError("Geom_BezierCurve::DN");
  gp_Vec V;

  const int aDeg = myPoles.Size() - 1;

  if (IsRational())
    BSplCLib::DN(U,
                 N,
                 0,
                 aDeg,
                 false,
                 myPoles,
                 &myWeights,
                 BezierKnots(),
                 &BezierMults(),
                 V);
  else
    BSplCLib::DN(U,
                 N,
                 0,
                 aDeg,
                 false,
                 myPoles,
                 BSplCLib::NoWeights(),
                 BezierKnots(),
                 &BezierMults(),
                 V);
  return V;
}

//=================================================================================================

gp_Pnt Geom_BezierCurve::StartPoint() const
{
  return myPoles(1);
}

//=================================================================================================

gp_Pnt Geom_BezierCurve::EndPoint() const
{
  return myPoles(myPoles.Upper());
}

//=================================================================================================

double Geom_BezierCurve::FirstParameter() const
{
  return 0.0;
}

//=================================================================================================

double Geom_BezierCurve::LastParameter() const
{
  return 1.0;
}

//=================================================================================================

int Geom_BezierCurve::NbPoles() const
{
  return myPoles.Length();
}

//=================================================================================================

const gp_Pnt& Geom_BezierCurve::Pole(const int Index) const
{
  if (Index < 1 || Index > myPoles.Length())
    throw Standard_OutOfRange("Geom_BezierCurve::Pole");
  return myPoles(Index);
}

//=================================================================================================

void Geom_BezierCurve::Poles(NCollection_Array1<gp_Pnt>& P) const
{
  if (P.Length() != myPoles.Length())
    throw Standard_DimensionError("Geom_BezierCurve::Poles");
  P = myPoles;
}

//=================================================================================================

const NCollection_Array1<gp_Pnt>& Geom_BezierCurve::Poles() const
{
  return myPoles;
}

//=================================================================================================

double Geom_BezierCurve::Weight(const int Index) const
{
  if (Index < 1 || Index > myPoles.Length())
    throw Standard_OutOfRange("Geom_BezierCurve::Weight");
  if (IsRational())
    return myWeights(Index);
  else
    return 1.;
}

//=================================================================================================

void Geom_BezierCurve::Weights(NCollection_Array1<double>& W) const
{

  int nbpoles = NbPoles();
  if (W.Length() != nbpoles)
    throw Standard_DimensionError("Geom_BezierCurve::Weights");
  if (IsRational())
    W = myWeights;
  else
  {
    for (int i = 1; i <= nbpoles; i++)
      W(i) = 1.;
  }
}

//=================================================================================================

void Geom_BezierCurve::Transform(const gp_Trsf& T)
{
  int nbpoles = NbPoles();

  for (int i = 1; i <= nbpoles; i++)
    myPoles(i).Transform(T);
}

//=================================================================================================

void Geom_BezierCurve::Resolution(const double Tolerance3D, double& UTolerance)
{
  if (!maxderivinvok)
  {
    const int aDeg = myPoles.Size() - 1;
    if (IsRational())
    {
      BSplCLib::Resolution(myPoles,
                           &myWeights,
                           myPoles.Length(),
                           BezierFlatKnots(),
                           aDeg,
                           1.,
                           maxderivinv);
    }
    else
    {
      BSplCLib::Resolution(myPoles,
                           BSplCLib::NoWeights(),
                           myPoles.Length(),
                           BezierFlatKnots(),
                           aDeg,
                           1.,
                           maxderivinv);
    }
    maxderivinvok = true;
  }
  UTolerance = Tolerance3D * maxderivinv;
}

//=================================================================================================

occ::handle<Geom_Geometry> Geom_BezierCurve::Copy() const
{
  return new Geom_BezierCurve(*this);
}

//=================================================================================================

void Geom_BezierCurve::Init(const NCollection_Array1<gp_Pnt>& thePoles,
                            const NCollection_Array1<double>* theWeights)
{
  int nbpoles = thePoles.Length();
  // closed ?
  closed =
    thePoles(thePoles.Lower()).Distance(thePoles(thePoles.Upper())) <= Precision::Confusion();

  // rational
  rational = (theWeights != nullptr);

  // set fields
  myPoles.Resize(1, nbpoles, false);
  myPoles = thePoles;

  if (rational)
  {
    myWeights.Resize(1, nbpoles, false);
    myWeights = *theWeights;
  }
  else
  {
    myWeights = NCollection_Array1<double>();
  }

  maxderivinv   = 0.0;
  maxderivinvok = false;
}

//=================================================================================================

void Geom_BezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, rational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, closed)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.Size())

  if (rational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myWeights.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}

//=================================================================================================

const NCollection_Array1<double>& Geom_BezierCurve::BezierKnots() const
{
  static const double THE_DATA[2] = {0.0, 1.0};
  static const NCollection_Array1<double> THE_KNOTS(THE_DATA[0], 1, 2);
  return THE_KNOTS;
}

//=================================================================================================

const NCollection_Array1<int>& Geom_BezierCurve::BezierMults() const
{
  static const int THE_DATA[26][2] = {
    {1, 1},   {2, 2},   {3, 3},   {4, 4},   {5, 5},   {6, 6},   {7, 7},
    {8, 8},   {9, 9},   {10, 10}, {11, 11}, {12, 12}, {13, 13}, {14, 14},
    {15, 15}, {16, 16}, {17, 17}, {18, 18}, {19, 19}, {20, 20}, {21, 21},
    {22, 22}, {23, 23}, {24, 24}, {25, 25}, {26, 26}};
  static const auto THE_MULTS = []() {
    std::array<NCollection_Array1<int>, 26> anArr;
    for (int i = 0; i < 26; ++i)
      anArr[i] = NCollection_Array1<int>(THE_DATA[i][0], 1, 2);
    return anArr;
  }();
  return THE_MULTS[myPoles.Size() - 1];
}

//=================================================================================================

const NCollection_Array1<double>& Geom_BezierCurve::BezierFlatKnots() const
{
  static const auto THE_FKNOTS = []() {
    std::array<NCollection_Array1<double>, 26> anArr;
    for (int i = 1; i <= BSplCLib::MaxDegree(); ++i)
      anArr[i] = NCollection_Array1<double>(BSplCLib::FlatBezierKnots(i), 1, 2 * (i + 1));
    return anArr;
  }();
  return THE_FKNOTS[myPoles.Size() - 1];
}
