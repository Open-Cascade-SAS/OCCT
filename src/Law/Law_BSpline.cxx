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

IMPLEMENT_STANDARD_RTTIEXT(Law_BSpline,Standard_Transient)

#define  POLES    (poles->Array1())
#define  KNOTS    (knots->Array1())
#define  FKNOTS   (flatknots->Array1())
#define  FMULTS   (BSplCLib::NoMults())

//=======================================================================
//function : SetPoles
//purpose  : 
//=======================================================================

static void SetPoles(const TColStd_Array1OfReal& Poles,
		     const TColStd_Array1OfReal& Weights,
		     TColStd_Array1OfReal&       FP)
{
  Standard_Integer i,j = FP.Lower();
  for (i = Poles.Lower(); i <= Poles.Upper(); i++) {
    Standard_Real w = Weights(i);
    FP(j) = Poles(i) * w;
    j++;
    FP(j) = w;
    j++;
  }
}


//=======================================================================
//function : GetPoles
//purpose  : 
//=======================================================================

static void GetPoles(const TColStd_Array1OfReal& FP,
		     TColStd_Array1OfReal&       Poles,
		     TColStd_Array1OfReal&       Weights)
     
{
  Standard_Integer i,j = FP.Lower();
  for (i = Poles.Lower(); i <= Poles.Upper(); i++) {
    Standard_Real w = FP(j+1);
    Weights(i) = w;
    Poles(i) = FP(j) /w;
    j+=2;
  }
}


//=======================================================================
//function : CheckCurveData
//purpose  : Internal use only
//=======================================================================

static void CheckCurveData
(const TColStd_Array1OfReal&       CPoles,
 const TColStd_Array1OfReal&       CKnots,
 const TColStd_Array1OfInteger&    CMults,
 const Standard_Integer            Degree,
 const Standard_Boolean            Periodic)
{
  if (Degree < 1 || Degree > Law_BSpline::MaxDegree()) {
    throw Standard_ConstructionError();
  }
  
  if (CPoles.Length() < 2)                throw Standard_ConstructionError();
  if (CKnots.Length() != CMults.Length()) throw Standard_ConstructionError();
  
  for (Standard_Integer I = CKnots.Lower(); I < CKnots.Upper(); I++) {
    if (CKnots (I+1) - CKnots (I) <= Epsilon (Abs(CKnots (I)))) {
      throw Standard_ConstructionError();
    }
  }
  
  if (CPoles.Length() != BSplCLib::NbPoles(Degree,Periodic,CMults))
    throw Standard_ConstructionError();
}


//=======================================================================
//function : KnotAnalysis
//purpose  : Internal use only
//=======================================================================

static void KnotAnalysis
(const Standard_Integer           Degree,
 const Standard_Boolean           Periodic,
 const TColStd_Array1OfReal&      CKnots,
 const TColStd_Array1OfInteger&   CMults,
 GeomAbs_BSplKnotDistribution&    KnotForm,
 Standard_Integer&                MaxKnotMult)
{
  KnotForm = GeomAbs_NonUniform;
  
  BSplCLib_KnotDistribution KSet = 
    BSplCLib::KnotForm (CKnots, 1, CKnots.Length());
  
  
  if (KSet == BSplCLib_Uniform) {
    BSplCLib_MultDistribution MSet =
      BSplCLib::MultForm (CMults, 1, CMults.Length());
    switch (MSet) {
    case BSplCLib_NonConstant   :       
      break;
    case BSplCLib_Constant      : 
      if (CKnots.Length() == 2) {
	KnotForm = GeomAbs_PiecewiseBezier;
      }
      else {
	if (CMults (1) == 1)  KnotForm = GeomAbs_Uniform;   
      }
      break;
    case BSplCLib_QuasiConstant :   
      if (CMults (1) == Degree + 1) {
	Standard_Real M = CMults (2);
	if (M == Degree )   KnotForm = GeomAbs_PiecewiseBezier;
	else if  (M == 1)   KnotForm = GeomAbs_QuasiUniform;
      }
      break;
    }
  }
  
  Standard_Integer FirstKM = 
    Periodic ? CKnots.Lower() :  BSplCLib::FirstUKnotIndex (Degree,CMults);
  Standard_Integer LastKM = 
    Periodic ? CKnots.Upper() :  BSplCLib::LastUKnotIndex (Degree,CMults);
  MaxKnotMult = 0;
  if (LastKM - FirstKM != 1) {
    Standard_Integer Multi;
    for (Standard_Integer i = FirstKM + 1; i < LastKM; i++) {
      Multi = CMults (i);
      MaxKnotMult = Max (MaxKnotMult, Multi);
    }
  }
}


//=======================================================================
//function : Rational
//purpose  : check rationality of an array of weights
//=======================================================================

static Standard_Boolean Rational(const TColStd_Array1OfReal& W)
{
  Standard_Integer i, n = W.Length();
  Standard_Boolean rat = Standard_False;
  for (i = 1; i < n; i++) {
    rat =  Abs(W(i) - W(i+1)) > gp::Resolution();
    if (rat) break;
  }
  return rat;
}

//=======================================================================
//function : Copy
//purpose  : 
//=======================================================================

Handle(Law_BSpline) Law_BSpline::Copy() const
{
  Handle(Law_BSpline) C;
  if (IsRational()) 
    C = new Law_BSpline(poles->Array1(),
			weights->Array1(),
			knots->Array1(),
			mults->Array1(),
			deg,periodic);
  else
    C = new Law_BSpline(poles->Array1(),
			knots->Array1(),
			mults->Array1(),
			deg,periodic);
  return C;
}



//=======================================================================
//function : Law_BSpline
//purpose  : 
//=======================================================================

Law_BSpline::Law_BSpline
(const TColStd_Array1OfReal&       Poles,
 const TColStd_Array1OfReal&     Knots,
 const TColStd_Array1OfInteger&  Mults,
 const Standard_Integer          Degree,
 const Standard_Boolean          Periodic) :
 rational(Standard_False),periodic(Periodic), deg(Degree)  
{
  // check
  
  CheckCurveData (Poles,
		  Knots,
		  Mults,
		  Degree,
		  Periodic);
  
  
  // copy arrays
  
  poles =  new TColStd_HArray1OfReal(1,Poles.Length());
  poles->ChangeArray1() = Poles;
  
  
  knots = new TColStd_HArray1OfReal(1,Knots.Length());
  knots->ChangeArray1() = Knots;
  
  mults = new TColStd_HArray1OfInteger(1,Mults.Length());
  mults->ChangeArray1() = Mults;
  
  UpdateKnots();
}



//=======================================================================
//function : Law_BSpline
//purpose  : 
//=======================================================================

Law_BSpline::Law_BSpline
(const TColStd_Array1OfReal&      Poles,
 const TColStd_Array1OfReal&    Weights,
 const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger& Mults,
 const Standard_Integer         Degree,
 const Standard_Boolean         Periodic)  :
 rational(Standard_True),  periodic(Periodic), deg(Degree)
     
{
  
  // check
  
  CheckCurveData (Poles,
		  Knots,
		  Mults,
		  Degree,
		  Periodic);
  
  if (Weights.Length() != Poles.Length())
    throw Standard_ConstructionError("Law_BSpline");
  
  Standard_Integer i;
  for (i = Weights.Lower(); i <= Weights.Upper(); i++) {
    if (Weights(i) <= gp::Resolution())  
      throw Standard_ConstructionError("Law_BSpline");
  }
  
  // check really rational
  rational = Rational(Weights);
  
  // copy arrays
  
  poles =  new TColStd_HArray1OfReal(1,Poles.Length());
  poles->ChangeArray1() = Poles;
  if (rational) {
    weights =  new TColStd_HArray1OfReal(1,Weights.Length());
    weights->ChangeArray1() = Weights;
  }
  
  knots = new TColStd_HArray1OfReal(1,Knots.Length());
  knots->ChangeArray1() = Knots;
  
  mults = new TColStd_HArray1OfInteger(1,Mults.Length());
  mults->ChangeArray1() = Mults;
  
  UpdateKnots();
}


//=======================================================================
//function : MaxDegree
//purpose  : 
//=======================================================================

Standard_Integer Law_BSpline::MaxDegree () 
{ 
  return BSplCLib::MaxDegree(); 
}


//=======================================================================
//function : IncreaseDegree
//purpose  : 
//=======================================================================

void Law_BSpline::IncreaseDegree  (const Standard_Integer Degree)
{
  if (Degree == deg) return;
  
  if (Degree < deg || Degree > Law_BSpline::MaxDegree()) {
    throw Standard_ConstructionError();
  }
  
  Standard_Integer FromK1 = FirstUKnotIndex ();
  Standard_Integer ToK2   = LastUKnotIndex  ();
  
  Standard_Integer Step   = Degree - deg;
  
  Handle(TColStd_HArray1OfReal) npoles = new
    TColStd_HArray1OfReal(1,poles->Length() + Step * (ToK2-FromK1));
  
  Standard_Integer nbknots = BSplCLib::IncreaseDegreeCountKnots
    (deg,Degree,periodic,mults->Array1());
  
  Handle(TColStd_HArray1OfReal) nknots = 
    new TColStd_HArray1OfReal(1,nbknots);
  
  Handle(TColStd_HArray1OfInteger) nmults = 
    new TColStd_HArray1OfInteger(1,nbknots);
  
  Handle(TColStd_HArray1OfReal) nweights;
  
  if (IsRational()) {
    nweights = new TColStd_HArray1OfReal(1,npoles->Upper());
    TColStd_Array1OfReal adimpol(1,2*poles->Upper());
    SetPoles(poles->Array1(),weights->Array1(),adimpol);
    TColStd_Array1OfReal adimnpol(1,2*npoles->Upper());
    BSplCLib::IncreaseDegree
      (deg,Degree, periodic,2,adimpol,
       knots->Array1(),mults->Array1(),adimnpol,
       nknots->ChangeArray1(),nmults->ChangeArray1());
    GetPoles(adimnpol,npoles->ChangeArray1(),nweights->ChangeArray1());
  }
  else {
    BSplCLib::IncreaseDegree
      (deg,Degree, periodic,1,poles->Array1(),
       knots->Array1(),mults->Array1(),npoles->ChangeArray1(),
       nknots->ChangeArray1(),nmults->ChangeArray1());
  }
  
  deg     = Degree;
  poles   = npoles;
  weights = nweights;
  knots   = nknots;
  mults   = nmults;
  UpdateKnots();
  
}


//=======================================================================
//function : IncreaseMultiplicity
//purpose  : 
//=======================================================================

void Law_BSpline::IncreaseMultiplicity  (const Standard_Integer Index,
					 const Standard_Integer M)
{
  TColStd_Array1OfReal k(1,1);
  k(1) = knots->Value(Index);
  TColStd_Array1OfInteger m(1,1);
  m(1) = M - mults->Value(Index);
  InsertKnots(k,m,Epsilon(1.));
}


//=======================================================================
//function : IncreaseMultiplicity
//purpose  : 
//=======================================================================

void Law_BSpline::IncreaseMultiplicity  (const Standard_Integer I1,
					 const Standard_Integer I2,
					 const Standard_Integer M)
{
  Handle(TColStd_HArray1OfReal)  tk = knots;
  TColStd_Array1OfReal k((knots->Array1())(I1),I1,I2);
  TColStd_Array1OfInteger m(I1,I2);
  Standard_Integer i;
  for (i = I1; i <= I2; i++)
    m(i) = M - mults->Value(i);
  InsertKnots(k,m,Epsilon(1.));
}

//=======================================================================
//function : IncrementMultiplicity
//purpose  : 
//=======================================================================

void Law_BSpline::IncrementMultiplicity
(const Standard_Integer I1,
 const Standard_Integer I2,
 const Standard_Integer Step)
{
  Handle(TColStd_HArray1OfReal) tk = knots;
  TColStd_Array1OfReal    k((knots->Array1())(I1),I1,I2);
  TColStd_Array1OfInteger m(I1,I2) ;
  m.Init(Step);
  InsertKnots(k,m,Epsilon(1.));
}


//=======================================================================
//function : InsertKnot
//purpose  : 
//=======================================================================

void Law_BSpline::InsertKnot
(const Standard_Real U, 
 const Standard_Integer M, 
 const Standard_Real ParametricTolerance,
 const Standard_Boolean Add)
{
  TColStd_Array1OfReal k(1,1);
  k(1) = U;
  TColStd_Array1OfInteger m(1,1);
  m(1) = M;
  InsertKnots(k,m,ParametricTolerance,Add);
}

//=======================================================================
//function : InsertKnots
//purpose  : 
//=======================================================================

void  Law_BSpline::InsertKnots(const TColStd_Array1OfReal& Knots, 
			       const TColStd_Array1OfInteger& Mults,
			       const Standard_Real Epsilon,
			       const Standard_Boolean Add)
{
  // Check and compute new sizes
  Standard_Integer nbpoles,nbknots;
  
  if (!BSplCLib::PrepareInsertKnots(deg,periodic,
				    knots->Array1(),mults->Array1(),
				    Knots,&Mults,nbpoles,nbknots,Epsilon,Add))
    throw Standard_ConstructionError("Law_BSpline::InsertKnots");
  
  if (nbpoles == poles->Length()) return;
  
  Handle(TColStd_HArray1OfReal) npoles = new TColStd_HArray1OfReal(1,nbpoles);
  Handle(TColStd_HArray1OfReal) nknots = knots;
  Handle(TColStd_HArray1OfInteger) nmults = mults;
  
  if (nbknots != knots->Length()) {
    nknots = new TColStd_HArray1OfReal(1,nbknots);
    nmults = new TColStd_HArray1OfInteger(1,nbknots);
  }
  
  if (rational) {
    Handle(TColStd_HArray1OfReal) nweights = 
      new TColStd_HArray1OfReal(1,nbpoles);
    TColStd_Array1OfReal adimpol(1,2*poles->Upper());
    SetPoles(poles->Array1(),weights->Array1(),adimpol);
    TColStd_Array1OfReal adimnpol(1,2*npoles->Upper());
    BSplCLib::InsertKnots(deg,periodic,2,adimpol,
			  knots->Array1(), mults->Array1(),
			  Knots, &Mults,adimnpol,
			  nknots->ChangeArray1(), nmults->ChangeArray1(),
			  Epsilon, Add);
    GetPoles(adimnpol,npoles->ChangeArray1(),nweights->ChangeArray1());
    weights = nweights;
  }
  else {
    BSplCLib::InsertKnots(deg,periodic,1,poles->Array1(), 
			  knots->Array1(), mults->Array1(),
			  Knots, &Mults,
			  npoles->ChangeArray1(), 
			  nknots->ChangeArray1(), nmults->ChangeArray1(),
			  Epsilon, Add);
  }
  
  poles = npoles;
  knots = nknots;
  mults = nmults;
  UpdateKnots();
  
}

//=======================================================================
//function : RemoveKnot
//purpose  : 
//=======================================================================

Standard_Boolean  Law_BSpline::RemoveKnot(const Standard_Integer Index,
					  const Standard_Integer M, 
					  const Standard_Real Tolerance)
{
  if (M < 0) return Standard_True;
  
  Standard_Integer I1  = FirstUKnotIndex ();
  Standard_Integer I2  = LastUKnotIndex  ();
  
  if ( !periodic && (Index <= I1 || Index >= I2) ) {
    throw Standard_OutOfRange();
  }
  else if ( periodic  && (Index < I1 || Index > I2)) {
    throw Standard_OutOfRange();
  }
  
  const TColStd_Array1OfReal   & oldpoles   = poles->Array1();
  Standard_Integer step = mults->Value(Index) - M;
  if (step <= 0) return Standard_True;
  
  Handle(TColStd_HArray1OfReal) npoles =
    new TColStd_HArray1OfReal(1,oldpoles.Length()-step);
  
  Handle(TColStd_HArray1OfReal)    nknots  = knots;
  Handle(TColStd_HArray1OfInteger) nmults  = mults;
  
  if (M == 0) {
    nknots = new TColStd_HArray1OfReal(1,knots->Length()-1);
    nmults = new TColStd_HArray1OfInteger(1,knots->Length()-1);
  }
  
  if (IsRational()) {
    Handle(TColStd_HArray1OfReal) nweights = 
      new TColStd_HArray1OfReal(1,npoles->Length());
    TColStd_Array1OfReal adimpol(1,2*poles->Upper());
    SetPoles(poles->Array1(),weights->Array1(),adimpol);
    TColStd_Array1OfReal adimnpol(1,2*npoles->Upper());
    if (!BSplCLib::RemoveKnot(Index, M, deg, periodic,2,adimpol,
			      knots->Array1(),mults->Array1(),adimnpol,
			      nknots->ChangeArray1(),
			      nmults->ChangeArray1(),Tolerance))
      return Standard_False;
    GetPoles(adimnpol,npoles->ChangeArray1(),nweights->ChangeArray1());
    weights = nweights;
  }
  else {
    if (!BSplCLib::RemoveKnot(Index, M, deg, periodic,1,poles->Array1(), 
			      knots->Array1(),mults->Array1(),
			      npoles->ChangeArray1(), nknots->ChangeArray1(),
			      nmults->ChangeArray1(),Tolerance))
      return Standard_False;
  }
  
  poles = npoles;
  knots = nknots;
  mults = nmults;
  
  UpdateKnots();
  return Standard_True;
}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

# if 0  

--- methodes otees du CDL -> spec trop vagues : on ne sait pas ou rajouter
le noeud

//=======================================================================
//function : InsertPoleAfter
//purpose  : 
//=======================================================================

void Law_BSpline::InsertPoleAfter
(const Standard_Integer Index,
 const Standard_Real& P)
{
  InsertPoleAfter(Index,P,1.);
}



//=======================================================================
//function : InsertPoleAfter
//purpose  : 
//=======================================================================

void Law_BSpline::InsertPoleAfter
(const Standard_Integer Index,
 const Standard_Real& P,
 const Standard_Real Weight)
{
  if (Index < 0 || Index > poles->Length())  throw Standard_OutOfRange();
  
  if (Weight <= gp::Resolution())     throw Standard_ConstructionError();
  
  
  // find the spans which are modified with the inserting pole
  //   --> evaluate NewKnot & KnotIndex : Value of the new knot to insert.
  Standard_Integer KnotIndex, k, sigma;
  Standard_Real    NewKnot;
  
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
    Standard_Integer first = k - 1;
    sigma -= Index;
    while ( sigma < (deg+1)) {
      sigma += mults->Value(k);
      k++;
    }
    Standard_Integer last = k - 1;
    
    KnotIndex = first + (( last - first) / 2);
    NewKnot = ( knots->Value(KnotIndex) + knots->Value(KnotIndex+1)) / 2.;
  }
  
  Standard_Integer nbknots = knots->Length();
  Handle(TColStd_HArray1OfReal) nknots = 
    new TColStd_HArray1OfReal(1,nbknots+1);
  TColStd_Array1OfReal& newknots = nknots->ChangeArray1();
  Handle(TColStd_HArray1OfInteger) nmults =
    new TColStd_HArray1OfInteger(1,nbknots+1);
  TColStd_Array1OfInteger& newmults = nmults->ChangeArray1();
  
  // insert the knot
  
  Standard_Integer i;
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
  
  Standard_Integer nbpoles = poles->Length();
  Handle(TColStd_HArray1OfReal) npoles = 
    new TColStd_HArray1OfReal(1,nbpoles+1);
  TColStd_Array1OfReal& newpoles = npoles->ChangeArray1();
  
  // insert the pole
  
  for (i = 1; i <= Index; i++)
    newpoles(i) = poles->Value(i);
  
  newpoles(Index+1) = P;
  
  for (i = Index+1; i <= nbpoles; i++)
    newpoles(i+1) = poles->Value(i);
  
  // insert the weight 
  
  Handle(TColStd_HArray1OfReal) nweights;
  Standard_Boolean rat = IsRational() || Abs(Weight-1.) > gp::Resolution();
  
  if (rat) {
    nweights = new TColStd_HArray1OfReal(1,nbpoles+1);
    TColStd_Array1OfReal& newweights = nweights->ChangeArray1();
    
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


//=======================================================================
//function : InsertPoleBefore
//purpose  : 
//=======================================================================

void Law_BSpline::InsertPoleBefore
(const Standard_Integer Index,
 const Standard_Real& P )
{
  InsertPoleAfter(Index-1,P,1.);
}



//=======================================================================
//function : InsertPoleBefore
//purpose  : 
//=======================================================================

void Law_BSpline::InsertPoleBefore
(const Standard_Integer Index,
 const Standard_Real& P,
 const Standard_Real Weight)
{
  InsertPoleAfter(Index-1,P,Weight);
}

//=======================================================================
//function : RemovePole
//purpose  : 
//=======================================================================

void Law_BSpline::RemovePole
(const Standard_Integer Index)
{
  if (Index < 1 || Index > poles->Length())  throw Standard_OutOfRange();
  
  if (poles->Length() <= 2)           throw Standard_ConstructionError();
  
  if (knotSet == GeomAbs_NonUniform || knotSet == GeomAbs_PiecewiseBezier) 
    throw Standard_ConstructionError();
  
  Standard_Integer i;
  Handle(TColStd_HArray1OfReal) nknots =
    new TColStd_HArray1OfReal(1,knots->Length()-1);
  TColStd_Array1OfReal& newknots = nknots->ChangeArray1();
  
  Handle(TColStd_HArray1OfInteger) nmults =
    new TColStd_HArray1OfInteger(1,mults->Length()-1);
  TColStd_Array1OfInteger& newmults = nmults->ChangeArray1();
  
  for (i = 1; i < newknots.Length(); i++) {
    newknots (i) = knots->Value (i);
    newmults (i) = 1;
  }
  newmults(1) = mults->Value(1);
  newknots(newknots.Upper()) = knots->Value (knots->Upper());
  newmults(newmults.Upper()) = mults->Value (mults->Upper());
  
  
  Handle(TColStd_HArray1OfReal) npoles =
    new TColStd_HArray1OfReal(1, poles->Upper()-1);
  TColStd_Array1OfReal& newpoles = npoles->ChangeArray1();
  
  for (i = 1; i < Index; i++)
    newpoles(i) = poles->Value(i);
  for (i = Index; i < newpoles.Length(); i++)
    newpoles(i) = poles->Value(i+1);
  
  Handle(TColStd_HArray1OfReal) nweights;
  if (IsRational()) {
    nweights = new TColStd_HArray1OfReal(1,newpoles.Length());
    TColStd_Array1OfReal& newweights = nweights->ChangeArray1();
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


//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Law_BSpline::Reverse ()
{ 
  BSplCLib::Reverse(knots->ChangeArray1());
  BSplCLib::Reverse(mults->ChangeArray1());
  Standard_Integer last;
  if (periodic)
    last = flatknots->Upper() - deg - 1;
  else
    last = poles->Upper();
  BSplCLib::Reverse(poles->ChangeArray1(),last);
  if (rational)
    BSplCLib::Reverse(weights->ChangeArray1(),last);
  UpdateKnots();
}


//=======================================================================
//function : ReversedParameter
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::ReversedParameter
(const Standard_Real U) const
{
  return (FirstParameter() + LastParameter() - U);
}


//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

void Law_BSpline::Segment(const Standard_Real U1,
			  const Standard_Real U2)
{
  Standard_DomainError_Raise_if ( U2 < U1,
				 "Law_BSpline::Segment");
  Standard_Real Eps = Epsilon(Max(Abs(U1),Abs(U2)));
  Standard_Real delta = U2 - U1;
  
  Standard_Real NewU1, NewU2;
  Standard_Real U;
  Standard_Integer index;
  
  TColStd_Array1OfReal    Knots(1,2);
  TColStd_Array1OfInteger Mults(1,2);
  
  index = 0;
  BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			    U1,periodic,knots->Lower(),knots->Upper(),
			    index,NewU1);
  index = 0;
  BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			    U2,periodic,knots->Lower(),knots->Upper(),
			    index,NewU2);
  Knots( 1) = Min( NewU1, NewU2);
  Knots( 2) = Max( NewU1, NewU2);
  Mults( 1) = Mults( 2) = deg;
  InsertKnots( Knots, Mults, Eps);
  
  if (periodic) { // set the origine at NewU1
    Standard_Integer index0 = 0;
    BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			      U1,periodic,knots->Lower(),knots->Upper(),
			      index0,U);
    if ( Abs(knots->Value(index0+1)-U) < Eps)
      index0++;
    SetOrigin(index0);
    SetNotPeriodic();
  }
  
  // compute index1 and index2 to set the new knots and mults 
  Standard_Integer index1 = 0, index2 = 0;
  Standard_Integer FromU1 = knots->Lower();
  Standard_Integer ToU2   = knots->Upper();
  BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			    NewU1,periodic,FromU1,ToU2,index1,U);
  BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			    NewU1 + delta,periodic,FromU1,ToU2,index2,U);
  if ( Abs(knots->Value(index2+1)-U) < Eps)
    index2++;
  
  Standard_Integer nbknots = index2 - index1 + 1;
  
  Handle(TColStd_HArray1OfReal) 
    nknots = new TColStd_HArray1OfReal(1,nbknots);
  Handle(TColStd_HArray1OfInteger) 
    nmults = new TColStd_HArray1OfInteger(1,nbknots);
  
  Standard_Integer i , k = 1;
  for ( i = index1; i<= index2; i++) {
    nknots->SetValue(k, knots->Value(i));
    nmults->SetValue(k, mults->Value(i));
    k++;
  }
  nmults->SetValue(      1, deg + 1);
  nmults->SetValue(nbknots, deg + 1);
  
  
  // compute index1 and index2 to set the new poles and weights
  Standard_Integer pindex1 
    = BSplCLib::PoleIndex(deg,index1,periodic,mults->Array1());
  Standard_Integer pindex2 
    = BSplCLib::PoleIndex(deg,index2,periodic,mults->Array1());
  
  pindex1++;
  pindex2 = Min( pindex2+1, poles->Length());
  
  Standard_Integer nbpoles  = pindex2 - pindex1 + 1;
  
  Handle(TColStd_HArray1OfReal) 
    nweights = new TColStd_HArray1OfReal(1,nbpoles);
  Handle(TColStd_HArray1OfReal)
    npoles = new TColStd_HArray1OfReal(1,nbpoles);
  
  k = 1;
  if ( rational) {
    nweights = new TColStd_HArray1OfReal( 1, nbpoles);
    for ( i = pindex1; i <= pindex2; i++) {
      npoles->SetValue(k, poles->Value(i));
      nweights->SetValue(k, weights->Value(i));
      k++;
    }
  }
  else {
    for ( i = pindex1; i <= pindex2; i++) {
      npoles->SetValue(k, poles->Value(i));
      k++;
    }
  }
  
  knots = nknots;
  mults = nmults;
  poles = npoles;
  if ( rational) 
    weights = nweights;
  
  UpdateKnots();
}


//=======================================================================
//function : SetKnot
//purpose  : 
//=======================================================================

void Law_BSpline::SetKnot
(const Standard_Integer Index,
 const Standard_Real K)
{
  if (Index < 1 || Index > knots->Length())     throw Standard_OutOfRange();
  Standard_Real DK = Abs(Epsilon (K));
  if (Index == 1) { 
    if (K >= knots->Value(2) - DK) throw Standard_ConstructionError();
  }
  else if (Index == knots->Length()) {
    if (K <= knots->Value (knots->Length()-1) + DK)  {
      throw Standard_ConstructionError();
    }
  }
  else {
    if (K <= knots->Value(Index-1) + DK ||
	K >= knots->Value(Index+1) - DK ) {
      throw Standard_ConstructionError();
    }
  }
  if (K != knots->Value (Index)) {
    knots->SetValue (Index, K);
    UpdateKnots();
  }
}


//=======================================================================
//function : SetKnots
//purpose  : 
//=======================================================================

void Law_BSpline::SetKnots
(const TColStd_Array1OfReal& K)
{
  CheckCurveData(poles->Array1(),K,mults->Array1(),deg,periodic);
  knots->ChangeArray1() = K;
  UpdateKnots();
}


//=======================================================================
//function : SetKnot
//purpose  : 
//=======================================================================

void Law_BSpline::SetKnot
(const Standard_Integer Index,
 const Standard_Real K,
 const Standard_Integer M)
{
  IncreaseMultiplicity (Index, M);
  SetKnot (Index, K);
}


//=======================================================================
//function : SetPeriodic
//purpose  : 
//=======================================================================

void Law_BSpline::SetPeriodic ()
{
  Standard_Integer first = FirstUKnotIndex();
  Standard_Integer last  = LastUKnotIndex();
  
  Handle(TColStd_HArray1OfReal) tk = knots;
  TColStd_Array1OfReal    cknots((knots->Array1())(first),first,last);
  knots = new TColStd_HArray1OfReal(1,cknots.Length());
  knots->ChangeArray1() = cknots;
  
  Handle(TColStd_HArray1OfInteger) tm = mults;
  TColStd_Array1OfInteger cmults((mults->Array1())(first),first,last);
  cmults(first) = cmults(last) = Max( cmults(first), cmults(last));
  mults = new TColStd_HArray1OfInteger(1,cmults.Length());
  mults->ChangeArray1() = cmults;
  
  // compute new number of poles;
  Standard_Integer nbp = BSplCLib::NbPoles(deg,Standard_True,cmults);
  
  Handle(TColStd_HArray1OfReal) tp = poles;
  TColStd_Array1OfReal cpoles((poles->Array1())(1),1,nbp);
  poles = new TColStd_HArray1OfReal(1,nbp);
  poles->ChangeArray1() = cpoles;
  
  if (rational) {
    Handle(TColStd_HArray1OfReal) tw = weights;
    TColStd_Array1OfReal cweights((weights->Array1())(1),1,nbp);
    weights = new TColStd_HArray1OfReal(1,nbp);
    weights->ChangeArray1() = cweights;
  }
  
  periodic = Standard_True;
  
  UpdateKnots();
}


//=======================================================================
//function : SetOrigin
//purpose  : 
//=======================================================================

void Law_BSpline::SetOrigin(const Standard_Integer Index)
{
  Standard_NoSuchObject_Raise_if( !periodic,
				 "Law_BSpline::SetOrigin");
  Standard_Integer i,k;
  Standard_Integer first = FirstUKnotIndex();
  Standard_Integer last  = LastUKnotIndex();
  
  Standard_DomainError_Raise_if( (Index < first) || (Index > last),
				"Law_BSpline::SetOrigine");
  
  Standard_Integer nbknots = knots->Length();
  Standard_Integer nbpoles = poles->Length();
  
  Handle(TColStd_HArray1OfReal) nknots = 
    new TColStd_HArray1OfReal(1,nbknots);
  TColStd_Array1OfReal& newknots = nknots->ChangeArray1();
  
  Handle(TColStd_HArray1OfInteger) nmults =
    new TColStd_HArray1OfInteger(1,nbknots);
  TColStd_Array1OfInteger& newmults = nmults->ChangeArray1();
  
  // set the knots and mults
  Standard_Real period = knots->Value(last) - knots->Value(first);
  k = 1;
  for ( i = Index; i <= last ; i++) {
    newknots(k) = knots->Value(i);
    newmults(k) = mults->Value(i);
    k++;
  }
  for ( i = first+1; i <= Index; i++) {
    newknots(k) = knots->Value(i) + period;
    newmults(k) = mults->Value(i);
    k++;
  }
  
  Standard_Integer index = 1;
  for (i = first+1; i <= Index; i++) 
    index += mults->Value(i);
  
  // set the poles and weights
  Handle(TColStd_HArray1OfReal) npoles =
    new TColStd_HArray1OfReal(1,nbpoles);
  Handle(TColStd_HArray1OfReal) nweights =
    new TColStd_HArray1OfReal(1,nbpoles);
  TColStd_Array1OfReal   & newpoles   = npoles->ChangeArray1();
  TColStd_Array1OfReal & newweights = nweights->ChangeArray1();
  first = poles->Lower();
  last  = poles->Upper();
  if ( rational) {
    k = 1;
    for ( i = index; i <= last; i++) {
      newpoles(k)   = poles->Value(i);
      newweights(k) = weights->Value(i);
      k++;
    }
    for ( i = first; i < index; i++) {
      newpoles(k)   = poles->Value(i);
      newweights(k) = weights->Value(i);
      k++;
    }
  }
  else {
    k = 1;
    for ( i = index; i <= last; i++) {
      newpoles(k) = poles->Value(i);
      k++;
    }
    for ( i = first; i < index; i++) {
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



//=======================================================================
//function : SetNotPeriodic
//purpose  : 
//=======================================================================

void Law_BSpline::SetNotPeriodic () 
{ 
  if ( periodic) {
    Standard_Integer NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize( deg, mults->Array1(),NbKnots,NbPoles);
    
    Handle(TColStd_HArray1OfReal) npoles 
      = new TColStd_HArray1OfReal(1,NbPoles);
    
    Handle(TColStd_HArray1OfReal) nknots 
      = new TColStd_HArray1OfReal(1,NbKnots);
    
    Handle(TColStd_HArray1OfInteger) nmults
      = new TColStd_HArray1OfInteger(1,NbKnots);
    
    Handle(TColStd_HArray1OfReal) nweights;
    
    if (IsRational()) {
      
      nweights = new TColStd_HArray1OfReal(1,NbPoles);
      
      TColStd_Array1OfReal adimpol(1,2*poles->Upper());
      SetPoles(poles->Array1(),weights->Array1(),adimpol);
      TColStd_Array1OfReal adimnpol(1,2*npoles->Upper());
      BSplCLib::Unperiodize
	(deg,1,mults->Array1(),knots->Array1(),adimpol,
	 nmults->ChangeArray1(),nknots->ChangeArray1(),
	 adimnpol);
      GetPoles(adimnpol,npoles->ChangeArray1(),nweights->ChangeArray1());
    }
    else {
      
      BSplCLib::Unperiodize(deg,1,mults->Array1(),knots->Array1(),
			    poles->Array1(),nmults->ChangeArray1(), 
			    nknots->ChangeArray1(),npoles->ChangeArray1());
      
    }
    poles   = npoles;
    weights = nweights;
    mults   = nmults;
    knots   = nknots;
    periodic = Standard_False;
    
    UpdateKnots();
  }
}


//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Law_BSpline::SetPole
(const Standard_Integer Index,
 const Standard_Real P)
{
  if (Index < 1 || Index > poles->Length()) throw Standard_OutOfRange();
  poles->SetValue (Index, P);
}


//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Law_BSpline::SetPole
(const Standard_Integer Index,
 const Standard_Real P,
 const Standard_Real W)
{
  SetPole(Index,P);
  SetWeight(Index,W);
}

//=======================================================================
//function : SetWeight
//purpose  : 
//=======================================================================

void Law_BSpline::SetWeight
(const Standard_Integer Index,
 const Standard_Real W)
{
  if (Index < 1 || Index > poles->Length())   throw Standard_OutOfRange();
  
  if (W <= gp::Resolution ())     throw Standard_ConstructionError();
  
  
  Standard_Boolean rat = IsRational() || (Abs(W - 1.) > gp::Resolution());
  
  if ( rat) {
    if (rat && !IsRational())
      weights = new TColStd_HArray1OfReal(1,poles->Length(),1.);
    
    weights->SetValue (Index, W);
    
    if (IsRational()) {
      rat = Rational(weights->Array1());
      if (!rat) weights.Nullify();
    }
    
    rational = !weights.IsNull();
  }
}


//=======================================================================
//function : UpdateKnots
//purpose  : 
//=======================================================================


void Law_BSpline::UpdateKnots()
{
  
  rational = !weights.IsNull();
  
  Standard_Integer MaxKnotMult = 0;
  KnotAnalysis (deg, 
		periodic,
		knots->Array1(), 
		mults->Array1(), 
		knotSet, MaxKnotMult);
  
  if (knotSet == GeomAbs_Uniform && !periodic)  {
    flatknots = knots;
  }
  else {
    flatknots = new TColStd_HArray1OfReal 
      (1, BSplCLib::KnotSequenceLength(mults->Array1(),deg,periodic));
    
    BSplCLib::KnotSequence (knots->Array1(), 
			    mults->Array1(),
			    deg,periodic,
			    flatknots->ChangeArray1());
  }
  
  if (MaxKnotMult == 0)  smooth = GeomAbs_CN;
  else {
    switch (deg - MaxKnotMult) {
    case 0:   smooth = GeomAbs_C0;   break;
    case 1:   smooth = GeomAbs_C1;   break;
    case 2:   smooth = GeomAbs_C2;   break;
    case 3:   smooth = GeomAbs_C3;   break;
    default:  smooth = GeomAbs_C3;   break;
    }
  }
}


//=======================================================================
//function : Normalizes the parameters if the curve is periodic
//purpose  : that is compute the cache so that it is valid
//=======================================================================

void Law_BSpline::PeriodicNormalization(Standard_Real&  Parameter) const 
{
  Standard_Real Period ;
  
  if (periodic){
    Period = flatknots->Value(flatknots->Upper() - deg) - 
      flatknots->Value (deg + 1) ;
    while (Parameter > flatknots->Value(flatknots->Upper()-deg)){
      Parameter -= Period ;
    }
    while (Parameter < flatknots->Value((deg + 1))){
      Parameter +=  Period ;
    }
  }
}


//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Law_BSpline::IsCN ( const Standard_Integer N) const
{
  Standard_RangeError_Raise_if
    (N < 0, "Law_BSpline::IsCN");

  switch (smooth) {
  case GeomAbs_CN : return Standard_True;
  case GeomAbs_C0 : return N <= 0;
  case GeomAbs_G1 : return N <= 0;
  case GeomAbs_C1 : return N <= 1;
  case GeomAbs_G2 : return N <= 1;
  case GeomAbs_C2 : return N <= 2;
  case GeomAbs_C3 : 
    return N <= 3 ? Standard_True :
           N <= deg - BSplCLib::MaxKnotMult (mults->Array1(), mults->Lower() + 1, mults->Upper() - 1);
  default:
    return Standard_False;
  }
}



//=======================================================================
//function : IsClosed
//purpose  : 
//=======================================================================

Standard_Boolean Law_BSpline::IsClosed () const
{ return (Abs(StartPoint()-EndPoint())) <= gp::Resolution (); }



//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================

Standard_Boolean Law_BSpline::IsPeriodic () const
{ return periodic; }

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Law_BSpline::Continuity () const
{ return smooth; }

//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer Law_BSpline::Degree () const
{ return deg; }


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::Value(const Standard_Real U)const 
{
  Standard_Real  P;
  D0(U,P);
  return P;
}


//=======================================================================
//function : D0
//purpose  : 
//=======================================================================

void Law_BSpline::D0 (const Standard_Real U, 
		      Standard_Real& P)  const 
{
  Standard_Real  NewU = U ;
  PeriodicNormalization(NewU) ;
  if (rational) {
    BSplCLib::D0(NewU,0,deg,periodic,POLES, &weights->Array1(),FKNOTS,FMULTS,P);
  }
  else {
    BSplCLib::D0(NewU,0,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,P);
  }
}




//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Law_BSpline::D1 (const Standard_Real U,
		      Standard_Real& P,
		      Standard_Real& V1) const
{
  Standard_Real  NewU = U ;
  PeriodicNormalization(NewU) ;
  if (rational) {
    BSplCLib::D1(NewU,0,deg,periodic,POLES, &weights->Array1(),FKNOTS,FMULTS,
		 P,V1) ;
  }
  else {
    BSplCLib::D1(NewU,0,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,
		 P,V1) ;
  }
}



//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Law_BSpline::D2(const Standard_Real U ,
		     Standard_Real& P ,
		     Standard_Real& V1,
		     Standard_Real& V2 ) const
{
  Standard_Real  NewU = U ;
  PeriodicNormalization(NewU) ;
  if (rational) {
    BSplCLib::D2(NewU,0,deg,periodic,POLES,&weights->Array1(),FKNOTS,FMULTS,
		 P, V1, V2) ;
  }
  else {
    BSplCLib::D2(NewU,0,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,
		 P, V1, V2) ;
  }
}



//=======================================================================
//function : D3
//purpose  : 
//=======================================================================

void Law_BSpline::D3(const Standard_Real U ,
		     Standard_Real& P ,
		     Standard_Real& V1,
		     Standard_Real& V2,
		     Standard_Real& V3 ) const
{
  Standard_Real  NewU = U ;
  PeriodicNormalization(NewU) ;
  if (rational) {
    BSplCLib::D3(NewU,0,deg,periodic,POLES,&weights->Array1(),FKNOTS,FMULTS,
		 P, V1, V2, V3) ;
  }
  else {
    BSplCLib::D3(NewU,0,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,
		 P, V1, V2, V3) ;
  }
}



//=======================================================================
//function : DN
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::DN(const Standard_Real    U,
			      const Standard_Integer N ) const
{
  Standard_Real V;
  if (rational) {
    BSplCLib::DN(U,N,0,deg,periodic,POLES,&weights->Array1(),FKNOTS,FMULTS,V);
  }
  else {
    BSplCLib::DN(U,N,0,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,V);
  }
  return V;
}



//=======================================================================
//function : EndPoint
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::EndPoint () const
{ 
  if (mults->Value (knots->Upper ()) == deg + 1) 
    return poles->Value (poles->Upper());
  else
    return Value(LastParameter());
}


//=======================================================================
//function : FirstUKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer Law_BSpline::FirstUKnotIndex () const
{ 
  if (periodic) return 1;
  else return BSplCLib::FirstUKnotIndex (deg, mults->Array1()); 
}


//=======================================================================
//function : FirstParameter
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::FirstParameter () const
{
  return flatknots->Value (deg+1); 
}


//=======================================================================
//function : Knot
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::Knot (const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if
    (Index < 1 || Index > knots->Length(), "Law_BSpline::Knot");
  return knots->Value (Index);
}



//=======================================================================
//function : KnotDistribution
//purpose  : 
//=======================================================================

GeomAbs_BSplKnotDistribution Law_BSpline::KnotDistribution () const
{ 
  return knotSet; 
}


//=======================================================================
//function : Knots
//purpose  : 
//=======================================================================

void Law_BSpline::Knots (TColStd_Array1OfReal& K) const
{
  Standard_DimensionError_Raise_if
    (K.Length() != knots->Length(), "Law_BSpline::Knots");
  K = knots->Array1();
}


//=======================================================================
//function : KnotSequence
//purpose  : 
//=======================================================================

void Law_BSpline::KnotSequence (TColStd_Array1OfReal& K) const
{
  Standard_DimensionError_Raise_if
    (K.Length() != flatknots->Length(), "Law_BSpline::KnotSequence");
  K = flatknots->Array1();
}



//=======================================================================
//function : LastUKnotIndex
//purpose  : 
//=======================================================================

Standard_Integer Law_BSpline::LastUKnotIndex() const
{
  if (periodic) return knots->Length();
  else return BSplCLib::LastUKnotIndex (deg, mults->Array1()); 
}


//=======================================================================
//function : LastParameter
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::LastParameter () const
{
  return flatknots->Value (flatknots->Upper()-deg); 
}


//=======================================================================
//function : LocalValue
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::LocalValue
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2)   const
{
  Standard_Real P;
  LocalD0(U,FromK1,ToK2,P);
  return P;
}

//=======================================================================
//function : LocalD0
//purpose  : 
//=======================================================================

void  Law_BSpline::LocalD0
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2,
 Standard_Real& P)   const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Law_BSpline::LocalValue");
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic,FromK1,ToK2, index,u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  if (rational) {
    BSplCLib::D0(u,index,deg,periodic,POLES,&weights->Array1(),FKNOTS,FMULTS,P);
  }
  else {
    BSplCLib::D0(u,index,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,P);
  }
}

//=======================================================================
//function : LocalD1
//purpose  : 
//=======================================================================

void Law_BSpline::LocalD1 (const Standard_Real    U,
			   const Standard_Integer FromK1,
			   const Standard_Integer ToK2,
			   Standard_Real&    P, 
			   Standard_Real&    V1)    const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Law_BSpline::LocalD1");
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1,ToK2, index, u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  if (rational) {
    BSplCLib::D1(u,index,deg,periodic,POLES,&weights->Array1(),FKNOTS,FMULTS,P,V1);
  }
  else {
    BSplCLib::D1(u,index,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,P,V1);
  }
}


//=======================================================================
//function : LocalD2
//purpose  : 
//=======================================================================

void Law_BSpline::LocalD2
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2, 
 Standard_Real&    P,
 Standard_Real&    V1,
 Standard_Real&    V2) const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Law_BSpline::LocalD2");
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1,ToK2, index, u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  if (rational) {
    BSplCLib::D2(u,index,deg,periodic,POLES, &weights->Array1(),FKNOTS,FMULTS,P,V1,V2);
  }
  else {
    BSplCLib::D2(u,index,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,P,V1,V2);
  }
}


//=======================================================================
//function : LocalD3
//purpose  : 
//=======================================================================

void Law_BSpline::LocalD3
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2, 
 Standard_Real&    P,
 Standard_Real&    V1,
 Standard_Real&    V2,
 Standard_Real&    V3) const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Law_BSpline::LocalD3");
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1,ToK2, index, u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  if (rational) {
    BSplCLib::D3(u,index,deg,periodic,POLES,&weights->Array1(),FKNOTS,FMULTS,P,V1,V2,V3);
  }
  else {
    BSplCLib::D3(u,index,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,P,V1,V2,V3);
  }
}



//=======================================================================
//function : LocalDN
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::LocalDN
(const Standard_Real    U,
 const Standard_Integer FromK1,
 const Standard_Integer ToK2,
 const Standard_Integer N      ) const
{
  Standard_DomainError_Raise_if (FromK1 == ToK2,
				 "Law_BSpline::LocalD3");
  Standard_Real u = U;
  Standard_Integer index = 0;
  BSplCLib::LocateParameter(deg, FKNOTS, U, periodic, FromK1,ToK2, index, u);
  index = BSplCLib::FlatIndex(deg,index,mults->Array1(),periodic);
  
  Standard_Real V;
  if (rational) {
    BSplCLib::DN(u,N,index,deg,periodic,POLES,&weights->Array1(),FKNOTS,FMULTS,V);
  }
  else {
    BSplCLib::DN(u,N,index,deg,periodic,POLES,BSplCLib::NoWeights(),FKNOTS,FMULTS,V);
  }
  return V;
}



//=======================================================================
//function : Multiplicity
//purpose  : 
//=======================================================================

Standard_Integer Law_BSpline::Multiplicity 
(const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if (Index < 1 || Index > mults->Length(),
				"Law_BSpline::Multiplicity");
  return mults->Value (Index);
}


//=======================================================================
//function : Multiplicities
//purpose  : 
//=======================================================================

void Law_BSpline::Multiplicities (TColStd_Array1OfInteger& M) const
{
  Standard_DimensionError_Raise_if (M.Length() != mults->Length(),
				    "Law_BSpline::Multiplicities");
  M = mults->Array1();
}


//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer Law_BSpline::NbKnots () const
{ return knots->Length(); }

//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer Law_BSpline::NbPoles () const
{ return poles->Length(); }


//=======================================================================
//function : Pole
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::Pole (const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if (Index < 1 || Index > poles->Length(),
				"Law_BSpline::Pole");
  return poles->Value (Index);      
}


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void Law_BSpline::Poles (TColStd_Array1OfReal& P) const
{
  Standard_DimensionError_Raise_if (P.Length() != poles->Length(),
				    "Law_BSpline::Poles");
  P = poles->Array1();
}


//=======================================================================
//function : StartPoint
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::StartPoint () const
{
  if (mults->Value (1) == deg + 1)  
    return poles->Value (1);
  else 
    return Value(FirstParameter());
}

//=======================================================================
//function : Weight
//purpose  : 
//=======================================================================

Standard_Real Law_BSpline::Weight
(const Standard_Integer Index) const
{
  Standard_OutOfRange_Raise_if (Index < 1 || Index > poles->Length(),
				"Law_BSpline::Weight");
  if (IsRational())
    return weights->Value (Index);
  else
    return 1.;
}



//=======================================================================
//function : Weights
//purpose  : 
//=======================================================================

void Law_BSpline::Weights
(TColStd_Array1OfReal& W) const
{
  Standard_DimensionError_Raise_if (W.Length() != poles->Length(),
				    "Law_BSpline::Weights");
  if (IsRational())
    W = weights->Array1();
  else {
    Standard_Integer i;
    for (i = W.Lower(); i <= W.Upper(); i++)
      W(i) = 1.;
  }
}



//=======================================================================
//function : IsRational
//purpose  : 
//=======================================================================

Standard_Boolean Law_BSpline::IsRational () const
{ 
  return !weights.IsNull(); 
} 


//=======================================================================
//function : LocateU
//purpose  : 
//=======================================================================

void Law_BSpline::LocateU
(const Standard_Real    U, 
 const Standard_Real    ParametricTolerance, 
 Standard_Integer&      I1,
 Standard_Integer&      I2,
 const Standard_Boolean WithKnotRepetition) const
{
  Standard_Real NewU = U;
  Handle(TColStd_HArray1OfReal) TheKnots;
  if (WithKnotRepetition)  TheKnots = flatknots;
  else                     TheKnots = knots;
  
  PeriodicNormalization(NewU); //Attention a la periode

  const TColStd_Array1OfReal & CKnots = TheKnots->Array1();
  Standard_Real UFirst = CKnots (1);
  Standard_Real ULast  = CKnots (CKnots.Length());
  if (Abs (U - UFirst) <= Abs(ParametricTolerance)) { I1 = I2 = 1; }
  else if (Abs (U - ULast) <= Abs(ParametricTolerance)) { 
    I1 = I2 = CKnots.Length();
  }
  else if (NewU < UFirst - Abs(ParametricTolerance)) {
    I2 = 1;
    I1 = 0;
  }
  else if (NewU > ULast + Abs(ParametricTolerance)) {
    I1 = CKnots.Length();
    I2 = I1 + 1;
  }
  else {
    I1 = 1;
    BSplCLib::Hunt (CKnots, NewU, I1);
    while ( Abs( CKnots(I1+1) - NewU) <= Abs(ParametricTolerance)) I1++;
    if ( Abs( CKnots(I1) - NewU) <= Abs(ParametricTolerance)) {
      I2 = I1;
    }
    else {
      I2 = I1 + 1;
    }   
  }
}
//=======================================================================
//function : MovePointAndTangent
//purpose  : 
//=======================================================================

void Law_BSpline::
  MovePointAndTangent(const Standard_Real    U,
		      const Standard_Real    P,
		      const Standard_Real    Tangent,
		      const Standard_Real    Tolerance,
		      const Standard_Integer StartingCondition,
		      const Standard_Integer EndingCondition,
		      Standard_Integer&      ErrorStatus) 
{
  TColStd_Array1OfReal new_poles(1, poles->Length());
  Standard_Real delta,
  *poles_array,
  *new_poles_array,
  delta_derivative;
  const Standard_Integer dimension = 1 ;
  D1(U, 
     delta,
     delta_derivative) ;
  delta = P - delta  ;
  
  delta_derivative = Tangent - delta_derivative ;
  poles_array = (Standard_Real *)
    &poles->Array1()(1) ;
  new_poles_array = (Standard_Real *) 
    &new_poles(1) ;
  BSplCLib::MovePointAndTangent(U,
				dimension,
				delta,
				delta_derivative,
				Tolerance,
				deg,
				rational,
				StartingCondition,
				EndingCondition,
				poles_array[0],
				weights->Array1(), 
				flatknots->Array1(), 
				new_poles_array[0],
				ErrorStatus) ;
  if (!ErrorStatus) {
    poles->ChangeArray1() = new_poles;
  }
}
	

//=======================================================================
//function : Resolution
//purpose  : 
//=======================================================================

void Law_BSpline::Resolution(const Standard_Real Tolerance3D,
			     Standard_Real &     UTolerance) const 
{
  void* bid = (void*)(&(poles->Value(1)));
  Standard_Real* bidr = (Standard_Real*)bid;
  if (rational) {
    BSplCLib::Resolution(*bidr,1,poles->Length(),
			 &weights->Array1(),FKNOTS,deg,
			 Tolerance3D,
			 UTolerance) ;
  }
  else {

    BSplCLib::Resolution(*bidr,1,poles->Length(),
			 BSplCLib::NoWeights(),FKNOTS,deg,
			 Tolerance3D,
			 UTolerance) ;
  } 
}
