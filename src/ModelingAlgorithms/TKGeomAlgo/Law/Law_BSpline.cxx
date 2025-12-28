// Created on: 1995-10-20
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

// Cut and past sauvage depuis Geom!?!?
// 14-Mar-96 : xab implemented MovePointAndTangent
// 03-02-97 : pmn ->LocateU sur Periodic (PRO6963),
//            bon appel a LocateParameter (PRO6973) et mise en conformite avec
//            le cdl de LocateU, lorsque U est un noeud (PRO6988)

#include <BSplCLib.hxx>
#include <BSplCLib_KnotDistribution.hxx>
#include <BSplCLib_MultDistribution.hxx>
#include <gp.hxx>
#include <Law_BSpline.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Law_BSpline, Standard_Transient)

#define POLES (poles->Array1())
#define KNOTS (knots->Array1())
#define FKNOTS (flatknots->Array1())
#define FMULTS (BSplCLib::NoMults())

//=================================================================================================

static void SetPoles(const NCollection_Array1<double>& Poles,
                     const NCollection_Array1<double>& Weights,
                     NCollection_Array1<double>&       FP)
{
  int i, j = FP.Lower();
  for (i = Poles.Lower(); i <= Poles.Upper(); i++)
  {
    double w = Weights(i);
    FP(j)           = Poles(i) * w;
    j++;
    FP(j) = w;
    j++;
  }
}

//=================================================================================================

static void GetPoles(const NCollection_Array1<double>& FP,
                     NCollection_Array1<double>&       Poles,
                     NCollection_Array1<double>&       Weights)

{
  int i, j = FP.Lower();
  for (i = Poles.Lower(); i <= Poles.Upper(); i++)
  {
    double w = FP(j + 1);
    Weights(i)      = w;
    Poles(i)        = FP(j) / w;
    j += 2;
  }
}

//=================================================================================================

static void CheckCurveData(const NCollection_Array1<double>&    CPoles,
                           const NCollection_Array1<double>&    CKnots,
                           const NCollection_Array1<int>& CMults,
                           const int         Degree,
                           const bool         Periodic)
{
  if (Degree < 1 || Degree > Law_BSpline::MaxDegree())
  {
    throw Standard_ConstructionError();
  }

  if (CPoles.Length() < 2)
    throw Standard_ConstructionError();
  if (CKnots.Length() != CMults.Length())
    throw Standard_ConstructionError();

  for (int I = CKnots.Lower(); I < CKnots.Upper(); I++)
  {
    if (CKnots(I + 1) - CKnots(I) <= Epsilon(std::abs(CKnots(I))))
    {
      throw Standard_ConstructionError();
    }
  }

  if (CPoles.Length() != BSplCLib::NbPoles(Degree, Periodic, CMults))
    throw Standard_ConstructionError();
}

//=================================================================================================

static void KnotAnalysis(const int         Degree,
                         const bool         Periodic,
                         const NCollection_Array1<double>&    CKnots,
                         const NCollection_Array1<int>& CMults,
                         GeomAbs_BSplKnotDistribution&  KnotForm,
                         int&              MaxKnotMult)
{
  KnotForm = GeomAbs_NonUniform;

  BSplCLib_KnotDistribution KSet = BSplCLib::KnotForm(CKnots, 1, CKnots.Length());

  if (KSet == BSplCLib_Uniform)
  {
    BSplCLib_MultDistribution MSet = BSplCLib::MultForm(CMults, 1, CMults.Length());
    switch (MSet)
    {
      case BSplCLib_NonConstant:
        break;
      case BSplCLib_Constant:
        if (CKnots.Length() == 2)
        {
          KnotForm = GeomAbs_PiecewiseBezier;
        }
        else
        {
          if (CMults(1) == 1)
            KnotForm = GeomAbs_Uniform;
        }
        break;
      case BSplCLib_QuasiConstant:
        if (CMults(1) == Degree + 1)
        {
          double M = CMults(2);
          if (M == Degree)
            KnotForm = GeomAbs_PiecewiseBezier;
          else if (M == 1)
            KnotForm = GeomAbs_QuasiUniform;
        }
        break;
    }
  }

  int FirstKM = Periodic ? CKnots.Lower() : BSplCLib::FirstUKnotIndex(Degree, CMults);
  int LastKM  = Periodic ? CKnots.Upper() : BSplCLib::LastUKnotIndex(Degree, CMults);
  MaxKnotMult              = 0;
  if (LastKM - FirstKM != 1)
  {
    int Multi;
    for (int i = FirstKM + 1; i < LastKM; i++)
    {
      Multi       = CMults(i);
      MaxKnotMult = std::max(MaxKnotMult, Multi);
    }
  }
}

//=======================================================================
// function : Rational
// purpose  : check rationality of an array of weights
//=======================================================================

static bool Rational(const NCollection_Array1<double>& W)
{
  int i, n = W.Length();
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

occ::handle<Law_BSpline> Law_BSpline::Copy() const
{
  occ::handle<Law_BSpline> C;
  if (IsRational())
    C = new Law_BSpline(poles->Array1(),
                        weights->Array1(),
                        knots->Array1(),
                        mults->Array1(),
                        deg,
                        periodic);
  else
    C = new Law_BSpline(poles->Array1(), knots->Array1(), mults->Array1(), deg, periodic);
  return C;
}

//=================================================================================================

Law_BSpline::Law_BSpline(const NCollection_Array1<double>&    Poles,
                         const NCollection_Array1<double>&    Knots,
                         const NCollection_Array1<int>& Mults,
                         const int         Degree,
                         const bool         Periodic)
    : rational(false),
      periodic(Periodic),
      deg(Degree)
{
  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  // copy arrays

  poles                 = new NCollection_HArray1<double>(1, Poles.Length());
  poles->ChangeArray1() = Poles;

  knots                 = new NCollection_HArray1<double>(1, Knots.Length());
  knots->ChangeArray1() = Knots;

  mults                 = new NCollection_HArray1<int>(1, Mults.Length());
  mults->ChangeArray1() = Mults;

  UpdateKnots();
}

//=================================================================================================

Law_BSpline::Law_BSpline(const NCollection_Array1<double>&    Poles,
                         const NCollection_Array1<double>&    Weights,
                         const NCollection_Array1<double>&    Knots,
                         const NCollection_Array1<int>& Mults,
                         const int         Degree,
                         const bool         Periodic)
    : rational(true),
      periodic(Periodic),
      deg(Degree)

{

  // check

  CheckCurveData(Poles, Knots, Mults, Degree, Periodic);

  if (Weights.Length() != Poles.Length())
    throw Standard_ConstructionError("Law_BSpline");

  int i;
  for (i = Weights.Lower(); i <= Weights.Upper(); i++)
  {
    if (Weights(i) <= gp::Resolution())
      throw Standard_ConstructionError("Law_BSpline");
  }

  // check really rational
  rational = Rational(Weights);

  // copy arrays

  poles                 = new NCollection_HArray1<double>(1, Poles.Length());
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

int Law_BSpline::MaxDegree()
{
  return BSplCLib::MaxDegree();
}

//=================================================================================================

void Law_BSpline::IncreaseDegree(const int Degree)
{
  if (Degree == deg)
    return;

  if (Degree < deg || Degree > Law_BSpline::MaxDegree())
  {
    throw Standard_ConstructionError();
  }

  int FromK1 = FirstUKnotIndex();
  int ToK2   = LastUKnotIndex();

  int Step = Degree - deg;

  occ::handle<NCollection_HArray1<double>> npoles =
    new NCollection_HArray1<double>(1, poles->Length() + Step * (ToK2 - FromK1));

  int nbknots =
    BSplCLib::IncreaseDegreeCountKnots(deg, Degree, periodic, mults->Array1());

  occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, nbknots);

  occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, nbknots);

  occ::handle<NCollection_HArray1<double>> nweights;

  if (IsRational())
  {
    nweights = new NCollection_HArray1<double>(1, npoles->Upper());
    NCollection_Array1<double> adimpol(1, 2 * poles->Upper());
    SetPoles(poles->Array1(), weights->Array1(), adimpol);
    NCollection_Array1<double> adimnpol(1, 2 * npoles->Upper());
    BSplCLib::IncreaseDegree(deg,
                             Degree,
                             periodic,
                             2,
                             adimpol,
                             knots->Array1(),
                             mults->Array1(),
                             adimnpol,
                             nknots->ChangeArray1(),
                             nmults->ChangeArray1());
    GetPoles(adimnpol, npoles->ChangeArray1(), nweights->ChangeArray1());
  }
  else
  {
    BSplCLib::IncreaseDegree(deg,
                             Degree,
                             periodic,
                             1,
                             poles->Array1(),
                             knots->Array1(),
                             mults->Array1(),
                             npoles->ChangeArray1(),
                             nknots->ChangeArray1(),
                             nmults->ChangeArray1());
  }

  deg     = Degree;
  poles   = npoles;
  weights = nweights;
  knots   = nknots;
  mults   = nmults;
  UpdateKnots();
}

//=================================================================================================

void Law_BSpline::IncreaseMultiplicity(const int Index, const int M)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = knots->Value(Index);
  NCollection_Array1<int> m(1, 1);
  m(1) = M - mults->Value(Index);
  InsertKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Law_BSpline::IncreaseMultiplicity(const int I1,
                                       const int I2,
                                       const int M)
{
  occ::handle<NCollection_HArray1<double>> tk = knots;
  NCollection_Array1<double>          k((knots->Array1())(I1), I1, I2);
  NCollection_Array1<int>       m(I1, I2);
  int              i;
  for (i = I1; i <= I2; i++)
    m(i) = M - mults->Value(i);
  InsertKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Law_BSpline::IncrementMultiplicity(const int I1,
                                        const int I2,
                                        const int Step)
{
  occ::handle<NCollection_HArray1<double>> tk = knots;
  NCollection_Array1<double>          k((knots->Array1())(I1), I1, I2);
  NCollection_Array1<int>       m(I1, I2);
  m.Init(Step);
  InsertKnots(k, m, Epsilon(1.));
}

//=================================================================================================

void Law_BSpline::InsertKnot(const double    U,
                             const int M,
                             const double    ParametricTolerance,
                             const bool Add)
{
  NCollection_Array1<double> k(1, 1);
  k(1) = U;
  NCollection_Array1<int> m(1, 1);
  m(1) = M;
  InsertKnots(k, m, ParametricTolerance, Add);
}

//=================================================================================================

void Law_BSpline::InsertKnots(const NCollection_Array1<double>&    Knots,
                              const NCollection_Array1<int>& Mults,
                              const double            Epsilon,
                              const bool         Add)
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
    throw Standard_ConstructionError("Law_BSpline::InsertKnots");

  if (nbpoles == poles->Length())
    return;

  occ::handle<NCollection_HArray1<double>>    npoles = new NCollection_HArray1<double>(1, nbpoles);
  occ::handle<NCollection_HArray1<double>>    nknots = knots;
  occ::handle<NCollection_HArray1<int>> nmults = mults;

  if (nbknots != knots->Length())
  {
    nknots = new NCollection_HArray1<double>(1, nbknots);
    nmults = new NCollection_HArray1<int>(1, nbknots);
  }

  if (rational)
  {
    occ::handle<NCollection_HArray1<double>> nweights = new NCollection_HArray1<double>(1, nbpoles);
    NCollection_Array1<double>          adimpol(1, 2 * poles->Upper());
    SetPoles(poles->Array1(), weights->Array1(), adimpol);
    NCollection_Array1<double> adimnpol(1, 2 * npoles->Upper());
    BSplCLib::InsertKnots(deg,
                          periodic,
                          2,
                          adimpol,
                          knots->Array1(),
                          mults->Array1(),
                          Knots,
                          &Mults,
                          adimnpol,
                          nknots->ChangeArray1(),
                          nmults->ChangeArray1(),
                          Epsilon,
                          Add);
    GetPoles(adimnpol, npoles->ChangeArray1(), nweights->ChangeArray1());
    weights = nweights;
  }
  else
  {
    BSplCLib::InsertKnots(deg,
                          periodic,
                          1,
                          poles->Array1(),
                          knots->Array1(),
                          mults->Array1(),
                          Knots,
                          &Mults,
                          npoles->ChangeArray1(),
                          nknots->ChangeArray1(),
                          nmults->ChangeArray1(),
                          Epsilon,
                          Add);
  }

  poles = npoles;
  knots = nknots;
  mults = nmults;
  UpdateKnots();
}

//=================================================================================================

bool Law_BSpline::RemoveKnot(const int Index,
                                         const int M,
                                         const double    Tolerance)
{
  if (M < 0)
    return true;

  int I1 = FirstUKnotIndex();
  int I2 = LastUKnotIndex();

  if (!periodic && (Index <= I1 || Index >= I2))
  {
    throw Standard_OutOfRange();
  }
  else if (periodic && (Index < I1 || Index > I2))
  {
    throw Standard_OutOfRange();
  }

  const NCollection_Array1<double>& oldpoles = poles->Array1();
  int            step     = mults->Value(Index) - M;
  if (step <= 0)
    return true;

  occ::handle<NCollection_HArray1<double>> npoles = new NCollection_HArray1<double>(1, oldpoles.Length() - step);

  occ::handle<NCollection_HArray1<double>>    nknots = knots;
  occ::handle<NCollection_HArray1<int>> nmults = mults;

  if (M == 0)
  {
    nknots = new NCollection_HArray1<double>(1, knots->Length() - 1);
    nmults = new NCollection_HArray1<int>(1, knots->Length() - 1);
  }

  if (IsRational())
  {
    occ::handle<NCollection_HArray1<double>> nweights = new NCollection_HArray1<double>(1, npoles->Length());
    NCollection_Array1<double>          adimpol(1, 2 * poles->Upper());
    SetPoles(poles->Array1(), weights->Array1(), adimpol);
    NCollection_Array1<double> adimnpol(1, 2 * npoles->Upper());
    if (!BSplCLib::RemoveKnot(Index,
                              M,
                              deg,
                              periodic,
                              2,
                              adimpol,
                              knots->Array1(),
                              mults->Array1(),
                              adimnpol,
                              nknots->ChangeArray1(),
                              nmults->ChangeArray1(),
                              Tolerance))
      return false;
    GetPoles(adimnpol, npoles->ChangeArray1(), nweights->ChangeArray1());
    weights = nweights;
  }
  else
  {
    if (!BSplCLib::RemoveKnot(Index,
                              M,
                              deg,
                              periodic,
                              1,
                              poles->Array1(),
                              knots->Array1(),
                              mults->Array1(),
                              npoles->ChangeArray1(),
                              nknots->ChangeArray1(),
                              nmults->ChangeArray1(),
                              Tolerance))
      return false;
  }

  poles = npoles;
  knots = nknots;
  mults = nmults;

  UpdateKnots();
  return true;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

#if 0  

--- methodes otees du CDL -> spec trop vagues : on ne sait pas ou rajouter
le noeud

//=================================================================================================

void Law_BSpline::InsertPoleAfter
(const int Index,
 const double& P)
{
  InsertPoleAfter(Index,P,1.);
}

//=================================================================================================

void Law_BSpline::InsertPoleAfter
(const int Index,
 const double& P,
 const double Weight)
{
  if (Index < 0 || Index > poles->Length())  throw Standard_OutOfRange();
  
  if (Weight <= gp::Resolution())     throw Standard_ConstructionError();
  
  
  // find the spans which are modified with the inserting pole
  //   --> evaluate NewKnot & KnotIndex : Value of the new knot to insert.
  int KnotIndex, k, sigma;
  double    NewKnot;
  
  if (periodic) {
    sigma = 0;
    k     = 1;
    while ( sigma < Index) {
      sigma += mults->Value(k);
      k++;
    }
    KnotIndex = k - 1;
    NewKnot   = ( knots->Value(KnotIndex) + knots->Value(KnotIndex+1)) / 2.;
  }
  else {
    sigma = 0;
    k     = 1;
    while ( sigma < Index) {
      sigma += mults->Value(k);
      k++;
    }
    int first = k - 1;
    sigma -= Index;
    while ( sigma < (deg+1)) {
      sigma += mults->Value(k);
      k++;
    }
    int last = k - 1;
    
    KnotIndex = first + (( last - first) / 2);
    NewKnot = ( knots->Value(KnotIndex) + knots->Value(KnotIndex+1)) / 2.;
  }
  
  int nbknots = knots->Length();
  occ::handle<NCollection_HArray1<double>> nknots = 
    new NCollection_HArray1<double>(1,nbknots+1);
  NCollection_Array1<double>& newknots = nknots->ChangeArray1();
  occ::handle<NCollection_HArray1<int>> nmults =
    new NCollection_HArray1<int>(1,nbknots+1);
  NCollection_Array1<int>& newmults = nmults->ChangeArray1();
  
  // insert the knot
  
  int i;
  for ( i = 1; i<= KnotIndex; i++) { 
    newknots(i) = knots->Value(i);
    newmults(i) = mults->Value(i);
  }
  newknots(KnotIndex+1) = NewKnot;
  newmults(KnotIndex+1) = 1;
  for ( i = KnotIndex+1; i <= nbknots; i++) {
    newknots(i+1) = knots->Value(i);
    newmults(i+1) = mults->Value(i);
  }
  
  int nbpoles = poles->Length();
  occ::handle<NCollection_HArray1<double>> npoles = 
    new NCollection_HArray1<double>(1,nbpoles+1);
  NCollection_Array1<double>& newpoles = npoles->ChangeArray1();
  
  // insert the pole
  
  for (i = 1; i <= Index; i++)
    newpoles(i) = poles->Value(i);
  
  newpoles(Index+1) = P;
  
  for (i = Index+1; i <= nbpoles; i++)
    newpoles(i+1) = poles->Value(i);
  
  // insert the weight 
  
  occ::handle<NCollection_HArray1<double>> nweights;
  bool rat = IsRational() || std::abs(Weight-1.) > gp::Resolution();
  
  if (rat) {
    nweights = new NCollection_HArray1<double>(1,nbpoles+1);
    NCollection_Array1<double>& newweights = nweights->ChangeArray1();
    
    for (i = 1; i <= Index; i++)
      if (IsRational())
	newweights(i) = weights->Value(i);
      else
	newweights(i) = 1.;
    
    newweights(Index+1) = Weight;
    
    for (i = Index+1; i <= nbpoles; i++)
      if (IsRational())
	newweights(i+1) = weights->Value(i);
      else
	newweights(i+1) = 1.;
  }
  
  poles   = npoles;
  weights = nweights;
  knots   = nknots;
  mults   = nmults;
  UpdateKnots();
}

//=================================================================================================

void Law_BSpline::InsertPoleBefore
(const int Index,
 const double& P )
{
  InsertPoleAfter(Index-1,P,1.);
}

//=================================================================================================

void Law_BSpline::InsertPoleBefore
(const int Index,
 const double& P,
 const double Weight)
{
  InsertPoleAfter(Index-1,P,Weight);
}

//=================================================================================================

void Law_BSpline::RemovePole
(const int Index)
{
  if (Index < 1 || Index > poles->Length())  throw Standard_OutOfRange();
  
  if (poles->Length() <= 2)           throw Standard_ConstructionError();
  
  if (knotSet == GeomAbs_NonUniform || knotSet == GeomAbs_PiecewiseBezier) 
    throw Standard_ConstructionError();
  
  int i;
  occ::handle<NCollection_HArray1<double>> nknots =
    new NCollection_HArray1<double>(1,knots->Length()-1);
  NCollection_Array1<double>& newknots = nknots->ChangeArray1();
  
  occ::handle<NCollection_HArray1<int>> nmults =
    new NCollection_HArray1<int>(1,mults->Length()-1);
  NCollection_Array1<int>& newmults = nmults->ChangeArray1();
  
  for (i = 1; i < newknots.Length(); i++) {
    newknots (i) = knots->Value (i);
    newmults (i) = 1;
  }
  newmults(1) = mults->Value(1);
  newknots(newknots.Upper()) = knots->Value (knots->Upper());
  newmults(newmults.Upper()) = mults->Value (mults->Upper());
  
  
  occ::handle<NCollection_HArray1<double>> npoles =
    new NCollection_HArray1<double>(1, poles->Upper()-1);
  NCollection_Array1<double>& newpoles = npoles->ChangeArray1();
  
  for (i = 1; i < Index; i++)
    newpoles(i) = poles->Value(i);
  for (i = Index; i < newpoles.Length(); i++)
    newpoles(i) = poles->Value(i+1);
  
  occ::handle<NCollection_HArray1<double>> nweights;
  if (IsRational()) {
    nweights = new NCollection_HArray1<double>(1,newpoles.Length());
    NCollection_Array1<double>& newweights = nweights->ChangeArray1();
    for (i = 1; i < Index; i++)
      newweights(i) = weights->Value(i);
    for (i = Index; i < newweights.Length(); i++)
      newweights(i) = weights->Value(i+1);
  }
  
  poles   = npoles;
  weights = nweights;
  knots   = nknots;
  mults   = nmults;
  UpdateKnots();
}

#endif

//=================================================================================================

void Law_BSpline::Reverse()
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

double Law_BSpline::ReversedParameter(const double U) const
{
  return (FirstParameter() + LastParameter() - U);
}

//=================================================================================================

void Law_BSpline::Segment(const double U1, const double U2)
{
  Standard_DomainError_Raise_if(U2 < U1, "Law_BSpline::Segment");
  double Eps   = Epsilon(std::max(std::abs(U1), std::abs(U2)));
  double delta = U2 - U1;

  double    NewU1, NewU2;
  double    U;
  int index;

  NCollection_Array1<double>    Knots(1, 2);
  NCollection_Array1<int> Mults(1, 2);

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
  Knots(1) = std::min(NewU1, NewU2);
  Knots(2) = std::max(NewU1, NewU2);
  Mults(1) = Mults(2) = deg;
  InsertKnots(Knots, Mults, Eps);

  if (periodic)
  { // set the origine at NewU1
    int index0 = 0;
    BSplCLib::LocateParameter(deg,
                              knots->Array1(),
                              mults->Array1(),
                              U1,
                              periodic,
                              knots->Lower(),
                              knots->Upper(),
                              index0,
                              U);
    if (std::abs(knots->Value(index0 + 1) - U) < Eps)
      index0++;
    SetOrigin(index0);
    SetNotPeriodic();
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
  BSplCLib::LocateParameter(deg,
                            knots->Array1(),
                            mults->Array1(),
                            NewU1 + delta,
                            periodic,
                            FromU1,
                            ToU2,
                            index2,
                            U);
  if (std::abs(knots->Value(index2 + 1) - U) < Eps)
    index2++;

  int nbknots = index2 - index1 + 1;

  occ::handle<NCollection_HArray1<double>>    nknots = new NCollection_HArray1<double>(1, nbknots);
  occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, nbknots);

  int i, k = 1;
  for (i = index1; i <= index2; i++)
  {
    nknots->SetValue(k, knots->Value(i));
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
  occ::handle<NCollection_HArray1<double>> npoles   = new NCollection_HArray1<double>(1, nbpoles);

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

  knots = nknots;
  mults = nmults;
  poles = npoles;
  if (rational)
    weights = nweights;

  UpdateKnots();
}

//=================================================================================================

void Law_BSpline::SetKnot(const int Index, const double K)
{
  if (Index < 1 || Index > knots->Length())
    throw Standard_OutOfRange();
  double DK = std::abs(Epsilon(K));
  if (Index == 1)
  {
    if (K >= knots->Value(2) - DK)
      throw Standard_ConstructionError();
  }
  else if (Index == knots->Length())
  {
    if (K <= knots->Value(knots->Length() - 1) + DK)
    {
      throw Standard_ConstructionError();
    }
  }
  else
  {
    if (K <= knots->Value(Index - 1) + DK || K >= knots->Value(Index + 1) - DK)
    {
      throw Standard_ConstructionError();
    }
  }
  if (K != knots->Value(Index))
  {
    knots->SetValue(Index, K);
    UpdateKnots();
  }
}

//=================================================================================================

void Law_BSpline::SetKnots(const NCollection_Array1<double>& K)
{
  CheckCurveData(poles->Array1(), K, mults->Array1(), deg, periodic);
  knots->ChangeArray1() = K;
  UpdateKnots();
}

//=================================================================================================

void Law_BSpline::SetKnot(const int Index,
                          const double    K,
                          const int M)
{
  IncreaseMultiplicity(Index, M);
  SetKnot(Index, K);
}

//=================================================================================================

void Law_BSpline::SetPeriodic()
{
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  occ::handle<NCollection_HArray1<double>> tk = knots;
  NCollection_Array1<double>          cknots((knots->Array1())(first), first, last);
  knots                 = new NCollection_HArray1<double>(1, cknots.Length());
  knots->ChangeArray1() = cknots;

  occ::handle<NCollection_HArray1<int>> tm = mults;
  NCollection_Array1<int>          cmults((mults->Array1())(first), first, last);
  cmults(first) = cmults(last) = std::max(cmults(first), cmults(last));
  mults                        = new NCollection_HArray1<int>(1, cmults.Length());
  mults->ChangeArray1()        = cmults;

  // compute new number of poles;
  int nbp = BSplCLib::NbPoles(deg, true, cmults);

  occ::handle<NCollection_HArray1<double>> tp = poles;
  NCollection_Array1<double>          cpoles((poles->Array1())(1), 1, nbp);
  poles                 = new NCollection_HArray1<double>(1, nbp);
  poles->ChangeArray1() = cpoles;

  if (rational)
  {
    occ::handle<NCollection_HArray1<double>> tw = weights;
    NCollection_Array1<double>          cweights((weights->Array1())(1), 1, nbp);
    weights                 = new NCollection_HArray1<double>(1, nbp);
    weights->ChangeArray1() = cweights;
  }

  periodic = true;

  UpdateKnots();
}

//=================================================================================================

void Law_BSpline::SetOrigin(const int Index)
{
  Standard_NoSuchObject_Raise_if(!periodic, "Law_BSpline::SetOrigin");
  int i, k;
  int first = FirstUKnotIndex();
  int last  = LastUKnotIndex();

  Standard_DomainError_Raise_if((Index < first) || (Index > last), "Law_BSpline::SetOrigine");

  int nbknots = knots->Length();
  int nbpoles = poles->Length();

  occ::handle<NCollection_HArray1<double>> nknots   = new NCollection_HArray1<double>(1, nbknots);
  NCollection_Array1<double>&         newknots = nknots->ChangeArray1();

  occ::handle<NCollection_HArray1<int>> nmults   = new NCollection_HArray1<int>(1, nbknots);
  NCollection_Array1<int>&         newmults = nmults->ChangeArray1();

  // set the knots and mults
  double period = knots->Value(last) - knots->Value(first);
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

  int index = 1;
  for (i = first + 1; i <= Index; i++)
    index += mults->Value(i);

  // set the poles and weights
  occ::handle<NCollection_HArray1<double>> npoles     = new NCollection_HArray1<double>(1, nbpoles);
  occ::handle<NCollection_HArray1<double>> nweights   = new NCollection_HArray1<double>(1, nbpoles);
  NCollection_Array1<double>&         newpoles   = npoles->ChangeArray1();
  NCollection_Array1<double>&         newweights = nweights->ChangeArray1();
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
  UpdateKnots();
}

//=================================================================================================

void Law_BSpline::SetNotPeriodic()
{
  if (periodic)
  {
    int NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize(deg, mults->Array1(), NbKnots, NbPoles);

    occ::handle<NCollection_HArray1<double>> npoles = new NCollection_HArray1<double>(1, NbPoles);

    occ::handle<NCollection_HArray1<double>> nknots = new NCollection_HArray1<double>(1, NbKnots);

    occ::handle<NCollection_HArray1<int>> nmults = new NCollection_HArray1<int>(1, NbKnots);

    occ::handle<NCollection_HArray1<double>> nweights;

    if (IsRational())
    {

      nweights = new NCollection_HArray1<double>(1, NbPoles);

      NCollection_Array1<double> adimpol(1, 2 * poles->Upper());
      SetPoles(poles->Array1(), weights->Array1(), adimpol);
      NCollection_Array1<double> adimnpol(1, 2 * npoles->Upper());
      BSplCLib::Unperiodize(deg,
                            1,
                            mults->Array1(),
                            knots->Array1(),
                            adimpol,
                            nmults->ChangeArray1(),
                            nknots->ChangeArray1(),
                            adimnpol);
      GetPoles(adimnpol, npoles->ChangeArray1(), nweights->ChangeArray1());
    }
    else
    {

      BSplCLib::Unperiodize(deg,
                            1,
                            mults->Array1(),
                            knots->Array1(),
                            poles->Array1(),
                            nmults->ChangeArray1(),
                            nknots->ChangeArray1(),
                            npoles->ChangeArray1());
    }
    poles    = npoles;
    weights  = nweights;
    mults    = nmults;
    knots    = nknots;
    periodic = false;

    UpdateKnots();
  }
}

//=================================================================================================

void Law_BSpline::SetPole(const int Index, const double P)
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange();
  poles->SetValue(Index, P);
}

//=================================================================================================

void Law_BSpline::SetPole(const int Index,
                          const double    P,
                          const double    W)
{
  SetPole(Index, P);
  SetWeight(Index, W);
}

//=================================================================================================

void Law_BSpline::SetWeight(const int Index, const double W)
{
  if (Index < 1 || Index > poles->Length())
    throw Standard_OutOfRange();

  if (W <= gp::Resolution())
    throw Standard_ConstructionError();

  bool rat = IsRational() || (std::abs(W - 1.) > gp::Resolution());

  if (rat)
  {
    if (rat && !IsRational())
      weights = new NCollection_HArray1<double>(1, poles->Length(), 1.);

    weights->SetValue(Index, W);

    if (IsRational())
    {
      rat = Rational(weights->Array1());
      if (!rat)
        weights.Nullify();
    }

    rational = !weights.IsNull();
  }
}

//=================================================================================================

void Law_BSpline::UpdateKnots()
{

  rational = !weights.IsNull();

  int MaxKnotMult = 0;
  KnotAnalysis(deg, periodic, knots->Array1(), mults->Array1(), knotSet, MaxKnotMult);

  if (knotSet == GeomAbs_Uniform && !periodic)
  {
    flatknots = knots;
  }
  else
  {
    flatknots =
      new NCollection_HArray1<double>(1, BSplCLib::KnotSequenceLength(mults->Array1(), deg, periodic));

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

void Law_BSpline::PeriodicNormalization(double& Parameter) const
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

bool Law_BSpline::IsCN(const int N) const
{
  Standard_RangeError_Raise_if(N < 0, "Law_BSpline::IsCN");

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
                    : N <= deg
                             - BSplCLib::MaxKnotMult(mults->Array1(),
                                                     mults->Lower() + 1,
                                                     mults->Upper() - 1);
    default:
      return false;
  }
}

//=================================================================================================

bool Law_BSpline::IsClosed() const
{
  return (std::abs(StartPoint() - EndPoint())) <= gp::Resolution();
}

//=================================================================================================

bool Law_BSpline::IsPeriodic() const
{
  return periodic;
}

//=================================================================================================

GeomAbs_Shape Law_BSpline::Continuity() const
{
  return smooth;
}

//=================================================================================================

int Law_BSpline::Degree() const
{
  return deg;
}

//=================================================================================================

double Law_BSpline::Value(const double U) const
{
  double P;
  D0(U, P);
  return P;
}

//=================================================================================================

void Law_BSpline::D0(const double U, double& P) const
{
  double NewU = U;
  PeriodicNormalization(NewU);
  if (rational)
  {
    BSplCLib::D0(NewU, 0, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, P);
  }
  else
  {
    BSplCLib::D0(NewU, 0, deg, periodic, POLES, BSplCLib::NoWeights(), FKNOTS, FMULTS, P);
  }
}

//=================================================================================================

void Law_BSpline::D1(const double U, double& P, double& V1) const
{
  double NewU = U;
  PeriodicNormalization(NewU);
  if (rational)
  {
    BSplCLib::D1(NewU, 0, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, P, V1);
  }
  else
  {
    BSplCLib::D1(NewU, 0, deg, periodic, POLES, BSplCLib::NoWeights(), FKNOTS, FMULTS, P, V1);
  }
}

//=================================================================================================

void Law_BSpline::D2(const double U,
                     double&      P,
                     double&      V1,
                     double&      V2) const
{
  double NewU = U;
  PeriodicNormalization(NewU);
  if (rational)
  {
    BSplCLib::D2(NewU, 0, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, P, V1, V2);
  }
  else
  {
    BSplCLib::D2(NewU, 0, deg, periodic, POLES, BSplCLib::NoWeights(), FKNOTS, FMULTS, P, V1, V2);
  }
}

//=================================================================================================

void Law_BSpline::D3(const double U,
                     double&      P,
                     double&      V1,
                     double&      V2,
                     double&      V3) const
{
  double NewU = U;
  PeriodicNormalization(NewU);
  if (rational)
  {
    BSplCLib::D3(NewU, 0, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, P, V1, V2, V3);
  }
  else
  {
    BSplCLib::D3(NewU,
                 0,
                 deg,
                 periodic,
                 POLES,
                 BSplCLib::NoWeights(),
                 FKNOTS,
                 FMULTS,
                 P,
                 V1,
                 V2,
                 V3);
  }
}

//=================================================================================================

double Law_BSpline::DN(const double U, const int N) const
{
  double V;
  if (rational)
  {
    BSplCLib::DN(U, N, 0, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, V);
  }
  else
  {
    BSplCLib::DN(U, N, 0, deg, periodic, POLES, BSplCLib::NoWeights(), FKNOTS, FMULTS, V);
  }
  return V;
}

//=================================================================================================

double Law_BSpline::EndPoint() const
{
  if (mults->Value(knots->Upper()) == deg + 1)
    return poles->Value(poles->Upper());
  else
    return Value(LastParameter());
}

//=================================================================================================

int Law_BSpline::FirstUKnotIndex() const
{
  if (periodic)
    return 1;
  else
    return BSplCLib::FirstUKnotIndex(deg, mults->Array1());
}

//=================================================================================================

double Law_BSpline::FirstParameter() const
{
  return flatknots->Value(deg + 1);
}

//=================================================================================================

double Law_BSpline::Knot(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > knots->Length(), "Law_BSpline::Knot");
  return knots->Value(Index);
}

//=================================================================================================

GeomAbs_BSplKnotDistribution Law_BSpline::KnotDistribution() const
{
  return knotSet;
}

//=================================================================================================

void Law_BSpline::Knots(NCollection_Array1<double>& K) const
{
  Standard_DimensionError_Raise_if(K.Length() != knots->Length(), "Law_BSpline::Knots");
  K = knots->Array1();
}

//=================================================================================================

void Law_BSpline::KnotSequence(NCollection_Array1<double>& K) const
{
  Standard_DimensionError_Raise_if(K.Length() != flatknots->Length(), "Law_BSpline::KnotSequence");
  K = flatknots->Array1();
}

//=================================================================================================

int Law_BSpline::LastUKnotIndex() const
{
  if (periodic)
    return knots->Length();
  else
    return BSplCLib::LastUKnotIndex(deg, mults->Array1());
}

//=================================================================================================

double Law_BSpline::LastParameter() const
{
  return flatknots->Value(flatknots->Upper() - deg);
}

//=================================================================================================

double Law_BSpline::LocalValue(const double    U,
                                      const int FromK1,
                                      const int ToK2) const
{
  double P;
  LocalD0(U, FromK1, ToK2, P);
  return P;
}

//=================================================================================================

void Law_BSpline::LocalD0(const double    U,
                          const int FromK1,
                          const int ToK2,
                          double&         P) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Law_BSpline::LocalValue");
  double    u     = U;
  int index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(deg, index, mults->Array1(), periodic);
  if (rational)
  {
    BSplCLib::D0(u, index, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, P);
  }
  else
  {
    BSplCLib::D0(u, index, deg, periodic, POLES, BSplCLib::NoWeights(), FKNOTS, FMULTS, P);
  }
}

//=================================================================================================

void Law_BSpline::LocalD1(const double    U,
                          const int FromK1,
                          const int ToK2,
                          double&         P,
                          double&         V1) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Law_BSpline::LocalD1");
  double    u     = U;
  int index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(deg, index, mults->Array1(), periodic);
  if (rational)
  {
    BSplCLib::D1(u, index, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, P, V1);
  }
  else
  {
    BSplCLib::D1(u, index, deg, periodic, POLES, BSplCLib::NoWeights(), FKNOTS, FMULTS, P, V1);
  }
}

//=================================================================================================

void Law_BSpline::LocalD2(const double    U,
                          const int FromK1,
                          const int ToK2,
                          double&         P,
                          double&         V1,
                          double&         V2) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Law_BSpline::LocalD2");
  double    u     = U;
  int index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(deg, index, mults->Array1(), periodic);
  if (rational)
  {
    BSplCLib::D2(u, index, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, P, V1, V2);
  }
  else
  {
    BSplCLib::D2(u, index, deg, periodic, POLES, BSplCLib::NoWeights(), FKNOTS, FMULTS, P, V1, V2);
  }
}

//=================================================================================================

void Law_BSpline::LocalD3(const double    U,
                          const int FromK1,
                          const int ToK2,
                          double&         P,
                          double&         V1,
                          double&         V2,
                          double&         V3) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Law_BSpline::LocalD3");
  double    u     = U;
  int index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(deg, index, mults->Array1(), periodic);
  if (rational)
  {
    BSplCLib::D3(u, index, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, P, V1, V2, V3);
  }
  else
  {
    BSplCLib::D3(u,
                 index,
                 deg,
                 periodic,
                 POLES,
                 BSplCLib::NoWeights(),
                 FKNOTS,
                 FMULTS,
                 P,
                 V1,
                 V2,
                 V3);
  }
}

//=================================================================================================

double Law_BSpline::LocalDN(const double    U,
                                   const int FromK1,
                                   const int ToK2,
                                   const int N) const
{
  Standard_DomainError_Raise_if(FromK1 == ToK2, "Law_BSpline::LocalD3");
  double    u     = U;
  int index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1, ToK2, index, u);
  index = BSplCLib::FlatIndex(deg, index, mults->Array1(), periodic);

  double V;
  if (rational)
  {
    BSplCLib::DN(u, N, index, deg, periodic, POLES, &weights->Array1(), FKNOTS, FMULTS, V);
  }
  else
  {
    BSplCLib::DN(u, N, index, deg, periodic, POLES, BSplCLib::NoWeights(), FKNOTS, FMULTS, V);
  }
  return V;
}

//=================================================================================================

int Law_BSpline::Multiplicity(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > mults->Length(), "Law_BSpline::Multiplicity");
  return mults->Value(Index);
}

//=================================================================================================

void Law_BSpline::Multiplicities(NCollection_Array1<int>& M) const
{
  Standard_DimensionError_Raise_if(M.Length() != mults->Length(), "Law_BSpline::Multiplicities");
  M = mults->Array1();
}

//=================================================================================================

int Law_BSpline::NbKnots() const
{
  return knots->Length();
}

//=================================================================================================

int Law_BSpline::NbPoles() const
{
  return poles->Length();
}

//=================================================================================================

double Law_BSpline::Pole(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > poles->Length(), "Law_BSpline::Pole");
  return poles->Value(Index);
}

//=================================================================================================

void Law_BSpline::Poles(NCollection_Array1<double>& P) const
{
  Standard_DimensionError_Raise_if(P.Length() != poles->Length(), "Law_BSpline::Poles");
  P = poles->Array1();
}

//=================================================================================================

double Law_BSpline::StartPoint() const
{
  if (mults->Value(1) == deg + 1)
    return poles->Value(1);
  else
    return Value(FirstParameter());
}

//=================================================================================================

double Law_BSpline::Weight(const int Index) const
{
  Standard_OutOfRange_Raise_if(Index < 1 || Index > poles->Length(), "Law_BSpline::Weight");
  if (IsRational())
    return weights->Value(Index);
  else
    return 1.;
}

//=================================================================================================

void Law_BSpline::Weights(NCollection_Array1<double>& W) const
{
  Standard_DimensionError_Raise_if(W.Length() != poles->Length(), "Law_BSpline::Weights");
  if (IsRational())
    W = weights->Array1();
  else
  {
    int i;
    for (i = W.Lower(); i <= W.Upper(); i++)
      W(i) = 1.;
  }
}

//=================================================================================================

bool Law_BSpline::IsRational() const
{
  return !weights.IsNull();
}

//=================================================================================================

void Law_BSpline::LocateU(const double    U,
                          const double    ParametricTolerance,
                          int&      I1,
                          int&      I2,
                          const bool WithKnotRepetition) const
{
  double                 NewU = U;
  occ::handle<NCollection_HArray1<double>> TheKnots;
  if (WithKnotRepetition)
    TheKnots = flatknots;
  else
    TheKnots = knots;

  PeriodicNormalization(NewU); // Attention a la periode

  const NCollection_Array1<double>& CKnots = TheKnots->Array1();
  double               UFirst = CKnots(1);
  double               ULast  = CKnots(CKnots.Length());
  if (std::abs(U - UFirst) <= std::abs(ParametricTolerance))
  {
    I1 = I2 = 1;
  }
  else if (std::abs(U - ULast) <= std::abs(ParametricTolerance))
  {
    I1 = I2 = CKnots.Length();
  }
  else if (NewU < UFirst - std::abs(ParametricTolerance))
  {
    I2 = 1;
    I1 = 0;
  }
  else if (NewU > ULast + std::abs(ParametricTolerance))
  {
    I1 = CKnots.Length();
    I2 = I1 + 1;
  }
  else
  {
    I1 = 1;
    BSplCLib::Hunt(CKnots, NewU, I1);
    I1 = std::max(std::min(I1, CKnots.Upper()), CKnots.Lower());
    while (I1 + 1 <= CKnots.Upper()
           && std::abs(CKnots(I1 + 1) - NewU) <= std::abs(ParametricTolerance))
    {
      I1++;
    }
    if (std::abs(CKnots(I1) - NewU) <= std::abs(ParametricTolerance))
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

void Law_BSpline::MovePointAndTangent(const double    U,
                                      const double    P,
                                      const double    Tangent,
                                      const double    Tolerance,
                                      const int StartingCondition,
                                      const int EndingCondition,
                                      int&      ErrorStatus)
{
  NCollection_Array1<double>   new_poles(1, poles->Length());
  double          delta, *poles_array, *new_poles_array, delta_derivative;
  const int dimension = 1;
  D1(U, delta, delta_derivative);
  delta = P - delta;

  delta_derivative = Tangent - delta_derivative;
  poles_array      = (double*)&poles->Array1()(1);
  new_poles_array  = (double*)&new_poles(1);
  BSplCLib::MovePointAndTangent(U,
                                dimension,
                                delta,
                                delta_derivative,
                                Tolerance,
                                deg,
                                StartingCondition,
                                EndingCondition,
                                poles_array[0],
                                rational ? &weights->Array1() : BSplCLib::NoWeights(),
                                flatknots->Array1(),
                                new_poles_array[0],
                                ErrorStatus);
  if (!ErrorStatus)
  {
    poles->ChangeArray1() = new_poles;
  }
}

//=================================================================================================

void Law_BSpline::Resolution(const double Tolerance3D, double& UTolerance) const
{
  void*          bid  = (void*)(&(poles->Value(1)));
  double* bidr = (double*)bid;
  if (rational)
  {
    BSplCLib::Resolution(*bidr,
                         1,
                         poles->Length(),
                         &weights->Array1(),
                         FKNOTS,
                         deg,
                         Tolerance3D,
                         UTolerance);
  }
  else
  {

    BSplCLib::Resolution(*bidr,
                         1,
                         poles->Length(),
                         BSplCLib::NoWeights(),
                         FKNOTS,
                         deg,
                         Tolerance3D,
                         UTolerance);
  }
}
