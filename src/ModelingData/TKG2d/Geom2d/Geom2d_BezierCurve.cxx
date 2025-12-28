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

#define No_Standard_OutOfRange
#define No_Standard_DimensionError

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
  // copy the poles

  occ::handle<NCollection_HArray1<gp_Pnt2d>> npoles =
    new NCollection_HArray1<gp_Pnt2d>(1, Poles.Length());

  npoles->ChangeArray1() = Poles;

  // Init non rational
  Init(npoles, occ::handle<NCollection_HArray1<double>>());
}

//=================================================================================================

Geom2d_BezierCurve::Geom2d_BezierCurve(const NCollection_Array1<gp_Pnt2d>& Poles,
                                       const NCollection_Array1<double>&   Weights)

{
  // copy the poles

  occ::handle<NCollection_HArray1<gp_Pnt2d>> npoles =
    new NCollection_HArray1<gp_Pnt2d>(1, Poles.Length());

  npoles->ChangeArray1() = Poles;

  // check the weights

  int nbpoles = Poles.Length();

  if (Weights.Length() != nbpoles)
    throw Standard_ConstructionError();

  int i;
  for (i = 1; i <= nbpoles; i++)
  {
    if (Weights(i) <= gp::Resolution())
    {
      throw Standard_ConstructionError();
    }
  }

  // check really rational
  bool rat = Rational(Weights);

  // copy the weights
  occ::handle<NCollection_HArray1<double>> nweights;
  if (rat)
  {
    nweights                 = new NCollection_HArray1<double>(1, nbpoles);
    nweights->ChangeArray1() = Weights;
  }

  // Init
  Init(npoles, nweights);
}

//=================================================================================================

Geom2d_BezierCurve::Geom2d_BezierCurve(const Geom2d_BezierCurve& theOther)
    : rational(theOther.rational),
      closed(theOther.closed),
      maxderivinv(theOther.maxderivinv),
      maxderivinvok(false)
{
  // Deep copy all data arrays without validation
  poles = new NCollection_HArray1<gp_Pnt2d>(theOther.poles->Lower(), theOther.poles->Upper());
  poles->ChangeArray1() = theOther.poles->Array1();

  if (!theOther.weights.IsNull())
  {
    weights = new NCollection_HArray1<double>(theOther.weights->Lower(), theOther.weights->Upper());
    weights->ChangeArray1() = theOther.weights->Array1();
  }
}

//=================================================================================================

void Geom2d_BezierCurve::Increase(const int Deg)
{
  if (Deg == Degree())
    return;

  Standard_ConstructionError_Raise_if(Deg < Degree() || Deg > Geom2d_BezierCurve::MaxDegree(),
                                      "Geom2d_BezierCurve::Increase");

  occ::handle<NCollection_HArray1<gp_Pnt2d>> npoles = new NCollection_HArray1<gp_Pnt2d>(1, Deg + 1);

  occ::handle<NCollection_HArray1<double>> nweights;

  NCollection_Array1<double> bidknots(1, 2);
  bidknots(1) = 0.;
  bidknots(2) = 1.;
  NCollection_Array1<int> bidmults(1, 2);
  bidmults.Init(Degree() + 1);

  if (IsRational())
  {
    nweights = new NCollection_HArray1<double>(1, Deg + 1);
    BSplCLib::IncreaseDegree(Degree(),
                             Deg,
                             false,
                             poles->Array1(),
                             &weights->Array1(),
                             bidknots,
                             bidmults,
                             npoles->ChangeArray1(),
                             &nweights->ChangeArray1(),
                             bidknots,
                             bidmults);
  }
  else
  {
    BSplCLib::IncreaseDegree(Degree(),
                             Deg,
                             false,
                             poles->Array1(),
                             BSplCLib::NoWeights(),
                             bidknots,
                             bidmults,
                             npoles->ChangeArray1(),
                             BSplCLib::NoWeights(),
                             bidknots,
                             bidmults);
  }

  Init(npoles, nweights);
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
  occ::handle<NCollection_HArray1<gp_Pnt2d>> npoles =
    new NCollection_HArray1<gp_Pnt2d>(1, nbpoles + 1);

  NCollection_Array1<gp_Pnt2d>&       newpoles = npoles->ChangeArray1();
  const NCollection_Array1<gp_Pnt2d>& oldpoles = poles->Array1();

  for (i = 1; i <= Index; i++)
    newpoles(i) = oldpoles(i);

  newpoles(Index + 1) = P;

  for (i = Index + 1; i <= nbpoles; i++)
    newpoles(i + 1) = oldpoles(i);

  // Insert the weight
  occ::handle<NCollection_HArray1<double>> nweights;
  bool rat = IsRational() || std::abs(Weight - 1.) > gp::Resolution();

  if (rat)
  {
    nweights                               = new NCollection_HArray1<double>(1, nbpoles + 1);
    NCollection_Array1<double>& newweights = nweights->ChangeArray1();

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

  Init(npoles, nweights);
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
  occ::handle<NCollection_HArray1<gp_Pnt2d>> npoles =
    new NCollection_HArray1<gp_Pnt2d>(1, nbpoles - 1);

  NCollection_Array1<gp_Pnt2d>&       newpoles = npoles->ChangeArray1();
  const NCollection_Array1<gp_Pnt2d>& oldpoles = poles->Array1();

  for (i = 1; i < Index; i++)
    newpoles(i) = oldpoles(i);

  for (i = Index + 1; i <= nbpoles; i++)
    newpoles(i - 1) = oldpoles(i);

  // Remove the weight
  occ::handle<NCollection_HArray1<double>> nweights;

  if (IsRational())
  {
    nweights                                     = new NCollection_HArray1<double>(1, nbpoles - 1);
    NCollection_Array1<double>&       newweights = nweights->ChangeArray1();
    const NCollection_Array1<double>& oldweights = weights->Array1();

    for (i = 1; i < Index; i++)
      newweights(i) = oldweights(i);

    for (i = Index + 1; i <= nbpoles; i++)
      newweights(i - 1) = oldweights(i);
  }

  Init(npoles, nweights);
}

//=================================================================================================

void Geom2d_BezierCurve::Reverse()
{
  gp_Pnt2d                      P;
  int                           i, nbpoles = NbPoles();
  NCollection_Array1<gp_Pnt2d>& cpoles = poles->ChangeArray1();

  // reverse poles
  for (i = 1; i <= nbpoles / 2; i++)
  {
    P                       = cpoles(i);
    cpoles(i)               = cpoles(nbpoles - i + 1);
    cpoles(nbpoles - i + 1) = P;
  }

  // reverse weights
  if (IsRational())
  {
    NCollection_Array1<double>& cweights = weights->ChangeArray1();
    double                      w;
    for (i = 1; i <= nbpoles / 2; i++)
    {
      w                         = cweights(i);
      cweights(i)               = cweights(nbpoles - i + 1);
      cweights(nbpoles - i + 1) = w;
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
  //
  //   WARNING: when calling trimming be careful that the cache
  //   is computed regarding 0.0e0 and not 1.0e0
  //
  NCollection_Array1<double>   bidflatknots(BSplCLib::FlatBezierKnots(Degree()),
                                          1,
                                          2 * (Degree() + 1));
  NCollection_Array1<gp_Pnt2d> coeffs(1, poles->Size());
  if (IsRational())
  {
    NCollection_Array1<double> wcoeffs(1, poles->Size());
    BSplCLib::BuildCache(0.0,
                         1.0,
                         false,
                         Degree(),
                         bidflatknots,
                         poles->Array1(),
                         &weights->Array1(),
                         coeffs,
                         &wcoeffs);
    PLib::Trimming(U1, U2, coeffs, &wcoeffs);
    PLib::CoefficientsPoles(coeffs, &wcoeffs, poles->ChangeArray1(), &weights->ChangeArray1());
  }
  else
  {
    BSplCLib::BuildCache(0.0,
                         1.0,
                         false,
                         Degree(),
                         bidflatknots,
                         poles->Array1(),
                         BSplCLib::NoWeights(),
                         coeffs,
                         BSplCLib::NoWeights());
    PLib::Trimming(U1, U2, coeffs, PLib::NoWeights());
    PLib::CoefficientsPoles(coeffs, PLib::NoWeights(), poles->ChangeArray1(), PLib::NoWeights());
  }
}

//=================================================================================================

void Geom2d_BezierCurve::SetPole(const int Index, const gp_Pnt2d& P)
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > NbPoles(), "Geom2d_BezierCurve::SetPole");

  NCollection_Array1<gp_Pnt2d>& cpoles = poles->ChangeArray1();
  cpoles(Index)                        = P;

  if (Index == 1 || Index == cpoles.Length())
  {
    closed = (cpoles(1).Distance(cpoles(NbPoles())) <= gp::Resolution());
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
    weights = new NCollection_HArray1<double>(1, nbpoles);
    weights->Init(1.);
  }

  NCollection_Array1<double>& cweights = weights->ChangeArray1();
  cweights(Index)                      = Weight;

  // is it turning into non rational
  if (wasrat && !Rational(cweights))
    weights.Nullify();
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
  return !weights.IsNull();
}

//=================================================================================================

GeomAbs_Shape Geom2d_BezierCurve::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

int Geom2d_BezierCurve::Degree() const
{
  return poles->Length() - 1;
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

  NCollection_Array1<double> bidknots(1, 2);
  bidknots(1) = 0.;
  bidknots(2) = 1.;
  NCollection_Array1<int> bidmults(1, 2);
  bidmults.Init(Degree() + 1);

  if (IsRational())
    BSplCLib::DN(U,
                 N,
                 0,
                 Degree(),
                 false,
                 poles->Array1(),
                 &weights->Array1(),
                 bidknots,
                 &bidmults,
                 V);
  else
    BSplCLib::DN(U,
                 N,
                 0,
                 Degree(),
                 false,
                 poles->Array1(),
                 BSplCLib::NoWeights(),
                 bidknots,
                 &bidmults,
                 V);
  return V;
}

//=================================================================================================

gp_Pnt2d Geom2d_BezierCurve::EndPoint() const
{
  return poles->Value(poles->Upper());
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
  return poles->Length();
}

//=================================================================================================

const gp_Pnt2d& Geom2d_BezierCurve::Pole(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > poles->Length(), "Geom2d_BezierCurve::Pole");
  return poles->Value(Index);
}

//=================================================================================================

void Geom2d_BezierCurve::Poles(NCollection_Array1<gp_Pnt2d>& P) const
{
  Standard_DimensionError_Raise_if(P.Length() != poles->Length(), "Geom2d_BezierCurve::Poles");
  P = poles->Array1();
}

//=================================================================================================

gp_Pnt2d Geom2d_BezierCurve::StartPoint() const
{
  return poles->Value(1);
}

//=================================================================================================

double Geom2d_BezierCurve::Weight(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > weights->Length(),
                               "Geom2d_BezierCurve::Weight");
  if (IsRational())
    return weights->Value(Index);
  else
    return 1.;
}

//=================================================================================================

void Geom2d_BezierCurve::Weights(NCollection_Array1<double>& W) const
{

  int nbpoles = NbPoles();
  Standard_DimensionError_Raise_if(W.Length() != nbpoles, "Geom2d_BezierCurve::Weights");
  if (IsRational())
    W = weights->Array1();
  else
  {
    int i;
    for (i = 1; i <= nbpoles; i++)
      W(i) = 1.;
  }
}

//=================================================================================================

void Geom2d_BezierCurve::Transform(const gp_Trsf2d& T)
{
  int                           nbpoles = NbPoles();
  NCollection_Array1<gp_Pnt2d>& cpoles  = poles->ChangeArray1();

  for (int i = 1; i <= nbpoles; i++)
    cpoles(i).Transform(T);
}

//=================================================================================================

void Geom2d_BezierCurve::Resolution(const double ToleranceUV, double& UTolerance)
{
  if (!maxderivinvok)
  {
    NCollection_Array1<double> bidflatknots(1, 2 * (Degree() + 1));
    for (int i = 1; i <= Degree() + 1; i++)
    {
      bidflatknots(i)                = 0.;
      bidflatknots(i + Degree() + 1) = 1.;
    }

    if (IsRational())
    {
      BSplCLib::Resolution(poles->Array1(),
                           &weights->Array1(),
                           poles->Length(),
                           bidflatknots,
                           Degree(),
                           1.,
                           maxderivinv);
    }
    else
    {
      BSplCLib::Resolution(poles->Array1(),
                           BSplCLib::NoWeights(),
                           poles->Length(),
                           bidflatknots,
                           Degree(),
                           1.,
                           maxderivinv);
    }
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

void Geom2d_BezierCurve::Init(const occ::handle<NCollection_HArray1<gp_Pnt2d>>& Poles,
                              const occ::handle<NCollection_HArray1<double>>&   Weights)
{
  int nbpoles = Poles->Length();
  // closed ?
  const NCollection_Array1<gp_Pnt2d>& cpoles = Poles->Array1();
  closed = cpoles(1).Distance(cpoles(nbpoles)) <= gp::Resolution();

  // rational
  rational = !Weights.IsNull();

  // set fields
  poles = Poles;
  if (rational)
    weights = Weights;
  else
    weights.Nullify();
}

//=================================================================================================

void Geom2d_BezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom2d_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, rational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, closed)
  if (!poles.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, poles->Size())
  if (!weights.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, weights->Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
