// Created on: 1995-11-20
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


#include <Adaptor3d_Curve.hxx>
#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <Law.hxx>
#include <Law_BSpFunc.hxx>
#include <Law_BSpline.hxx>
#include <Law_Interpolate.hxx>
#include <Law_Linear.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>

Handle(Law_BSpFunc) Law::MixBnd(const Handle(Law_Linear)& Lin)
{
  Standard_Real f,l;
  Lin->Bounds(f,l);
  TColStd_Array1OfReal Knots(1,4);
  TColStd_Array1OfInteger Mults(1,4);
  Knots(1) = f; Knots(4) = l; 
  Knots(2) = 0.75*f + 0.25*l; Knots(3) = 0.25*f + 0.75*l;
  Mults(1) = Mults(4) = 4;
  Mults(2) = Mults(3) = 1;
  Handle(TColStd_HArray1OfReal) pol = Law::MixBnd(3,Knots,Mults,Lin);
  Handle(Law_BSpline) bs = new Law_BSpline(pol->Array1(),Knots,Mults,3);
  Handle(Law_BSpFunc) bsf = new Law_BSpFunc();
  bsf->SetCurve(bs);
  return bsf;
}

Handle(TColStd_HArray1OfReal) Law::MixBnd
(const Standard_Integer         Degree,
 const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger& Mults,
 const Handle(Law_Linear)&      Lin)
{
  Standard_Integer nbpol = 0, nbfk = 0, i, j, k = 0;
  for(i = Mults.Lower(); i <= Mults.Upper(); i++){
    nbfk += Mults(i);
  }
  TColStd_Array1OfReal fk(1,nbfk);
  for(i = Mults.Lower(); i <= Mults.Upper(); i++){
    for(j = 1; j <= Mults(i); j++){
      fk(++k) = Knots(i);
    }
  }
  nbpol = nbfk - Degree - 1;
  TColStd_Array1OfReal par(1,nbpol);
  BSplCLib::BuildSchoenbergPoints(Degree,fk,par);
  Handle(TColStd_HArray1OfReal) res = new TColStd_HArray1OfReal(1,nbpol);
  TColStd_Array1OfReal& pol = res->ChangeArray1();
  for(i = 1; i <= nbpol; i++){
    pol(i) = Lin->Value(par(i));
  }
  TColStd_Array1OfInteger ord(1,nbpol);
  ord.Init(0);
  BSplCLib::Interpolate(Degree,fk,par,ord,1,pol(1),i);
  if(nbpol >= 4){
    pol(2) = pol(1);
    pol(nbpol - 1) = pol(nbpol);
  }
  return res;
}

static Standard_Real eval1(const Standard_Real    p,
			   const Standard_Real    first,
			   const Standard_Real    last,
			   const Standard_Real    piv,
			   const Standard_Boolean nulr)
{
  if((nulr && p >= piv) || (!nulr && p <= piv)) return 0.;
  else if(nulr){
    Standard_Real a = piv - first;
    a *= a;
    a = 1./a;
    Standard_Real b = p - first;
    a *= b;
    b = piv - p;
    a *= b;
    a *= b;
    return a;
  }
  else{
    Standard_Real a = last - piv;
    a *= a;
    a = 1./a;
    Standard_Real b = last - p;
    a *= b;
    b = p - piv;
    a *= b;
    a *= b;
    return a;
  }
}

Handle(TColStd_HArray1OfReal) Law::MixTgt
(const Standard_Integer         Degree,
 const TColStd_Array1OfReal&    Knots,
 const TColStd_Array1OfInteger& Mults,
 const Standard_Boolean         NulOnTheRight,
 const Standard_Integer         Index)
{
  Standard_Real first = Knots(Knots.Lower());
  Standard_Real last = Knots(Knots.Upper());
  Standard_Real piv = Knots(Index);
  Standard_Integer nbpol = 0, nbfk = 0, i, j, k = 0;
  for(i = Mults.Lower(); i <= Mults.Upper(); i++){
    nbfk += Mults(i);
  }
  TColStd_Array1OfReal fk(1,nbfk);
  for(i = Mults.Lower(); i <= Mults.Upper(); i++){
    for(j = 1; j <= Mults(i); j++){
      fk(++k) = Knots(i);
    }
  }
  nbpol = nbfk - Degree - 1;
  TColStd_Array1OfReal par(1,nbpol);
  BSplCLib::BuildSchoenbergPoints(Degree,fk,par);
  Handle(TColStd_HArray1OfReal) res = new TColStd_HArray1OfReal(1,nbpol);
  TColStd_Array1OfReal& pol = res->ChangeArray1();
  for(i = 1; i <= nbpol; i++){
    pol(i) = eval1(par(i),first,last,piv,NulOnTheRight);
  }
  TColStd_Array1OfInteger ord(1,nbpol);
  ord.Init(0);
  BSplCLib::Interpolate(Degree,fk,par,ord,1,pol(1),i);
  return res;
}

Handle(Law_BSpline) Law::Reparametrize(const Adaptor3d_Curve&   Curve,
				       const Standard_Real    First,
				       const Standard_Real    Last,
				       const Standard_Boolean HasDF,
				       const Standard_Boolean HasDL,
				       const Standard_Real    DFirst,
				       const Standard_Real    DLast,
				       const Standard_Boolean Rev,
				       const Standard_Integer NbPoints)
{
  // On evalue la longeur approximative de la courbe.
  
  Standard_Integer i;
  Standard_Real DDFirst = DFirst, DDLast = DLast;
  if(HasDF && Rev) DDFirst = -DFirst;
  if(HasDL && Rev) DDLast = -DLast;
  TColStd_Array1OfReal cumdist(1,2*NbPoints);
  TColStd_Array1OfReal ucourbe(1,2*NbPoints);
  gp_Pnt P1,P2;
  Standard_Real U1 = Curve.FirstParameter();
  Standard_Real U2 = Curve.LastParameter();
  Standard_Real U, DU, Length = 0.;
  if(!Rev){
    P1 = Curve.Value(U1);
    U = U1;
    DU = (U2 - U1) / ( 2*NbPoints - 1);
  }
  else{
    P1 = Curve.Value(U2);
    U = U2;
    DU = (U1 - U2) / ( 2*NbPoints - 1);
  }
  for ( i = 1; i <= 2*NbPoints ; i ++) {
    P2 = Curve.Value(U);
    Length += P2.Distance(P1);
    cumdist(i) = Length;
    ucourbe(i) = U;
    U += DU;
    P1 = P2;
  }
  if(Rev) ucourbe(2*NbPoints) = U1;
  else ucourbe(2*NbPoints) = U2;

  Handle(TColStd_HArray1OfReal) point = new TColStd_HArray1OfReal(1,NbPoints);
  Handle(TColStd_HArray1OfReal) param = new TColStd_HArray1OfReal(1,NbPoints);

  Standard_Real DCorde = Length / ( NbPoints - 1); 
  Standard_Real Corde  = DCorde;
  Standard_Integer Index = 1;
  Standard_Real Alpha;
  Standard_Real fac = 1./(NbPoints-1);
  
  point->SetValue(1,ucourbe(1));
  param->SetValue(1,First);
  point->SetValue(NbPoints,ucourbe(2*NbPoints));
  param->SetValue(NbPoints,Last);

  for ( i = 2; i < NbPoints; i++) {
    while (cumdist(Index) < Corde) Index ++;
    
    Alpha = (Corde - cumdist(Index - 1)) / (cumdist(Index) -cumdist(Index - 1));
    U = ucourbe(Index-1) + Alpha * (ucourbe(Index) - ucourbe(Index-1));
    point->SetValue(i,U);
    param->SetValue(i,((NbPoints-i)*First+(i-1)*Last)*fac);
    Corde = i*DCorde;
  }
  Law_Interpolate inter(point,param,0,1.e-9);
  if(HasDF || HasDL){
    TColStd_Array1OfReal tgt(1,NbPoints);
    Handle(TColStd_HArray1OfBoolean) 
      flag = new TColStd_HArray1OfBoolean(1,NbPoints);
    flag->ChangeArray1().Init(0);
    if(HasDF){ flag->SetValue(1,1); tgt.SetValue(1,DDFirst); }
    if(HasDL){ flag->SetValue(NbPoints,1); tgt.SetValue(NbPoints,DDLast); }
    inter.Load(tgt,flag);
  }
  inter.Perform();
  if(!inter.IsDone()) 
    throw Standard_Failure("Law::Reparametrize echec interpolation");
  Handle(Law_BSpline) bs = inter.Curve();
  return bs;
}

static Standard_Real eval2(const Standard_Real        p,
//			   const Standard_Real        first,
			   const Standard_Real        ,
//			   const Standard_Real        last,
			   const Standard_Real        ,
			   const Standard_Real        mil,
			   const Standard_Boolean     hasfirst,
			   const Standard_Boolean     haslast,
			   const Handle(Law_BSpline)& bs1,
			   const Handle(Law_BSpline)& bs2)
{
  if(hasfirst && p < mil) return bs1->Value(p);
  else if(haslast && p > mil) return bs2->Value(p);
  else return 1.;
}

Handle(Law_BSpline) Law::Scale(const Standard_Real    First,
			       const Standard_Real    Last,
			       const Standard_Boolean HasF,
			       const Standard_Boolean HasL,
			       const Standard_Real    VFirst,
			       const Standard_Real    VLast)
{
  Standard_Integer i;
  Standard_Real Milieu = 0.5*(First+Last);
  TColStd_Array1OfReal   knot(1,3);
  TColStd_Array1OfReal   fknot(1,10);
  TColStd_Array1OfInteger mult(1,3);
  knot(1) = First; knot(2) = Milieu; knot(3) = Last;
  fknot(1) = fknot(2) = fknot(3) = fknot(4) = First; 
  fknot(10) = fknot(9) = fknot(8) = fknot(7) = Last;
  fknot(5) = fknot(6) =  Milieu;
  mult(1) = 4; mult(3) = 4; mult(2) = 2;

  TColStd_Array1OfReal pbs(1,4);
  TColStd_Array1OfReal kbs(1,2);
  TColStd_Array1OfInteger mbs(1,2);
  mbs(1) = mbs(2) = 4;
  Handle(Law_BSpline) bs1,bs2;
  if(HasF){
    pbs(1) = pbs(2) = VFirst;
    pbs(3) = pbs(4) = 1.;
    kbs(1) = First; kbs(2) = Milieu;
    bs1 = new Law_BSpline(pbs,kbs,mbs,3);
  }
  if(HasL){
    pbs(1) = pbs(2) = 1.;
    pbs(3) = pbs(4) = VLast;
    kbs(1) = Milieu; kbs(2) = Last;
    bs2 = new Law_BSpline(pbs,kbs,mbs,3);
  }

  TColStd_Array1OfReal pol(1,6);
  TColStd_Array1OfReal par(1,6);
  BSplCLib::BuildSchoenbergPoints(3,fknot,par);
  for(i = 1; i <= 6; i++){
    pol(i) = eval2(par(i),First,Last,Milieu,HasF,HasL,bs1,bs2);
  }
  TColStd_Array1OfInteger ord(1,6);
  ord.Init(0);
  BSplCLib::Interpolate(3,fknot,par,ord,1,pol(1),i);
  bs1 = new Law_BSpline(pol,knot,mult,3);
  return bs1;
}


Handle(Law_BSpline) Law::ScaleCub(const Standard_Real    First,
				  const Standard_Real    Last,
				  const Standard_Boolean HasF,
				  const Standard_Boolean HasL,
				  const Standard_Real    VFirst,
				  const Standard_Real    VLast)
{
//Standard_Integer i;
  Standard_Real Milieu = 0.5*(First+Last);
  TColStd_Array1OfReal pol(1,5);
  TColStd_Array1OfReal   knot(1,3);
  TColStd_Array1OfInteger mult(1,3);
  knot(1) = First; knot(2) = Milieu; knot(3) = Last;
  mult(1) = 4; mult(3) = 4; mult(2) = 1;

  Handle(Law_BSpline) bs;
  if(HasF){ pol(1) = pol(2) = VFirst;}
  else { pol(1) = pol(2) = 1.; }
  if(HasL){ pol(4) = pol(5) = VLast;}
  else { pol(4) = pol(5) = 1.; }

  pol(3) = 1.;

  bs = new Law_BSpline(pol,knot,mult,3);
  return bs;
}
