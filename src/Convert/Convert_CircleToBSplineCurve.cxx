// Copyright (c) 1995-1999 Matra Datavision
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

//JCV 16/10/91

#include <Convert_CircleToBSplineCurve.ixx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColgp_Array1OfPnt2d.hxx>

#include <Precision.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Trsf2d.hxx>

//Attention :
//To avoid use of persistent tables in the fields
//the tables are dimensioned to the maximum (TheNbKnots and TheNbPoles)
//that correspond to the full circle. For an arc of circle there is a
//need of less poles and nodes, that is why the fields
//nbKnots and nbPoles are present and updated in the 
//constructor of an arc of B-spline circle to take into account 
//the real number of poles and nodes.


// parametrization :
// Reference : Rational B-spline for Curve and Surface Representation
//             Wayne Tiller  CADG September 1983
// x(t) = (1 - t^2) / (1 + t^2)
// y(t) =  2 t / (1 + t^2)
// then t = Sqrt(2) u /  ((Sqrt(2) - 2) u + 2)
// => u = 2 t / (Sqrt(2) + (2 - Sqrt(2)) t)

//=======================================================================
//function : Convert_CircleToBSplineCurve
//purpose  : this constructs a periodic circle 
//=======================================================================

Convert_CircleToBSplineCurve::Convert_CircleToBSplineCurve 
  (const gp_Circ2d& C, const Convert_ParameterisationType Parameterisation)
:Convert_ConicToBSplineCurve(0,0,0){

  Standard_Integer ii ;

  Standard_Real R,
  value ;
  Handle(TColStd_HArray1OfReal) CosNumeratorPtr,
  SinNumeratorPtr ;


  R = C.Radius() ; 
  if (Parameterisation != Convert_TgtThetaOver2 &&
    Parameterisation != Convert_RationalC1) {
    // In case if BuildCosAndSin does not know how to manage the periodicity
    // => trim on 0,2*PI
    isperiodic = Standard_False;
    Convert_ConicToBSplineCurve::
      BuildCosAndSin(Parameterisation,
		     0, 2*M_PI,
		     CosNumeratorPtr,
		     SinNumeratorPtr,
		     weights,
		     degree,
		     knots,
		     mults);
      }   
  else { 
    isperiodic = Standard_True;
    Convert_ConicToBSplineCurve::
      BuildCosAndSin(Parameterisation,
		     CosNumeratorPtr,
		     SinNumeratorPtr,
		     weights,
		     degree,
		     knots,
		     mults);
  }


  nbPoles = CosNumeratorPtr->Length();
  nbKnots = knots->Length();

  poles = 
    new TColgp_HArray1OfPnt2d(1,nbPoles);


  gp_Dir2d Ox = C.XAxis().Direction();
  gp_Dir2d Oy = C.YAxis().Direction();
  gp_Trsf2d Trsf;
  Trsf.SetTransformation( C.XAxis(), gp::OX2d());
  if  ( Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.0e0) {
    value = R ;
  }
  else {
    value = -R ;
   }
  
  // Replace the bspline in the reference of the circle.
  // and calculate the weight of the bspline.

  for (ii = 1; ii <= nbPoles ; ii++) {
     poles->ChangeArray1()(ii).SetCoord(1, R * CosNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).SetCoord(2, value * SinNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).Transform( Trsf); 
   }

}
//=======================================================================
//function : Convert_CircleToBSplineCurve
//purpose  : this constructs a non periodic circle
//=======================================================================

Convert_CircleToBSplineCurve::Convert_CircleToBSplineCurve 
  (const gp_Circ2d& C, 
   const Standard_Real  UFirst,
   const Standard_Real  ULast,
   const Convert_ParameterisationType Parameterisation)
:Convert_ConicToBSplineCurve(0,0,0)
{
  Standard_Real delta = ULast - UFirst ;
  Standard_Real Eps = Precision::PConfusion();

  if ( (delta > (2*M_PI + Eps)) || (delta <= 0.0e0) ) {
    Standard_DomainError::Raise( "Convert_CircleToBSplineCurve");
  }

  Standard_Integer ii;
  Standard_Real R, value ;
  Handle(TColStd_HArray1OfReal) CosNumeratorPtr,SinNumeratorPtr ;


  R = C.Radius() ;    
  isperiodic = Standard_False;
  Convert_ConicToBSplineCurve::BuildCosAndSin(Parameterisation,
					      UFirst,
					      ULast,
					      CosNumeratorPtr,
					      SinNumeratorPtr,
					      weights,
					      degree,
					      knots,
					      mults) ;



  nbPoles = CosNumeratorPtr->Length();
  nbKnots = knots->Length();

  poles = 
    new TColgp_HArray1OfPnt2d(1,nbPoles)   ;

  gp_Dir2d Ox = C.XAxis().Direction();
  gp_Dir2d Oy = C.YAxis().Direction();
  gp_Trsf2d Trsf;
  Trsf.SetTransformation( C.XAxis(), gp::OX2d());
  if  ( Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.0e0) {
    value = R ;
  }
  else {
    value = -R ;
   }
  
  // Replace the bspline in the reference of the circle.
  // and calculate the weight of the bspline.

  for (ii = 1; ii <= nbPoles ; ii++) {
     poles->ChangeArray1()(ii).SetCoord(1, R * CosNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).SetCoord(2, value * SinNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).Transform( Trsf); 
   }

}

