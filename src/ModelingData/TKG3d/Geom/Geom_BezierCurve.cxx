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

#define No_Standard_OutOfRange
#define No_Standard_DimensionError

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
    : myData(theOther.myData),
      rational(theOther.rational),
      closed(theOther.closed),
      maxderivinv(theOther.maxderivinv),
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

  NCollection_Array1<gp_Pnt> npoles(1, Deg + 1);
  NCollection_Array1<double> nweights(1, Deg + 1);

  if (IsRational())
  {
    BSplCLib::IncreaseDegree(Degree(),
                             Deg,
                             false,
                             myData.Poles,
                             &myData.Weights,
                             myData.Knots,
                             myData.Mults,
                             npoles,
                             &nweights,
                             myData.Knots,
                             myData.Mults);
    Init(npoles, &nweights);
  }
  else
  {
    BSplCLib::IncreaseDegree(Degree(),
                             Deg,
                             false,
                             myData.Poles,
                             BSplCLib::NoWeights(),
                             myData.Knots,
                             myData.Mults,
                             npoles,
                             BSplCLib::NoWeights(),
                             myData.Knots,
                             myData.Mults);
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
    npoles(i) = myData.Poles(i);

  npoles(Index + 1) = P;

  for (i = Index + 1; i <= nbpoles; i++)
    npoles(i + 1) = myData.Poles(i);

  // Insert the weight
  bool rat = IsRational() || std::abs(Weight - 1.) > gp::Resolution();

  if (rat)
  {
    NCollection_Array1<double> nweights(1, nbpoles + 1);

    for (i = 1; i <= Index; i++)
      if (IsRational())
        nweights(i) = myData.Weights(i);
      else
        nweights(i) = 1.;

    nweights(Index + 1) = Weight;

    for (i = Index + 1; i <= nbpoles; i++)
      if (IsRational())
        nweights(i + 1) = myData.Weights(i);
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
    npoles(i) = myData.Poles(i);

  for (i = Index + 1; i <= nbpoles; i++)
    npoles(i - 1) = myData.Poles(i);

  // Remove the weight
  if (IsRational())
  {
    NCollection_Array1<double> nweights(1, nbpoles - 1);

    for (i = 1; i < Index; i++)
      nweights(i) = myData.Weights(i);

    for (i = Index + 1; i <= nbpoles; i++)
      nweights(i - 1) = myData.Weights(i);

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
    P                             = myData.Poles(i);
    myData.Poles(i)               = myData.Poles(nbpoles - i + 1);
    myData.Poles(nbpoles - i + 1) = P;
  }

  // reverse weights
  if (IsRational())
  {
    double w;
    for (i = 1; i <= nbpoles / 2; i++)
    {
      w                               = myData.Weights(i);
      myData.Weights(i)               = myData.Weights(nbpoles - i + 1);
      myData.Weights(nbpoles - i + 1) = w;
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

  NCollection_Array1<gp_Pnt> coeffs(1, myData.Poles.Size());
  if (IsRational())
  {
    NCollection_Array1<double> wcoeffs(1, myData.Poles.Size());
    BSplCLib::BuildCache(0.0,
                         1.0,
                         false,
                         Degree(),
                         myData.FlatKnots,
                         myData.Poles,
                         &myData.Weights,
                         coeffs,
                         &wcoeffs);
    PLib::Trimming(U1, U2, coeffs, &wcoeffs);
    PLib::CoefficientsPoles(coeffs, &wcoeffs, myData.Poles, &myData.Weights);
  }
  else
  {
    BSplCLib::BuildCache(0.0,
                         1.0,
                         false,
                         Degree(),
                         myData.FlatKnots,
                         myData.Poles,
                         BSplCLib::NoWeights(),
                         coeffs,
                         BSplCLib::NoWeights());
    PLib::Trimming(U1, U2, coeffs, PLib::NoWeights());
    PLib::CoefficientsPoles(coeffs, PLib::NoWeights(), myData.Poles, PLib::NoWeights());
  }
}

//=================================================================================================

void Geom_BezierCurve::SetPole(const int Index, const gp_Pnt& P)
{
  if (Index < 1 || Index > NbPoles())
    throw Standard_OutOfRange("Geom_BezierCurve::SetPole");

  myData.Poles(Index) = P;

  if (Index == 1 || Index == myData.Poles.Length())
  {
    closed = (myData.Poles(1).Distance(myData.Poles(NbPoles())) <= Precision::Confusion());
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
    myData.Weights.Resize(1, nbpoles, false);
    myData.Weights.Init(1.);
  }

  myData.Weights(Index) = Weight;

  // is it turning into non rational
  if (wasrat && !Rational(myData.Weights))
    rational = false;
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
  return myData.Degree;
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

  if (IsRational())
    BSplCLib::DN(U,
                 N,
                 0,
                 Degree(),
                 false,
                 myData.Poles,
                 &myData.Weights,
                 myData.Knots,
                 &myData.Mults,
                 V);
  else
    BSplCLib::DN(U,
                 N,
                 0,
                 Degree(),
                 false,
                 myData.Poles,
                 BSplCLib::NoWeights(),
                 myData.Knots,
                 &myData.Mults,
                 V);
  return V;
}

//=================================================================================================

gp_Pnt Geom_BezierCurve::StartPoint() const
{
  return myData.Poles(1);
}

//=================================================================================================

gp_Pnt Geom_BezierCurve::EndPoint() const
{
  return myData.Poles(myData.Poles.Upper());
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
  return myData.Poles.Length();
}

//=================================================================================================

const gp_Pnt& Geom_BezierCurve::Pole(const int Index) const
{
  if (Index < 1 || Index > myData.Poles.Length())
    throw Standard_OutOfRange("Geom_BezierCurve::Pole");
  return myData.Poles(Index);
}

//=================================================================================================

void Geom_BezierCurve::Poles(NCollection_Array1<gp_Pnt>& P) const
{
  if (P.Length() != myData.Poles.Length())
    throw Standard_DimensionError("Geom_BezierCurve::Poles");
  P = myData.Poles;
}

//=================================================================================================

const NCollection_Array1<gp_Pnt>& Geom_BezierCurve::Poles() const
{
  return myData.Poles;
}

//=================================================================================================

double Geom_BezierCurve::Weight(const int Index) const
{
  if (Index < 1 || Index > myData.Poles.Length())
    throw Standard_OutOfRange("Geom_BezierCurve::Weight");
  if (IsRational())
    return myData.Weights(Index);
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
    W = myData.Weights;
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
    myData.Poles(i).Transform(T);
}

//=================================================================================================

void Geom_BezierCurve::Resolution(const double Tolerance3D, double& UTolerance)
{
  if (!maxderivinvok)
  {
    if (IsRational())
    {
      BSplCLib::Resolution(myData.Poles,
                           &myData.Weights,
                           myData.Poles.Length(),
                           myData.FlatKnots,
                           Degree(),
                           1.,
                           maxderivinv);
    }
    else
    {
      BSplCLib::Resolution(myData.Poles,
                           BSplCLib::NoWeights(),
                           myData.Poles.Length(),
                           myData.FlatKnots,
                           Degree(),
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

void Geom_BezierCurve::updateKnots()
{
  static const double THE_BEZIER_KNOTS[2]     = {0.0, 1.0};
  static const int    THE_BEZIER_MULTS[26][2] = {
    {1, 1},   {2, 2},   {3, 3},   {4, 4},   {5, 5},   {6, 6},   {7, 7},   {8, 8},   {9, 9},
    {10, 10}, {11, 11}, {12, 12}, {13, 13}, {14, 14}, {15, 15}, {16, 16}, {17, 17}, {18, 18},
    {19, 19}, {20, 20}, {21, 21}, {22, 22}, {23, 23}, {24, 24}, {25, 25}, {26, 26}};

  const int aDeg = myData.Degree;
  // Non-owning wrappers around static data — zero allocation
  myData.Knots     = NCollection_Array1<double>(THE_BEZIER_KNOTS[0], 1, 2);
  myData.Mults     = NCollection_Array1<int>(THE_BEZIER_MULTS[aDeg][0], 1, 2);
  myData.FlatKnots = NCollection_Array1<double>(BSplCLib::FlatBezierKnots(aDeg), 1, 2 * (aDeg + 1));
  myData.IsPeriodic = false;
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
  myData.Poles.Resize(1, nbpoles, false);
  myData.Poles  = thePoles;
  myData.Degree = nbpoles - 1;

  if (rational)
  {
    myData.Weights.Resize(1, nbpoles, false);
    myData.Weights = *theWeights;
  }

  updateKnots();
  maxderivinvok = false;
}

//=================================================================================================

void Geom_BezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, rational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, closed)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Poles.Size())

  if (rational)
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myData.Weights.Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
