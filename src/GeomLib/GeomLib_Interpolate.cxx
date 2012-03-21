// Created on: 1996-08-30
// Created by: Xavier BENVENISTE
// Copyright (c) 1996-1999 Matra Datavision
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



#include <GeomLib_Interpolate.ixx>

#include <Standard_ConstructionError.hxx>
#include <PLib.hxx>
#include <BSplCLib.hxx>
#include <gp_Vec.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfVec.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Handle_TColStd_HArray1OfBoolean.hxx>

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
  
