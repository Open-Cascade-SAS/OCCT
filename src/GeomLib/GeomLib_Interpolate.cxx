// Created on: 1996-08-30
// Created by: Xavier BENVENISTE
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


#include <BSplCLib.hxx>
#include <Geom_BSplineCurve.hxx>
#include <GeomLib_Interpolate.hxx>
#include <gp_Vec.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <StdFail_NotDone.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfBoolean.hxx>
#include <TColStd_HArray1OfReal.hxx>

//=======================================================================
//function : GeomLib_Interpolate
//purpose  : 
//=======================================================================
GeomLib_Interpolate::GeomLib_Interpolate
(const Standard_Integer      Degree,
 const Standard_Integer      NumPoints,
 const TColgp_Array1OfPnt&   PointsArray,
 const TColStd_Array1OfReal& ParametersArray) 
					 
{
  Standard_Integer ii,
  num_knots,
  inversion_problem,
  num_controls,
  jj ;
  
  
  if (NumPoints < Degree ||
      PointsArray.Lower() != 1 ||
      PointsArray.Upper() < NumPoints ||
      ParametersArray.Lower() != 1 ||
      ParametersArray.Upper() < NumPoints) {
    myError = GeomLib_NotEnoughtPoints ;
  }
  else if (Degree < 3) {
    myError = GeomLib_DegreeSmallerThan3 ;
  }
  else {
    gp_Pnt null_point(0.0e0, 0.0e0, 0.0e0) ;
    Standard_Integer  order  = Degree + 1,
    half_order ;
    if (order  % 2) {
      order  -= 1 ;
    }
    half_order = order / 2 ;
    num_knots = NumPoints + 2 *  order - 2  ;
    num_controls = num_knots - order ;
    TColStd_Array1OfReal       flat_knots(1,num_knots) ;
    TColStd_Array1OfInteger    contacts  (1,num_controls) ;
    TColStd_Array1OfInteger    multiplicities(1, NumPoints) ;
    TColStd_Array1OfReal       parameters(1,num_controls) ;
    TColgp_Array1OfPnt         poles(1,num_controls) ;
    
    for (ii = 1 ; ii <= NumPoints ; ii++) {
      multiplicities(ii) = 1 ;
    }
    multiplicities(1)         = order ;
    multiplicities(NumPoints) = order ;
    for (ii = 1,
	 jj = num_controls + 1 ; ii <=  order ; ii++, jj++) {

      flat_knots(ii) = ParametersArray(1) ;
      flat_knots(jj) = ParametersArray(NumPoints) ;
    }
    jj = order + 1 ;
    for (ii = 2 ; ii < NumPoints ; ii++) {
      flat_knots(jj) = ParametersArray(ii) ;
      jj+= 1 ;
    }
    for (ii = 1 ; ii <= num_controls ; ii++) {
      contacts(ii) = 0 ;
    }
    jj = num_controls ;
    for (ii = 1 ; ii <= half_order ; ii++) {
      contacts(ii) = half_order + ii - 1 ;
      parameters(ii) = ParametersArray(1) ;
      poles(ii) = null_point ;   
      contacts(jj) = half_order + ii - 1 ;
      parameters(jj) = ParametersArray(NumPoints) ;
      poles(jj) = null_point ;
      jj -= 1 ;
    }
    jj = half_order + 1 ;
    for (ii = 2 ; ii < NumPoints ; ii++) {
      parameters(jj) = ParametersArray(ii) ;
      poles(jj) = PointsArray(ii) ;
      jj += 1 ;
    }
    contacts(1) = 0 ;
    contacts(num_controls) = 0 ;
    poles(1) = PointsArray(1)  ;
    poles(num_controls) = PointsArray(NumPoints) ;
    BSplCLib::Interpolate(order-1,
			  flat_knots,
			  parameters,
			  contacts,
			  poles,
			  inversion_problem) ;
    
    if (!inversion_problem) {
      myCurve =
	new Geom_BSplineCurve(poles,
			      ParametersArray,
			      multiplicities,
			      order-1) ;
      myIsDone = Standard_True ;
    } 
    else {
      myError = GeomLib_InversionProblem ;
    }
  }
}
    


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve) GeomLib_Interpolate::Curve() const 
{
  return myCurve ;
}
  
