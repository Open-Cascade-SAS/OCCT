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

//JCV 16/10/91

#include <Convert_EllipseToBSplineCurve.hxx>
#include <gp.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Trsf2d.hxx>
#include <Precision.hxx>
#include <Standard_DomainError.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TColgp_HArray1OfPnt2d.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>

//Attention :
//To avoid use of persistent tables in the fields
//the tables are dimensioned to the maximum (TheNbKnots and TheNbPoles)
//that correspond to the full circle. For an arc of circle there is a
//need of less poles and nodes, that is why the fields
//nbKnots and nbPoles are present and updated in the 
//constructor of an arc of B-spline circle to take into account 
//the real number of poles and nodes.
// parameterization :
// Reference : Rational B-spline for Curve and Surface Representation
//             Wayne Tiller  CADG September 1983
// x(t) = (1 - t^2) / (1 + t^2)
// y(t) =  2 t / (1 + t^2)
// then t = Sqrt(2) u /  ((Sqrt(2) - 2) u + 2)
// => u = 2 t / (Sqrt(2) + (2 - Sqrt(2)) t)
//=======================================================================
//function : Convert_EllipseToBSplineCurve
//purpose  : this constructs a periodic Ellipse 
//=======================================================================
Convert_EllipseToBSplineCurve::Convert_EllipseToBSplineCurve 
  (const gp_Elips2d& E, const Convert_ParameterisationType Parameterisation)
:Convert_ConicToBSplineCurve(0,0,0){

  Standard_Integer ii ;

  Standard_Real R,
  r,
  value ;
  Handle(TColStd_HArray1OfReal) CosNumeratorPtr,
  SinNumeratorPtr ;

  
  R = E.MajorRadius();
  r = E.MinorRadius();


  if (Parameterisation != Convert_TgtThetaOver2 &&
    Parameterisation != Convert_RationalC1) {
    // If BuildCosAndSin cannot manage the periodicity
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
		     mults) ;
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
    new TColgp_HArray1OfPnt2d(1,nbPoles)   ;


  gp_Dir2d Ox = E.XAxis().Direction();
  gp_Dir2d Oy = E.YAxis().Direction();
  gp_Trsf2d Trsf;
  Trsf.SetTransformation( E.XAxis(), gp::OX2d());
  if  ( Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.0e0) {
    value = r ;
  }
  else {
    value = -r ;
   }
  
  // Replace the bspline in the mark of the circle.
  // and calculate the weight of the bspline.

  for (ii = 1; ii <= nbPoles ; ii++) {
     poles->ChangeArray1()(ii).SetCoord(1, R * CosNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).SetCoord(2, value * SinNumeratorPtr->Value(ii)) ;
     poles->ChangeArray1()(ii).Transform( Trsf); 
   }

}
//=======================================================================
//function : Convert_EllipseToBSplineCurve
//purpose  : this constructs a non periodic Ellipse
//=======================================================================

Convert_EllipseToBSplineCurve::Convert_EllipseToBSplineCurve 
  (const gp_Elips2d& E, 
   const Standard_Real  UFirst,
   const Standard_Real  ULast,
   const Convert_ParameterisationType Parameterisation)
:Convert_ConicToBSplineCurve(0,0,0)
{
#ifndef No_Exception
  Standard_Real Tol = Precision::PConfusion();
  Standard_Real delta = ULast - UFirst;
#endif
  Standard_DomainError_Raise_if( (delta > (2*M_PI+Tol)) || (delta <= 0.0e0),
				"Convert_EllipseToBSplineCurve");
  Standard_Integer ii;
  Standard_Real R, r, value;
  Handle(TColStd_HArray1OfReal) CosNumeratorPtr, SinNumeratorPtr;
  
  R = E.MajorRadius();
  r = E.MinorRadius();

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

  poles = new TColgp_HArray1OfPnt2d(1,nbPoles)   ;

  gp_Dir2d Ox = E.XAxis().Direction();
  gp_Dir2d Oy = E.YAxis().Direction();
  gp_Trsf2d Trsf;
  Trsf.SetTransformation( E.XAxis(), gp::OX2d());
  if  ( Ox.X() * Oy.Y() - Ox.Y() * Oy.X() > 0.0e0) {
    value = r ;
  }
  else {
    value = -r ;
  }
  
  // Replace the bspline in the mark of the circle.
  // and calculate the weight of the bspline.
  
  for (ii = 1; ii <= nbPoles ; ii++) {
    poles->ChangeArray1()(ii).SetCoord(1, R * CosNumeratorPtr->Value(ii)) ;
    poles->ChangeArray1()(ii).SetCoord(2, value * SinNumeratorPtr->Value(ii)) ;
    poles->ChangeArray1()(ii).Transform( Trsf); 
  }
  
}

