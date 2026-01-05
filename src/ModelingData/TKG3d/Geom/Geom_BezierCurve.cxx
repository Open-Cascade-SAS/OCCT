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
#include <BSplCLib_Cache.hxx>
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
    : rational(theOther.rational),
      closed(theOther.closed),
      maxderivinv(theOther.maxderivinv),
      maxderivinvok(false)
{
  // Deep copy all data arrays without validation
  poles = new NCollection_HArray1<gp_Pnt>(theOther.poles->Lower(), theOther.poles->Upper());
  poles->ChangeArray1() = theOther.poles->Array1();

  if (!theOther.weights.IsNull())
  {
    weights = new NCollection_HArray1<double>(theOther.weights->Lower(), theOther.weights->Upper());
    weights->ChangeArray1() = theOther.weights->Array1();
  }
}

//=================================================================================================

Geom_BezierCurve::Geom_BezierCurve(const NCollection_Array1<gp_Pnt>& Poles)
{
  int nbpoles = Poles.Length();
  if (nbpoles < 2 || nbpoles > (Geom_BezierCurve::MaxDegree() + 1))
    throw Standard_ConstructionError();
  //  copy the poles
  occ::handle<NCollection_HArray1<gp_Pnt>> npoles = new NCollection_HArray1<gp_Pnt>(1, nbpoles);

  npoles->ChangeArray1() = Poles;

  // Init non rational
  Init(npoles, occ::handle<NCollection_HArray1<double>>());
}

//=================================================================================================

Geom_BezierCurve::Geom_BezierCurve(const NCollection_Array1<gp_Pnt>& Poles,
                                   const NCollection_Array1<double>& Weights)
{
  // copy the poles
  int nbpoles = Poles.Length();
  if (nbpoles < 2 || nbpoles > (Geom_BezierCurve::MaxDegree() + 1))
    throw Standard_ConstructionError();

  occ::handle<NCollection_HArray1<gp_Pnt>> npoles = new NCollection_HArray1<gp_Pnt>(1, nbpoles);

  npoles->ChangeArray1() = Poles;

  // check the weights

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

void Geom_BezierCurve::Increase(const int Deg)
{
  if (Deg == Degree())
    return;

  if (Deg < Degree() || Deg > Geom_BezierCurve::MaxDegree())
    throw Standard_ConstructionError("Geom_BezierCurve::Increase");

  occ::handle<NCollection_HArray1<gp_Pnt>> npoles = new NCollection_HArray1<gp_Pnt>(1, Deg + 1);

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
  occ::handle<NCollection_HArray1<gp_Pnt>> npoles = new NCollection_HArray1<gp_Pnt>(1, nbpoles + 1);

  NCollection_Array1<gp_Pnt>&       newpoles = npoles->ChangeArray1();
  const NCollection_Array1<gp_Pnt>& oldpoles = poles->Array1();

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
  occ::handle<NCollection_HArray1<gp_Pnt>> npoles = new NCollection_HArray1<gp_Pnt>(1, nbpoles - 1);

  NCollection_Array1<gp_Pnt>&       newpoles = npoles->ChangeArray1();
  const NCollection_Array1<gp_Pnt>& oldpoles = poles->Array1();

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

void Geom_BezierCurve::Reverse()
{
  gp_Pnt                      P;
  int                         i, nbpoles = NbPoles();
  NCollection_Array1<gp_Pnt>& cpoles = poles->ChangeArray1();

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

  invalidateCache();
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

  NCollection_Array1<double>  bidflatknots(BSplCLib::FlatBezierKnots(Degree()),
                                          1,
                                          2 * (Degree() + 1));
  NCollection_HArray1<gp_Pnt> coeffs(1, poles->Size());
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

  invalidateCache();
}

//=================================================================================================

void Geom_BezierCurve::SetPole(const int Index, const gp_Pnt& P)
{
  if (Index < 1 || Index > NbPoles())
    throw Standard_OutOfRange("Geom_BezierCurve::SetPole");

  NCollection_Array1<gp_Pnt>& cpoles = poles->ChangeArray1();
  cpoles(Index)                      = P;

  if (Index == 1 || Index == cpoles.Length())
  {
    closed = (cpoles(1).Distance(cpoles(NbPoles())) <= Precision::Confusion());
  }

  invalidateCache();
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
    weights = new NCollection_HArray1<double>(1, nbpoles);
    weights->Init(1.);
  }

  NCollection_Array1<double>& cweights = weights->ChangeArray1();
  cweights(Index)                      = Weight;

  // is it turning into non rational
  if (wasrat && !Rational(cweights))
    weights.Nullify();

  invalidateCache();
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
  return !weights.IsNull();
}

//=================================================================================================

GeomAbs_Shape Geom_BezierCurve::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

int Geom_BezierCurve::Degree() const
{
  return poles->Length() - 1;
}

//=================================================================================================

BSplCLib_Cache& Geom_BezierCurve::ensureCache() const
{
  if (!myCache)
  {
    NCollection_Array1<double> aFlatKnots(BSplCLib::FlatBezierKnots(Degree()),
                                          1,
                                          2 * (Degree() + 1));
    myCache = new BSplCLib_Cache(Degree(), IsPeriodic(), aFlatKnots, poles->Array1(), Weights());
  }
  return *myCache;
}

//=================================================================================================

void Geom_BezierCurve::invalidateCache() const
{
  myCache = nullptr;
}

//=================================================================================================

void Geom_BezierCurve::D0(const double U, gp_Pnt& P) const
{
  BSplCLib_Cache& aCache = ensureCache();
  aCache.D0(U, P);
}

//=================================================================================================

void Geom_BezierCurve::D1(const double U, gp_Pnt& P, gp_Vec& V1) const
{
  BSplCLib_Cache& aCache = ensureCache();
  aCache.D1(U, P, V1);
}

//=================================================================================================

void Geom_BezierCurve::D2(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2) const
{
  BSplCLib_Cache& aCache = ensureCache();
  aCache.D2(U, P, V1, V2);
}

//=================================================================================================

void Geom_BezierCurve::D3(const double U, gp_Pnt& P, gp_Vec& V1, gp_Vec& V2, gp_Vec& V3) const
{
  BSplCLib_Cache& aCache = ensureCache();
  aCache.D3(U, P, V1, V2, V3);
}

//=================================================================================================

gp_Vec Geom_BezierCurve::DN(const double U, const int N) const
{
  if (N < 1)
    throw Standard_RangeError("Geom_BezierCurve::DN");
  gp_Vec V;

  NCollection_Array1<double> bidknots(1, 2);
  bidknots(1) = 0.;
  bidknots(2) = 1.;
  NCollection_Array1<int> bidmults(1, 2);
  bidmults.Init(Degree() + 1);

  if (IsRational())
    //    BSplCLib::DN(U,N,0,Degree(),0.,
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
    //    BSplCLib::DN(U,N,0,Degree(),0.,
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

gp_Pnt Geom_BezierCurve::StartPoint() const
{
  return poles->Value(1);
}

//=================================================================================================

gp_Pnt Geom_BezierCurve::EndPoint() const
{
  return poles->Value(poles->Upper());
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
  return poles->Length();
}

//=================================================================================================

const gp_Pnt& Geom_BezierCurve::Pole(const int Index) const
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange("Geom_BezierCurve::Pole");
  return poles->Value(Index);
}

//=================================================================================================

void Geom_BezierCurve::Poles(NCollection_Array1<gp_Pnt>& P) const
{
  if (P.Length() != poles->Length())
    throw Standard_DimensionError("Geom_BezierCurve::Poles");
  P = poles->Array1();
}

//=================================================================================================

const NCollection_Array1<gp_Pnt>& Geom_BezierCurve::Poles() const
{
  return poles->Array1();
}

//=================================================================================================

double Geom_BezierCurve::Weight(const int Index) const
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange("Geom_BezierCurve::Weight");
  if (IsRational())
    return weights->Value(Index);
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
    W = weights->Array1();
  else
  {
    int i;
    for (i = 1; i <= nbpoles; i++)
      W(i) = 1.;
  }
}

//=================================================================================================

void Geom_BezierCurve::Transform(const gp_Trsf& T)
{
  int                         nbpoles = NbPoles();
  NCollection_Array1<gp_Pnt>& cpoles  = poles->ChangeArray1();

  for (int i = 1; i <= nbpoles; i++)
    cpoles(i).Transform(T);

  invalidateCache();
}

//=================================================================================================

void Geom_BezierCurve::Resolution(const double Tolerance3D, double& UTolerance)
{
  if (!maxderivinvok)
  {
    NCollection_Array1<double> bidflatknots(BSplCLib::FlatBezierKnots(Degree()),
                                            1,
                                            2 * (Degree() + 1));

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
  UTolerance = Tolerance3D * maxderivinv;
}

//=================================================================================================

occ::handle<Geom_Geometry> Geom_BezierCurve::Copy() const
{
  return new Geom_BezierCurve(*this);
}

//=================================================================================================

void Geom_BezierCurve::Init(const occ::handle<NCollection_HArray1<gp_Pnt>>& Poles,
                            const occ::handle<NCollection_HArray1<double>>& Weights)
{
  int nbpoles = Poles->Length();
  // closed ?
  const NCollection_Array1<gp_Pnt>& cpoles = Poles->Array1();
  closed = cpoles(1).Distance(cpoles(nbpoles)) <= Precision::Confusion();

  // rational
  rational = !Weights.IsNull();

  // set fields
  poles = Poles;

  if (rational)
    weights = Weights;
  else
    weights.Nullify();

  // Invalidate cache since structure changed
  invalidateCache();
}

//=================================================================================================

void Geom_BezierCurve::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Geom_BoundedCurve)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, rational)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, closed)
  if (!poles.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, poles->Size())
  if (!weights.IsNull())
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, weights->Size())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinv)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, maxderivinvok)
}
