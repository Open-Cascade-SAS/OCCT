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

// Passage en classe persistante - 23/01/91
// Modif suite a la deuxieme revue de projet toolkit Geometry -23/01/91
// Infos :
// Actuellement pour les champs de la courbe le tableau des poles est
// declare de 1 a NbPoles et le tableau des poids est declare de 1 a NbPoles

// Revised RLE  Aug 19 1993
// Suppressed Swaps, added Init, removed typedefs



#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Geometry.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <array>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_BezierCurve, Geom2d_BoundedCurve)

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

Geom2d_BezierCurve::Geom2d_BezierCurve(const NCollection_Array1<gp_Pnt2d>& Poles)
{
  int nbpoles = Poles.Length();
  if (nbpoles < 2 || nbpoles > (Geom2d_BezierCurve::MaxDegree() + 1))
    throw Standard_ConstructionError();

  // Init non rational
  Init(Poles, nullptr);
}

//=================================================================================================

Geom2d_BezierCurve::Geom2d_BezierCurve(const NCollection_Array1<gp_Pnt2d>& Poles,
                                       const NCollection_Array1<double>&   Weights)
{
  int nbpoles = Poles.Length();
  if (nbpoles < 2 || nbpoles > (Geom2d_BezierCurve::MaxDegree() + 1))
    throw Standard_ConstructionError();

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

Geom2d_BezierCurve::Geom2d_BezierCurve(const Geom2d_BezierCurve& theOther)
    : myPoles(theOther.myPoles),
      myWeights(theOther.myWeights),
      rational(theOther.rational),
      closed(theOther.closed),
      maxderivinvok(false)
{
}

//=================================================================================================

void Geom2d_BezierCurve::Increase(const int Deg)
{
  if (Deg == Degree())
    return;

  Standard_ConstructionError_Raise_if(Deg < Degree() || Deg > Geom2d_BezierCurve::MaxDegree(),
                                      "Geom2d_BezierCurve::Increase");

  NCollection_Array1<gp_Pnt2d> npoles(1, Deg + 1);
  double                       aKnotsBuf[2];
  int                          aMultsBuf[2];
  NCollection_Array1<double>   nknots(aKnotsBuf[0], 1, 2);
  NCollection_Array1<int>      nmults(aMultsBuf[0], 1, 2);

  if (IsRational())
  {
    NCollection_Array1<double> nweights(1, Deg + 1);
    BSplCLib::IncreaseDegree(Degree(),
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
    BSplCLib::IncreaseDegree(Degree(),
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

int Geom2d_BezierCurve::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

void Geom2d_BezierCurve::InsertPoleAfter(const int Index, const gp_Pnt2d& P, const double Weight)
{
  int nbpoles = NbPoles();

  Standard_ConstructionError_Raise_if(nbpoles >= Geom2d_BezierCurve::MaxDegree()
                                        || Weight <= gp::Resolution(),
                                      "Geom2d_BezierCurve::InsertPoleAfter");

  Standard_OutOfRange_Raise_if(Index < 0 || Index > nbpoles, "Geom2d_BezierCurve::InsertPoleAfter");

  int i;

  // Insert the pole
  NCollection_Array1<gp_Pnt2d> npoles(1, nbpoles + 1);

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

void Geom2d_BezierCurve::InsertPoleBefore(const int Index, const gp_Pnt2d& P, const double Weight)
{
  InsertPoleAfter(Index - 1, P, Weight);
}

//=================================================================================================

void Geom2d_BezierCurve::RemovePole(const int Index)
{
  int nbpoles = NbPoles();

  Standard_ConstructionError_Raise_if(nbpoles <= 2, "Geom2d_BezierCurve::RemovePole");

  Standard_OutOfRange_Raise_if(Index < 1 || Index > nbpoles, "Geom2d_BezierCurve::RemovePole");

  int i;

  // Remove the pole
  NCollection_Array1<gp_Pnt2d> npoles(1, nbpoles - 1);

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

void Geom2d_BezierCurve::Reverse()
{
  gp_Pnt2d P;
  int      i, nbpoles = NbPoles();

  // reverse poles
  for (i = 1; i <= nbpoles / 2; i++)
  {
    P                                   = myPoles(i);
    myPoles(i)                     = myPoles(nbpoles - i + 1);
    myPoles(nbpoles - i + 1)       = P;
  }

  // reverse weights
  if (IsRational())
  {
    double w;
    for (i = 1; i <= nbpoles / 2; i++)
    {
      w                                   = myWeights(i);
      myWeights(i)                   = myWeights(nbpoles - i + 1);
      myWeights(nbpoles - i + 1)     = w;
    }
  }
}

//=================================================================================================

double Geom2d_BezierCurve::ReversedParameter(const double U) const
{
  return (1. - U);
}

//=================================================================================================

void Geom2d_BezierCurve::Segment(const double U1, const double U2)
{
  closed = (std::abs(Value(U1).Distance(Value(U2))) <= gp::Resolution());

  NCollection_Array1<gp_Pnt2d> coeffs(1, myPoles.Size());
  if (IsRational())
  {
    NCollection_Array1<double> wcoeffs(1, myPoles.Size());
    BSplCLib::BuildCache(0.0,
                         1.0,
                         false,
                         Degree(),
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
                         Degree(),
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

void Geom2d_BezierCurve::SetPole(const int Index, const gp_Pnt2d& P)
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoles(), "Geom2d_BezierCurve::SetPole");

  myPoles(Index) = P;

  if (Index == 1 || Index == myPoles.Length())
  {
    closed = (myPoles(1).Distance(myPoles(NbPoles())) <= gp::Resolution());
  }
}

//=================================================================================================

void Geom2d_BezierCurve::SetPole(const int Index, const gp_Pnt2d& P, const double Weight)
{
  SetPole(Index, P);
  SetWeight(Index, Weight);
}

//=================================================================================================

void Geom2d_BezierCurve::SetWeight(const int Index, const double Weight)
{
  int nbpoles = NbPoles();

  Standard_OutOfRange_Raise_if(Index < 1 || Index > nbpoles, "Geom2d_BezierCurve::SetWeight");
  Standard_ConstructionError_Raise_if(Weight <= gp::Resolution(), "Geom2d_BezierCurve::SetWeight");

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

bool Geom2d_BezierCurve::IsClosed() const
{
  return closed;
}

//=================================================================================================

bool Geom2d_BezierCurve::IsCN(const int) const
{
  return true;
}

//=================================================================================================

bool Geom2d_BezierCurve::IsPeriodic() const
{
  return false;
}

//=================================================================================================

bool Geom2d_BezierCurve::IsRational() const
{
  return rational;
}

//=================================================================================================

GeomAbs_Shape Geom2d_BezierCurve::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

int Geom2d_BezierCurve::Degree() const
{
  return myPoles.Size() - 1;
}

//=================================================================================================

void Geom2d_BezierCurve::D0(const double U, gp_Pnt2d& P) const
{
  BSplCLib::D0(U, Poles(), Weights(), P);
}

//=================================================================================================

void Geom2d_BezierCurve::D1(const double U, gp_Pnt2d& P, gp_Vec2d& V1) const
{
  BSplCLib::D1(U, Poles(), Weights(), P, V1);
}

//=================================================================================================

void Geom2d_BezierCurve::D2(const double U, gp_Pnt2d& P, gp_Vec2d& V1, gp_Vec2d& V2) const
{
  BSplCLib::D2(U, Poles(), Weights(), P, V1, V2);
}

//=================================================================================================

void Geom2d_BezierCurve::D3(const double U,
                            gp_Pnt2d&    P,
                            gp_Vec2d&    V1,
                            gp_Vec2d&    V2,
                            gp_Vec2d&    V3) const
{
  BSplCLib::D3(U, Poles(), Weights(), P, V1, V2, V3);
}

//=================================================================================================

gp_Vec2d Geom2d_BezierCurve::DN(const double U, const int N) const
{
  Standard_RangeError_Raise_if(N < 1, "Geom2d_BezierCurve::DN");
  gp_Vec2d V;

  BSplCLib::DN(U,
               N,
               0,
               Degree(),
               false,
               myPoles,
               Weights(),
               BezierKnots(),
               &BezierMults(),
               V);
  return V;
}

//=================================================================================================

gp_Pnt2d Geom2d_BezierCurve::EndPoint() const
{
  return myPoles(myPoles.Upper());
}

//=================================================================================================

double Geom2d_BezierCurve::FirstParameter() const
{
  return 0.0;
}

//=================================================================================================

double Geom2d_BezierCurve::LastParameter() const
{
  return 1.0;
}

//=================================================================================================

int Geom2d_BezierCurve::NbPoles() const
{
  return myPoles.Length();
}

//=================================================================================================

const gp_Pnt2d& Geom2d_BezierCurve::Pole(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myPoles.Length(),
                               "Geom2d_BezierCurve::Pole");
  return myPoles(Index);
}

//=================================================================================================

void Geom2d_BezierCurve::Poles(NCollection_Array1<gp_Pnt2d>& P) const
{
  Standard_DimensionError_Raise_if(P.Length() != myPoles.Length(),
                                   "Geom2d_BezierCurve::Poles");
  P = myPoles;
}

//=================================================================================================

gp_Pnt2d Geom2d_BezierCurve::StartPoint() const
{
  return myPoles(1);
}

//=================================================================================================

double Geom2d_BezierCurve::Weight(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > myPoles.Length(),
                               "Geom2d_BezierCurve::Weight");
  if (IsRational())
    return myWeights(Index);
  else
    return 1.;
}

//=================================================================================================

void Geom2d_BezierCurve::Weights(NCollection_Array1<double>& W) const
{

  int nbpoles = NbPoles();
  Standard_DimensionError_Raise_if(W.Length() != nbpoles, "Geom2d_BezierCurve::Weights");
  if (IsRational())
    W = myWeights;
  else
  {
    for (int i = 1; i <= nbpoles; i++)
      W(i) = 1.;
  }
}

//=================================================================================================

void Geom2d_BezierCurve::Transform(const gp_Trsf2d& T)
{
  int nbpoles = NbPoles();

  for (int i = 1; i <= nbpoles; i++)
    myPoles(i).Transform(T);
}

//=================================================================================================

void Geom2d_BezierCurve::Resolution(const double ToleranceUV, double& UTolerance)
{
  if (!maxderivinvok)
  {
    BSplCLib::Resolution(myPoles,
                         Weights(),
                         myPoles.Length(),
                         BezierFlatKnots(),
                         Degree(),
                         1.,
                         maxderivinv);
    maxderivinvok = true;
  }
  UTolerance = ToleranceUV * maxderivinv;
}

//=================================================================================================

occ::handle<Geom2d_Geometry> Geom2d_BezierCurve::Copy() const
{
  return new Geom2d_BezierCurve(*this);
}

//=================================================================================================

void Geom2d_BezierCurve::Init(const NCollection_Array1<gp_Pnt2d>& thePoles,
                              const NCollection_Array1<double>*   theWeights)
{
  int nbpoles = thePoles.Length();
  // closed ?
  closed = thePoles(thePoles.Lower()).Distance(thePoles(thePoles.Upper())) <= gp::Resolution();

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

void Geom2d_BezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, rational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, closed)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myPoles.Size())

  if (rational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myWeights.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}

//=================================================================================================

const NCollection_Array1<double>& Geom2d_BezierCurve::BezierKnots() const
{
  static const double THE_DATA[2] = {0.0, 1.0};
  static const NCollection_Array1<double> THE_KNOTS(THE_DATA[0], 1, 2);
  return THE_KNOTS;
}

//=================================================================================================

const NCollection_Array1<int>& Geom2d_BezierCurve::BezierMults() const
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

const NCollection_Array1<double>& Geom2d_BezierCurve::BezierFlatKnots() const
{
  static const auto THE_FKNOTS = []() {
    std::array<NCollection_Array1<double>, 26> anArr;
    for (int i = 1; i <= BSplCLib::MaxDegree(); ++i)
      anArr[i] = NCollection_Array1<double>(BSplCLib::FlatBezierKnots(i), 1, 2 * (i + 1));
    return anArr;
  }();
  return THE_FKNOTS[myPoles.Size() - 1];
}
