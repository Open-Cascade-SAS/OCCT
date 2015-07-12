// Created on: 1996-09-23
// Created by: Philippe MANGIN
// Copyright (c) 1996-1999 Matra Datavision
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

// Modified:	Fri Jul 10 11:23:35 1998
//              JCT : Add WithRatio,MinM

#include <Geom_BoundedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomConvert.hxx>
#include <GeomConvert_CompCurveToBSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

//=======================================================================
//function : constructor
//purpose  :
//=======================================================================
GeomConvert_CompCurveToBSplineCurve::GeomConvert_CompCurveToBSplineCurve (const Convert_ParameterisationType theParameterisation)
: myTol (Precision::Confusion()),
  myType (theParameterisation)
{
  //
}

//=======================================================================
//function : constructor
//purpose  :
//=======================================================================
GeomConvert_CompCurveToBSplineCurve::
GeomConvert_CompCurveToBSplineCurve(const Handle(Geom_BoundedCurve)& BasisCurve, 
				    const Convert_ParameterisationType Parameterisation) :
				    myTol(Precision::Confusion()),
				    myType(Parameterisation)
{
  Handle(Geom_BSplineCurve) Bs = 
      Handle(Geom_BSplineCurve)::DownCast(BasisCurve);
  if (!Bs.IsNull()) { 
    myCurve =  Handle(Geom_BSplineCurve)::DownCast(BasisCurve->Copy()); 
  }
  else {
    myCurve = GeomConvert::CurveToBSplineCurve (BasisCurve, myType);
  }
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================

Standard_Boolean GeomConvert_CompCurveToBSplineCurve::
Add(const Handle(Geom_BoundedCurve)& NewCurve,
    const Standard_Real Tolerance,
    const Standard_Boolean After,
    const Standard_Boolean WithRatio,
    const Standard_Integer MinM)
{
  // conversion
  Handle(Geom_BSplineCurve) Bs = Handle(Geom_BSplineCurve)::DownCast (NewCurve);
  if (!Bs.IsNull())
  {
    Bs = Handle(Geom_BSplineCurve)::DownCast (NewCurve->Copy());
  }
  else
  {
    Bs = GeomConvert::CurveToBSplineCurve (NewCurve, myType);
  }
  if (myCurve.IsNull())
  {
    myCurve = Bs;
    return Standard_True;
  }

  Standard_Boolean avant, apres;
  myTol = Tolerance;

  Standard_Integer LBs = Bs->NbPoles(), LCb = myCurve->NbPoles();

  avant = (( myCurve->Pole(1).Distance(Bs->Pole(1))  < myTol)||
	   ( myCurve->Pole(1).Distance(Bs->Pole(LBs))< myTol));
  apres = (( myCurve->Pole(LCb).Distance(Bs->Pole(1))  < myTol) ||
	   ( myCurve->Pole(LCb).Distance(Bs->Pole(LBs))< myTol));

  // myCurve est (sera) elle fermee ?
  if (avant && apres) { // On leve l'ambiguite
    if (After) avant = Standard_False;
    else       apres = Standard_False;
  }

  // Ajout Apres ?
  if ( apres) {
    if (myCurve->Pole(LCb).Distance(Bs->Pole(LBs)) < myTol) {Bs->Reverse();}
    Add(myCurve, Bs, Standard_True, WithRatio, MinM);
    return Standard_True;
  }
  // Ajout avant ?  
  else if (avant) {
    if (myCurve->Pole(1).Distance(Bs->Pole(1)) < myTol) {Bs->Reverse();}
    Add(Bs, myCurve, Standard_False, WithRatio, MinM);
    return Standard_True;
  }
  
  return Standard_False;
}

void GeomConvert_CompCurveToBSplineCurve::Add( 
      Handle(Geom_BSplineCurve)& FirstCurve, 
      Handle(Geom_BSplineCurve)& SecondCurve,
      const Standard_Boolean After,
      const Standard_Boolean WithRatio,
      const Standard_Integer MinM)
{
// Harmonisation des degres.
  Standard_Integer Deg = Max(FirstCurve->Degree(), SecondCurve->Degree());
  if (FirstCurve->Degree() < Deg) { FirstCurve->IncreaseDegree(Deg); }
  if (SecondCurve->Degree() < Deg)  { SecondCurve->IncreaseDegree(Deg); }

// Declarationd
  Standard_Real L1, L2, U_de_raccord;
  Standard_Integer ii, jj;
  Standard_Real  Ratio=1, Ratio1, Ratio2, Delta1, Delta2;
  Standard_Integer NbP1 = FirstCurve->NbPoles(), NbP2 = SecondCurve->NbPoles();
  Standard_Integer NbK1 = FirstCurve->NbKnots(), NbK2 = SecondCurve->NbKnots();
  TColStd_Array1OfReal Noeuds (1, NbK1+NbK2-1);
  TColgp_Array1OfPnt Poles (1, NbP1+ NbP2-1);
  TColStd_Array1OfReal Poids  (1, NbP1+ NbP2-1);
  TColStd_Array1OfInteger Mults (1, NbK1+NbK2-1);

  // Ratio de reparametrisation (C1 si possible)
  if (WithRatio) {
    L1 = FirstCurve->DN(FirstCurve->LastParameter(), 1).Magnitude();
    L2 = SecondCurve->DN(SecondCurve->FirstParameter(), 1). Magnitude();
    
    if ( (L1 > Precision::Confusion()) && (L2 > Precision::Confusion()) ) {
      Ratio = L1 / L2;
    }
    if ( (Ratio < Precision::Confusion()) || (Ratio > 1/Precision::Confusion()) ) {Ratio = 1;}
  }

  if (After) {
// On ne bouge pas la premiere courbe
    Ratio1 = 1;
    Delta1 = 0;
    Ratio2 = 1/Ratio;
    Delta2 = Ratio2*SecondCurve->Knot(1) - FirstCurve->Knot(NbK1);
    U_de_raccord = FirstCurve->LastParameter();
  }
  else {
// On ne bouge pas la seconde courbe
    Ratio1 = Ratio;
    Delta1 = Ratio1*FirstCurve->Knot(NbK1) - SecondCurve->Knot(1);
    Ratio2 = 1;
    Delta2 = 0;
    U_de_raccord = SecondCurve->FirstParameter();
  }    

// Les Noeuds
  Standard_Real eps;
  for (ii=1; ii<NbK1; ii++) {
    Noeuds(ii) = Ratio1*FirstCurve->Knot(ii) - Delta1;
    if(ii > 1) {
      eps = Epsilon (Abs(Noeuds(ii-1)));
      if( eps < 5.e-10 ) eps = 5.e-10;
      if(Noeuds(ii) - Noeuds(ii-1) <= eps) {
	Noeuds(ii) += eps;
      }
    }
    Mults(ii) = FirstCurve->Multiplicity(ii);
  }
  Noeuds(NbK1) = U_de_raccord;
  eps = Epsilon (Abs(Noeuds(NbK1-1)));
  if(Noeuds(NbK1) - Noeuds(NbK1-1) <= eps) {
    Noeuds(NbK1) += eps;
  }
  Mults(NbK1) = FirstCurve->Degree();
  for (ii=2, jj=NbK1+1; ii<=NbK2; ii++, jj++) {
    Noeuds(jj) = Ratio2*SecondCurve->Knot(ii) - Delta2;
    eps = Epsilon (Abs(Noeuds(jj-1)));
    if( eps < 5.e-10 ) eps = 5.e-10;
    if(Noeuds(jj) - Noeuds(jj-1) <= eps) {
      Noeuds(jj) += eps;
    }
    Mults(jj) = SecondCurve->Multiplicity(ii);
  }
  
  Ratio = FirstCurve->Weight(NbP1) ;
  Ratio /=  SecondCurve->Weight(1) ;
// Les Poles et Poids
  for (ii=1;  ii<NbP1; ii++) {
     Poles(ii) =  FirstCurve->Pole(ii);
     Poids(ii) =  FirstCurve->Weight(ii);
   }
  for (ii=1, jj=NbP1;  ii<=NbP2; ii++, jj++) {
     Poles(jj) =   SecondCurve->Pole(ii);
//
// attentiion les poids ne se raccord pas forcement C0
// d'ou Ratio
//
     Poids(jj) =   Ratio * SecondCurve->Weight(ii);
   }
  
// Creation de la BSpline
  myCurve = new (Geom_BSplineCurve) (Poles, Poids, Noeuds, Mults, Deg);

// Reduction eventuelle de la multiplicite jusqu'a MinM
  Standard_Boolean Ok = Standard_True;
  Standard_Integer M = Mults(NbK1);
  while ( (M>MinM) && Ok) {
    M--;
    Ok = myCurve->RemoveKnot(NbK1, M, myTol);
  }

  
}

//=======================================================================
//function : BSplineCurve
//purpose  :
//=======================================================================

Handle(Geom_BSplineCurve) GeomConvert_CompCurveToBSplineCurve::BSplineCurve() const 
{
 return myCurve;
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================

void GeomConvert_CompCurveToBSplineCurve::Clear()
{
  myCurve.Nullify();
}
