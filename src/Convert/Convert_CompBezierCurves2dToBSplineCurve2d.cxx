// Created on: 1993-10-20
// Created by: Bruno DUMORTIER
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


#include <Convert_CompBezierCurves2dToBSplineCurve2d.ixx>

#include <Precision.hxx>
#include <BSplCLib.hxx>
#include <PLib.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>


//=======================================================================
//function : Convert_CompBezierCurves2dToBSplineCurve2d
//purpose  : 
//=======================================================================

Convert_CompBezierCurves2dToBSplineCurve2d::
Convert_CompBezierCurves2dToBSplineCurve2d(
	          const Standard_Real AngularTolerance) :
		  myAngular(AngularTolerance),
		  myDone(Standard_False)
{
}


//=======================================================================
//function : AddCurve
//purpose  : 
//=======================================================================

void  Convert_CompBezierCurves2dToBSplineCurve2d::AddCurve
  (const TColgp_Array1OfPnt2d& Poles)
{
  if ( !mySequence.IsEmpty()) {
    gp_Pnt2d P1,P2;
    P1 = mySequence.Last()->Value(mySequence.Last()->Upper());
    P2 = Poles(Poles.Lower());

// User defined tolerance NYI
//    Standard_ConstructionError_Raise_if
//      ( !P1.IsEqual(P2,Precision::Confusion()),
//       "Convert_CompBezierCurves2dToBSplineCurve2d::Addcurve");

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

Standard_Integer  Convert_CompBezierCurves2dToBSplineCurve2d::Degree()
const {
  return myDegree;
}


//=======================================================================
//function : NbPoles
//purpose  : 
//=======================================================================

Standard_Integer  Convert_CompBezierCurves2dToBSplineCurve2d::NbPoles()
const {
  return CurvePoles.Length();
}


//=======================================================================
//function : Poles
//purpose  : 
//=======================================================================

void  Convert_CompBezierCurves2dToBSplineCurve2d::Poles
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

Standard_Integer  Convert_CompBezierCurves2dToBSplineCurve2d::NbKnots()
const {
  return CurveKnots.Length();
}


//=======================================================================
//function : KnotsAndMults
//purpose  : 
//=======================================================================

void  Convert_CompBezierCurves2dToBSplineCurve2d::KnotsAndMults
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

void Convert_CompBezierCurves2dToBSplineCurve2d::Perform() 
{
  myDone = Standard_True;
  CurvePoles.Clear();
  CurveKnots.Clear();
  KnotsMultiplicities.Clear();
  Standard_Integer LowerI     = 1;
  Standard_Integer UpperI     = mySequence.Length();
  Standard_Integer NbrCurv    = UpperI-LowerI+1;
//  Standard_Integer NbKnotsSpl = NbrCurv + 1 ;
  TColStd_Array1OfReal     CurveKnVals         (1,NbrCurv);

  Standard_Integer i;
  myDegree = 0;
  for ( i = 1; i <= mySequence.Length(); i++) {
    myDegree = Max( myDegree, (mySequence(i))->Length() -1);
  }

  Standard_Real D1, D2, Lambda, Det=0;
  gp_Pnt2d P1, P2, P3;
  Standard_Integer Deg, Inc, MaxDegree = myDegree;
  TColgp_Array1OfPnt2d Points(1, myDegree+1);

  for (i = LowerI ; i <= UpperI ; i++) {
    // 1- Rise Bezier curve to the maximum degree.
    Deg = mySequence(i)->Length()-1;
    Inc = myDegree - Deg;
    if ( Inc > 0) {
      BSplCLib::IncreaseDegree(myDegree, 
			       mySequence(i)->Array1(), PLib::NoWeights(),
			       Points, PLib::NoWeights());
    }
    else {
      Points = mySequence(i)->Array1();
    }

    // 2- Process the node of junction between Bezier curves.
    if (i == LowerI) {
      // Processing of initial node of the BSpline.
      for (Standard_Integer j = 1 ; j <= MaxDegree ; j++) {
	CurvePoles.Append(Points(j));
      }
      CurveKnVals(1)         = 1.; // To begin the series.
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


      // Processing of the tangency between the Bezier and the previous.
      // This allows guaranteeing at least continuity C1 if the tangents are coherent.
      

      // Test of angle at myAngular

      if (V1.Magnitude() > gp::Resolution() &&
	  V2.Magnitude() > gp::Resolution() &&
	  V1.IsParallel(V2, myAngular )) {
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

      // Store poles.
      for (Standard_Integer j = 2 ; j <= MaxDegree ; j++) {
	CurvePoles.Append(Points(j));
      }

    }


    if (i == UpperI) {
      // Process end node of the BSpline.
      CurvePoles.Append(Points(MaxDegree+1));
      KnotsMultiplicities.Append(MaxDegree+1);
    }
    P1 = Points(MaxDegree);
  }

  // Correct nodal values to make them variable within [0.,1.].
  CurveKnots.Append(0.0);
  for (i = 2 ; i <= NbrCurv ; i++) {
    CurveKnots.Append(CurveKnots(i-1) + (CurveKnVals(i-1)/Det));
  }
  CurveKnots.Append(1.0);
}


