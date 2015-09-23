// Created on: 1993-10-20
// Created by: Bruno DUMORTIER
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

// modified 25/06/1996 PMN : Ajout d'une tolerance Angulaire dans le 
//  constructeur pour le test de continuite G1 (1 Radians c'etait trop
//  cf BUG PRO4481)     
//rln 20.06.99 work-around

#include <BSplCLib.hxx>
#include <gp.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <PLib.hxx>
#include <Precision.hxx>
#include <ShapeConstruct_CompBezierCurves2dToBSplineCurve2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>

//=======================================================================
//function : ShapeConstruct_CompBezierCurves2dToBSplineCurve2d
//purpose  : 
//=======================================================================
ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::
ShapeConstruct_CompBezierCurves2dToBSplineCurve2d(
	          const Standard_Real AngularTolerance) :
		  myAngular(AngularTolerance),
		  myDone(Standard_False)
{
}


//=======================================================================
//function : AddCurve
//purpose  : 
//=======================================================================

void  ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::AddCurve
  (const TColgp_Array1OfPnt2d& Poles)
{
  if ( !mySequence.IsEmpty()) {
    gp_Pnt2d P1,P2;
    P1 = mySequence.Last()->Value(mySequence.Last()->Upper());
    P2 = Poles(Poles.Lower());

// User defined tolerance NYI
//    Standard_ConstructionError_Raise_if
//      ( !P1.IsEqual(P2,Precision::Confusion()),
//       "ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::Addcurve");

  }
  myDone = Standard_False;
  Handle(TColgp_HArray1OfPnt2d) HPoles = 
    new TColgp_HArray1OfPnt2d(Poles.Lower(),Poles.Upper());
  HPoles->ChangeArray1() = Poles;
  mySequence.Append(HPoles);
}


//=======================================================================
//function : Degree
//purpose  : 
//=======================================================================

Standard_Integer  ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::Degree()
const {
  return myDegree;
}


//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer  ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::NbPoles()
const {
  return CurvePoles.Length();
}


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::Poles
  (TColgp_Array1OfPnt2d& Poles) const
{
  Standard_Integer i, Lower = Poles.Lower(), Upper = Poles.Upper();
  Standard_Integer k = 1;
  for (i = Lower; i <= Upper; i++) {
    Poles(i) = CurvePoles(k++);
  }
}


//=======================================================================
//function : NbKnots
//purpose  : 
//=======================================================================

Standard_Integer  ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::NbKnots()
const {
  return CurveKnots.Length();
}


//=======================================================================
//function : KnotsAndMults
//purpose  : 
//=======================================================================

void  ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::KnotsAndMults
  (TColStd_Array1OfReal&    Knots, 
   TColStd_Array1OfInteger& Mults ) const
{
  Standard_Integer i, LowerK = Knots.Lower(), UpperK = Knots.Upper();
  Standard_Integer LowerM = Mults.Lower(), UpperM = Mults.Upper();
  Standard_Integer k = 1;
  for (i = LowerK; i <= UpperK; i++) {
    Knots(i) = CurveKnots(k++);
  }
  k = 1;
  for (i = LowerM; i <= UpperM; i++) {
    Mults(i) = KnotsMultiplicities(k++);
  }
}





//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void ShapeConstruct_CompBezierCurves2dToBSplineCurve2d::Perform() 
{
  myDone = Standard_True;
  CurvePoles.Clear();
  CurveKnots.Clear();
  KnotsMultiplicities.Clear();
  Standard_Integer LowerI     = 1;
  Standard_Integer UpperI     = mySequence.Length();
  Standard_Integer NbrCurv    = UpperI-LowerI+1;
  TColStd_Array1OfReal     CurveKnVals         (1,NbrCurv);

  Standard_Integer i;
  myDegree = 0;
  for ( i = 1; i <= mySequence.Length(); i++) {
    myDegree = Max( myDegree, (mySequence(i))->Length() -1);
  }

  Standard_Real D1, D2, Lambda, Det=0.;
  gp_Pnt2d P1, P2, P3;
  Standard_Integer Deg, Inc, MaxDegree = myDegree;
  TColgp_Array1OfPnt2d Points(1, myDegree+1);

  for (i = LowerI ; i <= UpperI ; i++) {
    // 1- Elever la courbe de Bezier au degre maximum.
    Deg = mySequence(i)->Length()-1;
    Inc = myDegree - Deg;
    if ( Inc > 0) {
      BSplCLib::IncreaseDegree(myDegree, 
			       mySequence(i)->Array1(), BSplCLib::NoWeights(),
			       Points, BSplCLib::NoWeights());
    }
    else {
      Points = mySequence(i)->Array1();
    }

    // 2- Traiter le noeud de jonction entre 2 courbes de Bezier.
    if (i == LowerI) {
      // Traitement du noeud initial de la BSpline.
      for (Standard_Integer j = 1 ; j <= MaxDegree ; j++) {
	CurvePoles.Append(Points(j));
      }
      CurveKnVals(1)         = 1.; // Pour amorcer la serie.
      KnotsMultiplicities.Append(MaxDegree+1);
      Det = 1.;
    }


    if (i != LowerI) {
      P2 = Points(1);
      P3 = Points(2);
      gp_Vec2d V1(P1, P2), V2(P2, P3);
      D1 = P1.SquareDistance(P2);
      D2 = P3.SquareDistance(P2);
      Lambda = Sqrt(D2/D1);


      // Traitement de la tangence entre la Bezier et sa precedente.
      // Ceci permet d''assurer au moins une continuite C1 si 
      // les tangentes sont coherentes.
      

      // Test de l'angle a myAngular

      if (V1.Magnitude() > gp::Resolution() &&
	  V2.Magnitude() > gp::Resolution() &&
	  V1.IsParallel(V2, myAngular) &&
	  MaxDegree > 1) {//rln 20.06.99 work-around
	KnotsMultiplicities.Append(MaxDegree-1);
	CurveKnVals(i) = CurveKnVals(i-1) * Lambda;
	Det += CurveKnVals(i);

      }
      else {
	CurveKnVals(i) = 1.0e0 ;
	Det += CurveKnVals(i) ;
	CurvePoles.Append(Points(1));
	KnotsMultiplicities.Append(MaxDegree);
      }

      // Stocker les poles.
      for (Standard_Integer j = 2 ; j <= MaxDegree ; j++) {
	CurvePoles.Append(Points(j));
      }

    }


    if (i == UpperI) {
      // Traitement du noeud terminal de la BSpline.
      CurvePoles.Append(Points(MaxDegree+1));
      KnotsMultiplicities.Append(MaxDegree+1);
    }
    P1 = Points(MaxDegree);
  }

  // Corriger les valeurs nodales pour les faire varier dans [0.,1.].
  CurveKnots.Append(0.0);
  for (i = 2 ; i <= NbrCurv ; i++) {
    CurveKnots.Append(CurveKnots(i-1) + (CurveKnVals(i-1)/Det));
  }
  CurveKnots.Append(1.0);
}


