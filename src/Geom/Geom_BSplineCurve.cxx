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

//Avril 1991 : constructeurs + methodes de lecture.
//Mai 1991   : revue des specifs + debut de realisation des classes tool =>
//             implementation des methodes Set et calcul du point courant.
//Juillet 1991 : voir egalement File Geom_BSplineCurve_1.cxx
//Juin    1992 : mise a plat des valeurs nodales - amelioration des
//               performances sur calcul du point courant

//RLE Aug 1993  Remove Swaps, Remove typedefs, Update BSplCLib
//              debug periodic, IncreaseDegree
// 21-Mar-95 : xab implemented cache
// 14-Mar-96 : xab implemented MovePointAndTangent 
// 13-Oct-96 : pmn Bug dans SetPeriodic (PRO6088) et Segment (PRO6250)

#define No_Standard_OutOfRange


#include <BSplCLib.hxx>
#include <BSplCLib_KnotDistribution.hxx>
#include <BSplCLib_MultDistribution.hxx>
#include <ElCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Geometry.hxx>
#include <Geom_UndefinedDerivative.hxx>
#include <gp.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Real.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_BSplineCurve,Geom_BoundedCurve)

//=======================================================================
//function : CheckCurveData
//purpose  : Internal use only
//=======================================================================
static void CheckCurveData
(const TColgp_Array1OfPnt&         CPoles,
 const TColStd_Array1OfReal&       CKnots,
 const TColStd_Array1OfInteger&    CMults,
 const Standard_Integer            Degree,
 const Standard_Boolean            Periodic)
{
  if (Degree < 1 || Degree > Geom_BSplineCurve::MaxDegree()) {
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

Handle(Geom_Geometry) Geom_BSplineCurve::Copy() const
{
  Handle(Geom_BSplineCurve) C;
  if (IsRational()) 
    C = new Geom_BSplineCurve(poles->Array1(),
			      weights->Array1(),
			      knots->Array1(),
			      mults->Array1(),
			      deg,periodic);
  else
    C = new Geom_BSplineCurve(poles->Array1(),
			      knots->Array1(),
			      mults->Array1(),
			      deg,periodic);
  return C;
}

//=======================================================================
//function : Geom_BSplineCurve
//purpose  : 
//=======================================================================

Geom_BSplineCurve::Geom_BSplineCurve
(const TColgp_Array1OfPnt&       Poles,
 const TColStd_Array1OfReal&     Knots,
 const TColStd_Array1OfInteger&  Mults,
 const Standard_Integer          Degree,
 const Standard_Boolean          Periodic) :
 rational(Standard_False),
 periodic(Periodic),
 deg(Degree),
 maxderivinvok(Standard_False)
{
  // check
  
  CheckCurveData(Poles,
                 Knots,
                 Mults,
                 Degree,
                 Periodic);

  // copy arrays

  poles =  new TColgp_HArray1OfPnt(1,Poles.Length());
  poles->ChangeArray1() = Poles;
  

  knots = new TColStd_HArray1OfReal(1,Knots.Length());
  knots->ChangeArray1() = Knots;

  mults = new TColStd_HArray1OfInteger(1,Mults.Length());
  mults->ChangeArray1() = Mults;

  UpdateKnots();
}

//=======================================================================
//function : Geom_BSplineCurve
//purpose  : 
//=======================================================================

Geom_BSplineCurve::Geom_BSplineCurve
(const TColgp_Array1OfPnt&      Poles,
 const TColStd_Array1OfReal&    Weights,
 const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger& Mults,
 const Standard_Integer         Degree,
 const Standard_Boolean         Periodic,
 const Standard_Boolean         CheckRational)  :
 rational(Standard_True),
 periodic(Periodic),
 deg(Degree),
 maxderivinvok(Standard_False)

{

  // check
  
  CheckCurveData(Poles,
                 Knots,
                 Mults,
                 Degree,
                 Periodic);

  if (Weights.Length() != Poles.Length())
    throw Standard_ConstructionError("Geom_BSplineCurve");

  Standard_Integer i;
  for (i = Weights.Lower(); i <= Weights.Upper(); i++) {
    if (Weights(i) <= gp::Resolution())  
      throw Standard_ConstructionError("Geom_BSplineCurve");
  }
  
  // check really rational
  if (CheckRational)
    rational = Rational(Weights);
  
  // copy arrays
  
  poles =  new TColgp_HArray1OfPnt(1,Poles.Length());
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

Standard_Integer Geom_BSplineCurve::MaxDegree () 
{ 
  return BSplCLib::MaxDegree(); 
}

//=======================================================================
//function : IncreaseDegree
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::IncreaseDegree  (const Standard_Integer Degree)
{
  if (Degree == deg) return;
  
  if (Degree < deg || Degree > Geom_BSplineCurve::MaxDegree()) {
    throw Standard_ConstructionError();
  }
  Standard_Integer FromK1 = FirstUKnotIndex ();
  Standard_Integer ToK2   = LastUKnotIndex  ();
  
  Standard_Integer Step   = Degree - deg;
  
  Handle(TColgp_HArray1OfPnt) npoles = new
    TColgp_HArray1OfPnt(1,poles->Length() + Step * (ToK2-FromK1));

  Standard_Integer nbknots = BSplCLib::IncreaseDegreeCountKnots
    (deg,Degree,periodic,mults->Array1());

  Handle(TColStd_HArray1OfReal) nknots = 
    new TColStd_HArray1OfReal(1,nbknots);

  Handle(TColStd_HArray1OfInteger) nmults = 
    new TColStd_HArray1OfInteger(1,nbknots);

  Handle(TColStd_HArray1OfReal) nweights;
  
  if (IsRational()) {
    
    nweights = new TColStd_HArray1OfReal(1,npoles->Upper());
    
    BSplCLib::IncreaseDegree
      (deg,Degree, periodic,
       poles->Array1(),&weights->Array1(),
       knots->Array1(),mults->Array1(),
       npoles->ChangeArray1(),&nweights->ChangeArray1(),
       nknots->ChangeArray1(),nmults->ChangeArray1());
  }
  else {
    BSplCLib::IncreaseDegree
      (deg,Degree, periodic,
       poles->Array1(),BSplCLib::NoWeights(),
       knots->Array1(),mults->Array1(),
       npoles->ChangeArray1(),
       BSplCLib::NoWeights(),
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

void Geom_BSplineCurve::IncreaseMultiplicity  (const Standard_Integer Index,
					       const Standard_Integer M)
{
  TColStd_Array1OfReal k(1,1);
  k(1) = knots->Value(Index);
  TColStd_Array1OfInteger m(1,1);
  m(1) = M - mults->Value(Index);
  InsertKnots(k,m,Epsilon(1.),Standard_True);
}

//=======================================================================
//function : IncreaseMultiplicity
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::IncreaseMultiplicity  (const Standard_Integer I1,
					       const Standard_Integer I2,
					       const Standard_Integer M)
{
  Handle(TColStd_HArray1OfReal)  tk = knots;
  TColStd_Array1OfReal k((knots->Array1())(I1),I1,I2);
  TColStd_Array1OfInteger m(I1,I2);
  Standard_Integer i;
  for (i = I1; i <= I2; i++)
    m(i) = M - mults->Value(i);
  InsertKnots(k,m,Epsilon(1.),Standard_True);
}

//=======================================================================
//function : IncrementMultiplicity
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::IncrementMultiplicity
(const Standard_Integer I1,
 const Standard_Integer I2,
 const Standard_Integer Step)
{
  Handle(TColStd_HArray1OfReal) tk = knots;
  TColStd_Array1OfReal    k((knots->Array1())(I1),I1,I2);
  TColStd_Array1OfInteger m(I1,I2) ;
  m.Init(Step);
  InsertKnots(k,m,Epsilon(1.),Standard_True);
}

//=======================================================================
//function : InsertKnot
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::InsertKnot
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

void  Geom_BSplineCurve::InsertKnots(const TColStd_Array1OfReal& Knots, 
				     const TColStd_Array1OfInteger& Mults,
				     const Standard_Real Epsilon,
				     const Standard_Boolean Add)
{
  // Check and compute new sizes
  Standard_Integer nbpoles,nbknots;

  if (!BSplCLib::PrepareInsertKnots(deg,periodic,
				    knots->Array1(),mults->Array1(),
				    Knots,&Mults,nbpoles,nbknots,Epsilon,Add))
    throw Standard_ConstructionError("Geom_BSplineCurve::InsertKnots");

  if (nbpoles == poles->Length()) return;

  Handle(TColgp_HArray1OfPnt)      npoles = new TColgp_HArray1OfPnt(1,nbpoles);
  Handle(TColStd_HArray1OfReal)    nknots = knots;
  Handle(TColStd_HArray1OfInteger) nmults = mults;

  if (nbknots != knots->Length()) {
    nknots = new TColStd_HArray1OfReal(1,nbknots);
    nmults = new TColStd_HArray1OfInteger(1,nbknots);
  }

  if (rational) {
    Handle(TColStd_HArray1OfReal) nweights = 
      new TColStd_HArray1OfReal(1,nbpoles);
    BSplCLib::InsertKnots(deg,periodic,
			  poles->Array1(), &weights->Array1(),
			  knots->Array1(), mults->Array1(),
			  Knots, &Mults,
			  npoles->ChangeArray1(), &nweights->ChangeArray1(),
			  nknots->ChangeArray1(), nmults->ChangeArray1(),
			  Epsilon, Add);
    weights = nweights;
  }
  else {
    BSplCLib::InsertKnots(deg,periodic,
			  poles->Array1(), BSplCLib::NoWeights(),
			  knots->Array1(), mults->Array1(),
			  Knots, &Mults,
			  npoles->ChangeArray1(),
        BSplCLib::NoWeights(),
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

Standard_Boolean  Geom_BSplineCurve::RemoveKnot(const Standard_Integer Index,
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
  
  const TColgp_Array1OfPnt   & oldpoles   = poles->Array1();

  Standard_Integer step = mults->Value(Index) - M;
  if (step <= 0) return Standard_True;

  Handle(TColgp_HArray1OfPnt) npoles =
    new TColgp_HArray1OfPnt(1,oldpoles.Length()-step);

  Handle(TColStd_HArray1OfReal)    nknots  = knots;
  Handle(TColStd_HArray1OfInteger) nmults  = mults;

  if (M == 0) {
    nknots = new TColStd_HArray1OfReal(1,knots->Length()-1);
    nmults = new TColStd_HArray1OfInteger(1,knots->Length()-1);
  }

  if (IsRational()) {
    Handle(TColStd_HArray1OfReal) nweights = 
      new TColStd_HArray1OfReal(1,npoles->Length());
    if (!BSplCLib::RemoveKnot
	(Index, M, deg, periodic,
	 poles->Array1(),&weights->Array1(), 
	 knots->Array1(),mults->Array1(),
	 npoles->ChangeArray1(), &nweights->ChangeArray1(),
	 nknots->ChangeArray1(),nmults->ChangeArray1(),
	 Tolerance))
      return Standard_False;
    weights = nweights;
  }
  else {
    if (!BSplCLib::RemoveKnot
	(Index, M, deg, periodic,
	 poles->Array1(), BSplCLib::NoWeights(),
	 knots->Array1(),mults->Array1(),
	 npoles->ChangeArray1(),
   BSplCLib::NoWeights(),
	 nknots->ChangeArray1(),nmults->ChangeArray1(),
	 Tolerance))
      return Standard_False;
  }
  
  poles = npoles;
  knots = nknots;
  mults = nmults;

  UpdateKnots();
  maxderivinvok = 0;
  return Standard_True;
}

//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::Reverse ()
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

Standard_Real Geom_BSplineCurve::ReversedParameter
(const Standard_Real U) const
{
  return (FirstParameter() + LastParameter() - U);
}

//=======================================================================
//function : Segment
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::Segment(const Standard_Real U1,
				const Standard_Real U2)
{
  if (U2 < U1)
    throw Standard_DomainError("Geom_BSplineCurve::Segment");
  
  Standard_Real NewU1, NewU2;
  Standard_Real U,DU=0,aDDU=0;
  Standard_Integer index;
  Standard_Boolean wasPeriodic = periodic;

  TColStd_Array1OfReal    Knots(1,2);
  TColStd_Array1OfInteger Mults(1,2);

  // define param ditance to keep (eap, Apr 18 2002, occ311)
  if (periodic) {
    Standard_Real Period = LastParameter() - FirstParameter();
    DU = U2 - U1;
    if (DU - Period > Precision::PConfusion())
      throw Standard_DomainError("Geom_BSplineCurve::Segment");
    if (DU > Period)
      DU = Period;
    aDDU = DU;
  }

  index = 0;
  BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			    U1,periodic,knots->Lower(),knots->Upper(),
			    index,NewU1);
  index = 0;
  BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			    U2,periodic,knots->Lower(),knots->Upper(),
			    index,NewU2);

  //-- DBB
  Standard_Real aNu2 = NewU2;
  //-- DBB


  Knots( 1) = Min( NewU1, NewU2);
  Knots( 2) = Max( NewU1, NewU2);
  Mults( 1) = Mults( 2) = deg;

  Standard_Real AbsUMax = Max(Abs(NewU1),Abs(NewU2));

//  Modified by Sergey KHROMOV - Fri Apr 11 12:15:40 2003 Begin
  AbsUMax = Max(AbsUMax, Max(Abs(FirstParameter()),Abs(LastParameter())));
//  Modified by Sergey KHROMOV - Fri Apr 11 12:15:40 2003 End

  Standard_Real Eps = 100. * Epsilon(AbsUMax);

  InsertKnots( Knots, Mults, Eps);

  if (periodic) { // set the origine at NewU1
    index = 0;
    BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			      U1,periodic,knots->Lower(),knots->Upper(),
			      index,U);
    // Test si l'insertion est Ok et decalage sinon. 
    if ( Abs(knots->Value(index+1)-U) <= Eps) // <= pour etre homogene a InsertKnots
      index++;
    SetOrigin(index);
    SetNotPeriodic();
    NewU2 = NewU1 + DU;
  }

  // compute index1 and index2 to set the new knots and mults 
  Standard_Integer index1 = 0, index2 = 0;
  Standard_Integer FromU1 = knots->Lower();
  Standard_Integer ToU2   = knots->Upper();
  BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			    NewU1,periodic,FromU1,ToU2,index1,U);
  if ( Abs(knots->Value(index1+1)-U) <= Eps)
    index1++;

  BSplCLib::LocateParameter(deg,knots->Array1(),mults->Array1(),
			    NewU2,periodic,FromU1,ToU2,index2,U);
  if ( Abs(knots->Value(index2+1)-U) <= Eps || index2 == index1)
    index2++;
  
  Standard_Integer nbknots = index2 - index1 + 1;

  Handle(TColStd_HArray1OfReal) 
    nknots = new TColStd_HArray1OfReal(1,nbknots);
  Handle(TColStd_HArray1OfInteger) 
    nmults = new TColStd_HArray1OfInteger(1,nbknots);

  // to restore changed U1
  if (DU > 0) // if was periodic
    DU = NewU1 - U1;
  
  Standard_Integer i , k = 1;
  for ( i = index1; i<= index2; i++) {
    nknots->SetValue(k, knots->Value(i) - DU);
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
  Handle(TColgp_HArray1OfPnt)
    npoles = new TColgp_HArray1OfPnt(1,nbpoles);

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

  //-- DBB
  if( wasPeriodic ) {
    nknots->ChangeValue(nknots->Lower()) = U1;
    if( aNu2 < U2 ) {
      nknots->ChangeValue(nknots->Upper()) = U1 + aDDU;
    }
  }
  //-- DBB

  knots = nknots;
  mults = nmults;
  poles = npoles;
  if ( rational) 
    weights = nweights;

  maxderivinvok = 0;
  UpdateKnots();
}

//=======================================================================
//function : SetKnot
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetKnot
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
    maxderivinvok = 0;
    UpdateKnots();
  }
}

//=======================================================================
//function : SetKnots
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetKnots
(const TColStd_Array1OfReal& K)
{
  CheckCurveData(poles->Array1(),K,mults->Array1(),deg,periodic);
  knots->ChangeArray1() = K;
  maxderivinvok = 0;
  UpdateKnots();
}

//=======================================================================
//function : SetKnot
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetKnot
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

void Geom_BSplineCurve::SetPeriodic ()
{
  Standard_Integer first = FirstUKnotIndex();
  Standard_Integer last  = LastUKnotIndex();

  Handle(TColStd_HArray1OfReal) tk = knots;
  TColStd_Array1OfReal    cknots((knots->Array1())(first),first,last);
  knots = new TColStd_HArray1OfReal(1,cknots.Length());
  knots->ChangeArray1() = cknots;

  Handle(TColStd_HArray1OfInteger) tm = mults;
  TColStd_Array1OfInteger cmults((mults->Array1())(first),first,last);
  cmults(first) = cmults(last) = Min(deg, Max( cmults(first), cmults(last)));
  mults = new TColStd_HArray1OfInteger(1,cmults.Length());
  mults->ChangeArray1() = cmults;

  // compute new number of poles;
  Standard_Integer nbp = BSplCLib::NbPoles(deg,Standard_True,cmults);
  
  Handle(TColgp_HArray1OfPnt) tp = poles;
  TColgp_Array1OfPnt cpoles((poles->Array1())(1),1,nbp);
  poles = new TColgp_HArray1OfPnt(1,nbp);
  poles->ChangeArray1() = cpoles;
  
  if (rational) {
    Handle(TColStd_HArray1OfReal) tw = weights;
    TColStd_Array1OfReal cweights((weights->Array1())(1),1,nbp);
    weights = new TColStd_HArray1OfReal(1,nbp);
    weights->ChangeArray1() = cweights;
  }

  periodic = Standard_True;
  
  maxderivinvok = 0;
  UpdateKnots();
}

//=======================================================================
//function : SetOrigin
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetOrigin(const Standard_Integer Index)
{
  if (!periodic)
    throw Standard_NoSuchObject("Geom_BSplineCurve::SetOrigin");

  Standard_Integer i,k;
  Standard_Integer first = FirstUKnotIndex();
  Standard_Integer last  = LastUKnotIndex();

  if ((Index < first) || (Index > last))
    throw Standard_DomainError("Geom_BSplineCurve::SetOrigin");

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
  Handle(TColgp_HArray1OfPnt) npoles =
    new TColgp_HArray1OfPnt(1,nbpoles);
  Handle(TColStd_HArray1OfReal) nweights =
    new TColStd_HArray1OfReal(1,nbpoles);
  TColgp_Array1OfPnt   & newpoles   = npoles->ChangeArray1();
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
  maxderivinvok = 0;
  UpdateKnots();
}

//=======================================================================
//function : SetOrigin
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetOrigin(const Standard_Real U,
				  const Standard_Real Tol)
{
  if (!periodic)
    throw Standard_NoSuchObject("Geom_BSplineCurve::SetOrigin");
  //U est il dans la period.
  Standard_Real uf = FirstParameter(), ul = LastParameter();
  Standard_Real u = U, period = ul - uf;
  while (Tol < (uf-u)) u += period;
  while (Tol > (ul-u)) u -= period;

  if(Abs(U-u)>Tol) { //On reparametre la courbe
    Standard_Real delta = U-u;
    uf += delta;
    ul += delta;
    TColStd_Array1OfReal& kn = knots->ChangeArray1();
    Standard_Integer fk = kn.Lower(), lk = kn.Upper();
    for(Standard_Integer i = fk; i <= lk; i++){
      kn.ChangeValue(i) += delta;
    }
    UpdateKnots();
  }
  if(Abs(U-uf)<Tol) return;
  
  TColStd_Array1OfReal& kn = knots->ChangeArray1();
  Standard_Integer fk = kn.Lower(), lk = kn.Upper(),ik=0;
  Standard_Real delta = RealLast();
  for(Standard_Integer i = fk; i<= lk; i++){
    Standard_Real dki = kn.Value(i)-U;
    if(Abs(dki)<Abs(delta)){
      ik = i;
      delta = dki;
    }
  }
  if(Abs(delta)>Tol){
    InsertKnot(U);
    if(delta < 0.) ik++;
  }
  SetOrigin(ik);
}

//=======================================================================
//function : SetNotPeriodic
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetNotPeriodic () 
{ 
  if ( periodic) {
    Standard_Integer NbKnots, NbPoles;
    BSplCLib::PrepareUnperiodize( deg, mults->Array1(),NbKnots,NbPoles);
    
    Handle(TColgp_HArray1OfPnt) npoles 
      = new TColgp_HArray1OfPnt(1,NbPoles);
    
    Handle(TColStd_HArray1OfReal) nknots 
      = new TColStd_HArray1OfReal(1,NbKnots);
    
    Handle(TColStd_HArray1OfInteger) nmults
      = new TColStd_HArray1OfInteger(1,NbKnots);
    
    Handle(TColStd_HArray1OfReal) nweights;
    
    if (IsRational()) {
      
      nweights = new TColStd_HArray1OfReal(1,NbPoles);
      
      BSplCLib::Unperiodize
	(deg,mults->Array1(),knots->Array1(),poles->Array1(),
	 &weights->Array1(),nmults->ChangeArray1(),
	 nknots->ChangeArray1(),npoles->ChangeArray1(),
	 &nweights->ChangeArray1());
      
    }
    else {
      
      BSplCLib::Unperiodize
	(deg,mults->Array1(),knots->Array1(),poles->Array1(),
	 BSplCLib::NoWeights(),nmults->ChangeArray1(),
	 nknots->ChangeArray1(),npoles->ChangeArray1(),
   BSplCLib::NoWeights());
      
    }
    poles   = npoles;
    weights = nweights;
    mults   = nmults;
    knots   = nknots;
    periodic = Standard_False;
    
    maxderivinvok = 0;
    UpdateKnots();
  }
}

//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetPole
(const Standard_Integer Index,
 const gp_Pnt& P)
{
  if (Index < 1 || Index > poles->Length()) throw Standard_OutOfRange();
  poles->SetValue (Index, P);
  maxderivinvok = 0;
}

//=======================================================================
//function : SetPole
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetPole
(const Standard_Integer Index,
 const gp_Pnt& P,
 const Standard_Real W)
{
  SetPole(Index,P);
  SetWeight(Index,W);
}

//=======================================================================
//function : SetWeight
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::SetWeight
(const Standard_Integer Index,
 const Standard_Real W)
{
  if (Index < 1 || Index > poles->Length())   throw Standard_OutOfRange();

  if (W <= gp::Resolution ())     throw Standard_ConstructionError();


  Standard_Boolean rat = IsRational() || (Abs(W - 1.) > gp::Resolution());

  if ( rat) {
    if (rat && !IsRational()) {
      weights = new TColStd_HArray1OfReal(1,poles->Length());
      weights->Init(1.);
    }
    
    weights->SetValue (Index, W);
    
    if (IsRational()) {
      rat = Rational(weights->Array1());
      if (!rat) weights.Nullify();
    }
    
    rational = !weights.IsNull();
  }
  maxderivinvok = 0;
}

//=======================================================================
//function : MovePoint
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::MovePoint(const Standard_Real U,
                                  const gp_Pnt& P,
                                  const Standard_Integer Index1,
                                  const Standard_Integer Index2,
                                  Standard_Integer& FirstModifiedPole,
                                  Standard_Integer& LastmodifiedPole)
{
  if (Index1 < 1 || Index1 > poles->Length() || 
      Index2 < 1 || Index2 > poles->Length() || Index1 > Index2) {
    throw Standard_OutOfRange();
  }
  TColgp_Array1OfPnt npoles(1, poles->Length());
  gp_Pnt P0;
  D0(U, P0);
  gp_Vec Displ(P0, P);
  BSplCLib::MovePoint(U, Displ, Index1, Index2, deg, rational, poles->Array1(), 
                      weights->Array1(), flatknots->Array1(), 
                      FirstModifiedPole, LastmodifiedPole, npoles);
  if (FirstModifiedPole) {
    poles->ChangeArray1() = npoles;
    maxderivinvok = 0;
  }
}

//=======================================================================
//function : MovePointAndTangent
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::MovePointAndTangent(const Standard_Real    U,
                                            const gp_Pnt&          P,
                                            const gp_Vec&          Tangent,
                                            const Standard_Real    Tolerance,
                                            const Standard_Integer StartingCondition,
                                            const Standard_Integer EndingCondition,
                                            Standard_Integer&      ErrorStatus) 
{
  Standard_Integer ii ;
  if (IsPeriodic()) {
    //
    // for the time being do not deal with periodic curves
    //
    SetNotPeriodic() ;
  }
  TColgp_Array1OfPnt new_poles(1, poles->Length());
  gp_Pnt P0;


  gp_Vec delta_derivative;
  D1(U, P0,
     delta_derivative) ;
  gp_Vec delta(P0, P);
  for (ii = 1 ; ii <= 3 ; ii++) {
    delta_derivative.SetCoord(ii, Tangent.Coord(ii)-delta_derivative.Coord(ii));
  }
  BSplCLib::MovePointAndTangent(U,
                                delta,
                                delta_derivative,
                                Tolerance,
                                deg,
                                rational,
                                StartingCondition,
                                EndingCondition,
                                poles->Array1(), 
                                weights->Array1(), 
                                flatknots->Array1(), 
                                new_poles,
                                ErrorStatus) ;
  if (!ErrorStatus) {
    poles->ChangeArray1() = new_poles;
    maxderivinvok = 0;
  }
}

//=======================================================================
//function : UpdateKnots
//purpose  : 
//=======================================================================

void Geom_BSplineCurve::UpdateKnots()
{
  rational = !weights.IsNull();

  Standard_Integer MaxKnotMult = 0;
  BSplCLib::KnotAnalysis(deg, 
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

    BSplCLib::KnotSequence(knots->Array1(), 
                           mults->Array1(),
                           deg,periodic,
                           flatknots->ChangeArray1());
  }
  
  if (MaxKnotMult == 0)  smooth = GeomAbs_CN;
  else {
    switch (deg - MaxKnotMult) {
    case 0 :   smooth = GeomAbs_C0;   break;
    case 1 :   smooth = GeomAbs_C1;   break;
    case 2 :   smooth = GeomAbs_C2;   break;
    case 3 :   smooth = GeomAbs_C3;   break;
      default :  smooth = GeomAbs_C3;   break;
    }
  }
}

//=======================================================================
//function : Normalizes the parameters if the curve is periodic
//purpose  : that is compute the cache so that it is valid
//=======================================================================

void Geom_BSplineCurve::PeriodicNormalization(Standard_Real&  Parameter) const 
{
  Standard_Real Period ;

  if (periodic) {
    Period = flatknots->Value(flatknots->Upper() - deg) - flatknots->Value (deg + 1) ;
    while (Parameter > flatknots->Value(flatknots->Upper()-deg)) {
      Parameter -= Period ;
    }
    while (Parameter < flatknots->Value((deg + 1))) {
      Parameter +=  Period ;
    }
  }
}

