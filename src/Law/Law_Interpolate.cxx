// Created on: 1995-11-16
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

// Programme cree 

#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <Law_BSpline.hxx>
#include <Law_Interpolate.hxx>
#include <PLib.hxx>
#include <Standard_ConstructionError.hxx>
#include <StdFail_NotDone.hxx>
#include <TColStd_Array1OfBoolean.hxx>
#include <TColStd_Array1OfInteger.hxx>

//=======================================================================
//function : CheckParameters
//purpose  : 
//=======================================================================
static Standard_Boolean CheckParameters
(const TColStd_Array1OfReal&  Parameters) 
{
  Standard_Integer ii;
  Standard_Real distance;
  Standard_Boolean result = Standard_True;
  for (ii = Parameters.Lower() ; result && ii < Parameters.Upper() ; ii++) {
    distance = Parameters.Value(ii+1) - Parameters.Value(ii);
    result = (distance >= RealSmall());
  }
  return result;
}	

//=======================================================================
//function : BuildParameters
//purpose  : 
//=======================================================================
static void  BuildParameters(const Standard_Boolean         PeriodicFlag,
			     const TColStd_Array1OfReal&    PointsArray,
			     Handle(TColStd_HArray1OfReal)& ParametersPtr)
{
  Standard_Integer ii, index = 2;
  Standard_Real distance;
  Standard_Integer num_parameters = PointsArray.Length();
  if (PeriodicFlag) {
    num_parameters += 1;
  }
  ParametersPtr = new TColStd_HArray1OfReal(1, num_parameters);
  ParametersPtr->SetValue(1,0.);
  for (ii = PointsArray.Lower(); ii < PointsArray.Upper(); ii++) {
    distance = Abs(PointsArray.Value(ii) - PointsArray.Value(ii+1));
    ParametersPtr->SetValue(index, ParametersPtr->Value(ii) + distance);
    index += 1 ;
  }
  if (PeriodicFlag) {
    distance = Abs(PointsArray.Value(PointsArray.Upper()) - 
		   PointsArray.Value(PointsArray.Lower()));
    ParametersPtr->SetValue(index, ParametersPtr->Value(ii) + distance);
  }
}

//=======================================================================
//function : BuildPeriodicTangents
//purpose  : 
//=======================================================================

static void BuildPeriodicTangent
(const TColStd_Array1OfReal& PointsArray,
 TColStd_Array1OfReal&       TangentsArray,
 TColStd_Array1OfBoolean&    TangentFlags,
 const TColStd_Array1OfReal& ParametersArray)
{
  Standard_Real point_array[3], parameter_array[3], eval_result[2];
  
  if (PointsArray.Length() < 2) {
    TangentFlags.SetValue(1,Standard_True);
    TangentsArray.SetValue(1,0.);
  }   
  else if (!TangentFlags.Value(1)){
    //Pour les periodiques on evalue la tangente du point de fermeture
    //par une interpolation de degre 2 entre le dernier point, le point
    //de fermeture et le deuxieme point.
    Standard_Integer degree = 2;
    Standard_Real period = (ParametersArray.Value(ParametersArray.Upper()) -
			    ParametersArray.Value(ParametersArray.Lower()));
    point_array [0] = PointsArray.Value(PointsArray.Upper());
    point_array [1] = PointsArray.Value(PointsArray.Lower());
    point_array [2] = PointsArray.Value(PointsArray.Lower()+1);
    parameter_array[0] = ParametersArray.Value(ParametersArray.Upper() - 1) - period;
    parameter_array[1] = ParametersArray.Value(ParametersArray.Lower());
    parameter_array[2] = ParametersArray.Value(ParametersArray.Lower() + 1);
    TangentFlags.SetValue(1,Standard_True);
    PLib::EvalLagrange(parameter_array[1],
		       1,
		       degree,
		       1,
		       point_array[0],
		       parameter_array[0],
		       eval_result[0]);
    TangentsArray.SetValue(1,eval_result[1]);
  }
}

//=======================================================================
//function : BuildTangents
//purpose  : 
//=======================================================================

static void BuildTangents(const TColStd_Array1OfReal&  PointsArray,
			  TColStd_Array1OfReal&        TangentsArray,
			  TColStd_Array1OfBoolean&     TangentFlags,
		          const TColStd_Array1OfReal&  ParametersArray)
{
  Standard_Integer  degree = 3;//,ii;
  Standard_Real *point_array, *parameter_array, eval_result[2];
  
  if ( PointsArray.Length() < 3) {
    throw Standard_ConstructionError();
  }   
  if (PointsArray.Length() == 3) {
    degree = 2;
  }
  if (!TangentFlags.Value(1)) {
    point_array = (Standard_Real *) &PointsArray.Value(PointsArray.Lower()); 
    parameter_array = (Standard_Real *) &ParametersArray.Value(1);
    TangentFlags.SetValue(1,Standard_True);
    PLib::EvalLagrange(ParametersArray.Value(1),
		       1,
		       degree,
		       1,
		       point_array[0],
		       parameter_array[0],
		       eval_result[0]);
    TangentsArray.SetValue(1,eval_result[1]);
  }
  if (! TangentFlags.Value(TangentFlags.Upper())) {
    point_array = 
      (Standard_Real *) &PointsArray.Value(PointsArray.Upper() - degree);
    TangentFlags.SetValue(TangentFlags.Upper(),Standard_True);
    Standard_Integer iup = ParametersArray.Upper() - degree;
    parameter_array = (Standard_Real *) &ParametersArray.Value(iup);
    PLib::EvalLagrange(ParametersArray.Value(ParametersArray.Upper()),
		       1,
		       degree,
		       1,
		       point_array[0],
		       parameter_array[0],
		       eval_result[0]);
    TangentsArray.SetValue(TangentsArray.Upper(),eval_result[1]);
  }
} 

//=======================================================================
//function : Law_Interpolate
//purpose  : 
//=======================================================================

Law_Interpolate::Law_Interpolate
(const Handle(TColStd_HArray1OfReal)& PointsPtr,
 const Standard_Boolean               PeriodicFlag,
 const Standard_Real                  Tolerance) :
 myTolerance(Tolerance),
 myPoints(PointsPtr),
 myIsDone(Standard_False),
 myPeriodic(PeriodicFlag),
 myTangentRequest(Standard_False) 
     
{
//Standard_Integer ii;
  myTangents = new TColStd_HArray1OfReal (myPoints->Lower(),
					  myPoints->Upper());
  myTangentFlags = new TColStd_HArray1OfBoolean(myPoints->Lower(),
						myPoints->Upper());

  BuildParameters(PeriodicFlag,
		  PointsPtr->Array1(),
		  myParameters);
  myTangentFlags->Init(Standard_False);
}

//=======================================================================
//function : Law_Interpolate
//purpose  : 
//=======================================================================

Law_Interpolate::Law_Interpolate
(const Handle(TColStd_HArray1OfReal)& PointsPtr,
 const Handle(TColStd_HArray1OfReal)& ParametersPtr,
 const Standard_Boolean               PeriodicFlag,
 const Standard_Real                  Tolerance) :
 myTolerance(Tolerance),
 myPoints(PointsPtr),
 myIsDone(Standard_False),
 myParameters(ParametersPtr),
 myPeriodic(PeriodicFlag),
 myTangentRequest(Standard_False) 
{
//Standard_Integer ii;
  if (PeriodicFlag) {
    if ((PointsPtr->Length()) + 1 != ParametersPtr->Length()) {
      throw Standard_ConstructionError();
    }
  }
  myTangents = new TColStd_HArray1OfReal(myPoints->Lower(),
				       myPoints->Upper());
  myTangentFlags = new TColStd_HArray1OfBoolean(myPoints->Lower(),
						myPoints->Upper());
  Standard_Boolean result = CheckParameters(ParametersPtr->Array1());
  if (!result) {
    throw Standard_ConstructionError();
  }
  myTangentFlags->Init(Standard_False);
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Law_Interpolate::Load
(const TColStd_Array1OfReal&             Tangents,
 const Handle(TColStd_HArray1OfBoolean)& TangentFlagsPtr) 
{
//Standard_Boolean result;
  Standard_Integer ii;
  myTangentRequest = Standard_True;
  myTangentFlags = TangentFlagsPtr;
  if (Tangents.Length() != myPoints->Length() ||
      TangentFlagsPtr->Length() != myPoints->Length()) {
    throw Standard_ConstructionError();
  }
  myTangents = new TColStd_HArray1OfReal(Tangents.Lower(),Tangents.Upper());
  for (ii = Tangents.Lower() ; ii <= Tangents.Upper() ; ii++ ) {
    myTangents->SetValue(ii,Tangents.Value(ii));
  }
}

//=======================================================================
//function : Load
//purpose  : 
//=======================================================================

void Law_Interpolate::Load(const Standard_Real InitialTangent,
			   const Standard_Real FinalTangent) 
{
//Standard_Boolean result;
  myTangentRequest = Standard_True;
  myTangentFlags->SetValue(1,Standard_True);
  myTangents->SetValue(1,InitialTangent);
  myTangentFlags->SetValue(myPoints->Length(),Standard_True);
  myTangents->SetValue(myPoints->Length(),FinalTangent) ;
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void Law_Interpolate::Perform() 
{
  if (myPeriodic) {
    PerformPeriodic();
  }
  else {
    PerformNonPeriodic();
  }
}

//=======================================================================
//function : PerformPeriodic
//purpose  : 
//=======================================================================

void Law_Interpolate::PerformPeriodic()
{ 
  Standard_Integer degree,
  ii,
//jj,
  index,
  index1,
//index2,
  mult_index,
  half_order,
  inversion_problem,
  num_points,
  num_distinct_knots,
  num_poles;
  
  Standard_Real period;
  
//gp_Pnt a_point;
  
  num_points = myPoints->Length();
  period = myParameters->Value(myParameters->Upper()) -
    myParameters->Value(myParameters->Lower())  ;
  num_poles = num_points + 1 ;
  num_distinct_knots = num_points + 1;
  half_order = 2;  
  degree = 3;
  num_poles += 2;
  if (myTangentRequest) {
    for (ii = myTangentFlags->Lower() + 1; 
	 ii <= myTangentFlags->Upper(); ii++) {
      if (myTangentFlags->Value(ii)) {
	num_poles += 1;
      }
    }
  }
  TColStd_Array1OfReal     parameters(1,num_poles);  
  TColStd_Array1OfReal     flatknots(1,num_poles + degree + 1);
  TColStd_Array1OfInteger  mults(1,num_distinct_knots);
  TColStd_Array1OfInteger  contact_order_array(1, num_poles);
  TColStd_Array1OfReal     poles(1,num_poles);
  
  for (ii = 1 ; ii <= half_order ; ii++) {
    flatknots.SetValue(ii,myParameters->Value(myParameters->Upper() -1) -
		       period);
    flatknots.SetValue(ii + half_order,myParameters->
		       Value(myParameters->Lower()));
    flatknots.SetValue(num_poles + ii,
		       myParameters->Value(myParameters->Upper()));
    flatknots.SetValue(num_poles + half_order + ii,
		       myParameters->Value(half_order) + period);
  }
  for (ii = 1 ; ii <= num_poles ; ii++) {
    contact_order_array.SetValue(ii,0) ;
  }
  for (ii = 2; ii < num_distinct_knots; ii++) {
    mults.SetValue(ii,1); 
  }
  mults.SetValue(1,half_order);
  mults.SetValue(num_distinct_knots ,half_order);
  
  BuildPeriodicTangent(myPoints->Array1(),
		       myTangents->ChangeArray1(),
		       myTangentFlags->ChangeArray1(),
		       myParameters->Array1());
  
    contact_order_array.SetValue(2,1);
  parameters.SetValue(1,myParameters->Value(1));
  parameters.SetValue(2,myParameters->Value(1));
  poles.SetValue(1,myPoints->Value(1));
  poles.SetValue(2,myTangents->Value(1));
  mult_index = 2;
  index = 3;
  index1 = degree + 2;
  if (myTangentRequest) {
    for (ii = myTangentFlags->Lower() + 1; 
	 ii <= myTangentFlags->Upper(); ii++) {
      parameters.SetValue(index,myParameters->Value(ii));
      flatknots.SetValue(index1,myParameters->Value(ii));
      poles.SetValue(index,myPoints->Value(ii));
      index += 1;
      index1 += 1;
      if (myTangentFlags->Value(ii)) {
	mults.SetValue(mult_index,mults.Value(mult_index)  + 1);
	contact_order_array(index) = 1;
	
	parameters.SetValue(index,
			    myParameters->Value(ii));
	flatknots.SetValue(index1,myParameters->Value(ii));
	poles.SetValue(index,myTangents->Value(ii));
	index  += 1;
	index1 += 1;
      }
      mult_index += 1;
    }
  }
  else {
    index = degree + 1;
    index1 = 2 ;
    for(ii = myParameters->Lower(); ii <= myParameters->Upper(); ii++) {
      parameters.SetValue(index1, myParameters->Value(ii));
      flatknots.SetValue(index, myParameters->Value(ii));
      index += 1;
      index1 += 1;
    }
    index = 3;
    for (ii = myPoints->Lower() + 1; ii <= myPoints->Upper(); ii++) {
      //
      // copy all the given points since the last one will be initialized
      // below by the first point in the array myPoints
      //
      poles.SetValue(index, myPoints->Value(ii));
      index += 1;
    }
  }
  contact_order_array.SetValue(num_poles - 1, 1);
  parameters.SetValue(num_poles-1,
		      myParameters->Value(myParameters->Upper()));
  //
  // for the periodic curve ONLY  the tangent of the first point
  // will be used since the curve should close itself at the first
  // point See BuildPeriodicTangent
  //
  poles.SetValue(num_poles-1,myTangents->Value(1));
  parameters.SetValue(num_poles,
		      myParameters->Value(myParameters->Upper()));
  poles.SetValue(num_poles,
		 myPoints->Value(1));
  
  BSplCLib::Interpolate(degree,
			flatknots,
			parameters,
			contact_order_array,
			1,
			poles(1),
			inversion_problem);
  if (!inversion_problem) {
    TColStd_Array1OfReal  newpoles(poles.Value(1),
				   1,
				   num_poles - 2) ;
    myCurve =	new Law_BSpline(newpoles,
				myParameters->Array1(),
				mults,
				degree,
				myPeriodic);
    myIsDone = Standard_True;
  }
}


//=======================================================================
//function : PerformNonPeriodic
//purpose  : 
//=======================================================================

void Law_Interpolate::PerformNonPeriodic() 
{
  Standard_Integer degree,
  ii,
//jj,
  index,
  index1,
  index2,
  index3,
  mult_index,
  inversion_problem,
  num_points,
  num_distinct_knots,
  num_poles;
  
  num_points =
    num_distinct_knots =
      num_poles = myPoints->Length();
  if (num_poles == 2 &&   !myTangentRequest)  {
    degree = 1;
  } 
  else if (num_poles == 3 && !myTangentRequest) {
    degree = 2;
    num_distinct_knots = 2;
  }
  else {
    degree = 3;
    num_poles += 2;
    if (myTangentRequest) {
      for (ii = myTangentFlags->Lower() + 1; 
	   ii < myTangentFlags->Upper(); ii++) {
	if (myTangentFlags->Value(ii)) {
	  num_poles += 1;
	}
      }
    }
  }
  TColStd_Array1OfReal     parameters(1,num_poles) ;  
  TColStd_Array1OfReal     flatknots(1,num_poles + degree + 1) ;
  TColStd_Array1OfInteger  mults(1,num_distinct_knots) ;
  TColStd_Array1OfReal     knots(1,num_distinct_knots) ;
  TColStd_Array1OfInteger  contact_order_array(1, num_poles) ;
  TColStd_Array1OfReal     poles(1,num_poles) ;
  
  for (ii = 1 ; ii <= degree + 1 ; ii++) {
    flatknots.SetValue(ii,myParameters->Value(1));
    flatknots.SetValue(ii + num_poles,
		       myParameters->Value(num_points));
  }
  for (ii = 1 ; ii <= num_poles ; ii++) {
    contact_order_array.SetValue(ii,0);
  }
  for (ii = 2 ; ii < num_distinct_knots ; ii++) {
    mults.SetValue(ii,1); 
  }
  mults.SetValue(1,degree + 1);
  mults.SetValue(num_distinct_knots ,degree + 1);
  
  switch (degree) {
  case 1:
    for (ii = 1 ; ii <= num_poles ; ii++) {
      poles.SetValue(ii ,myPoints->Value(ii));
    }
    myCurve =
      new Law_BSpline(poles,
		      myParameters->Array1(),
		      mults,
		      degree) ;
    myIsDone = Standard_True ;
    break ;
  case 2:
    knots.SetValue(1,myParameters->Value(1)) ;
    knots.SetValue(2,myParameters->Value(num_poles)) ;
    for (ii = 1 ; ii <= num_poles ; ii++) {
      poles.SetValue(ii,myPoints->Value(ii)) ;
      
    }
    BSplCLib::Interpolate(degree,
			  flatknots,
			  myParameters->Array1(),
			  contact_order_array,
			  1,
			  poles(1),
			  inversion_problem) ;
    if (!inversion_problem) {
      myCurve = new Law_BSpline(poles,
				knots,
				mults,
				degree);
      myIsDone = Standard_True;
    }
    break;
  case 3:
    //
    // check if the boundary conditions are set
    //
    if (num_points >= 3) {
      //
      // cannot build the tangents with degree 3 with only 2 points
      // if those where not given in advance 
      //
      BuildTangents(myPoints->Array1(),
		    myTangents->ChangeArray1(),
		    myTangentFlags->ChangeArray1(),
		    myParameters->Array1()) ;
    }
    contact_order_array.SetValue(2,1);
    parameters.SetValue(1,myParameters->Value(1));
    parameters.SetValue(2,myParameters->Value(1));
    poles.SetValue(1,myPoints->Value(1));
    poles.SetValue(2,myTangents->Value(1));
    mult_index = 2 ;
    index = 3 ;
    index1 = 2 ;
    index2 = myPoints->Lower() + 1 ;
    index3 = degree + 2 ;
    if (myTangentRequest) {
      for (ii = myParameters->Lower() + 1 ; 
	   ii < myParameters->Upper() ; ii++) {
	parameters.SetValue(index,myParameters->Value(ii)) ;
	poles.SetValue(index,myPoints->Value(index2)) ;
        flatknots.SetValue(index3,myParameters->Value(ii)) ; 
	index += 1 ;
        index3 += 1 ;
	if (myTangentFlags->Value(index1)) {
	  //
	  // set the multiplicities, the order of the contact, the 
	  // the flatknots,
	  //
	  mults.SetValue(mult_index,mults.Value(mult_index) + 1) ;
	  contact_order_array(index) = 1 ;
          flatknots.SetValue(index3, myParameters->Value(ii)) ;
	  parameters.SetValue(index,
			      myParameters->Value(ii));
	  poles.SetValue(index,myTangents->Value(ii));
	  index += 1  ;
	  index3 += 1 ;
	}
	mult_index += 1 ;
	index1 += 1 ;
	index2 += 1 ;
      }
    }
    else {
      index1 = 2 ;
      for(ii = myParameters->Lower(); ii <= myParameters->Upper(); ii++) {
	parameters.SetValue(index1, myParameters->Value(ii));
	index1 += 1;
      }
      index = 3 ;
      for (ii = myPoints->Lower() + 1; ii <= myPoints->Upper() - 1; ii++){
	poles.SetValue(index, myPoints->Value(ii));
	index += 1 ;
      }
      index = degree + 1;
      for(ii = myParameters->Lower(); ii <= myParameters->Upper(); ii++){
	flatknots.SetValue(index, myParameters->Value(ii));
	index += 1 ;
      }
    }
    poles.SetValue(num_poles-1, myTangents->Value(num_points));
    
    contact_order_array.SetValue(num_poles - 1, 1);
    parameters.SetValue(num_poles,
			myParameters->Value(myParameters->Upper())) ;
    parameters.SetValue(num_poles -1,
			myParameters->Value(myParameters->Upper())) ;
    
    poles.SetValue(num_poles,
		   myPoints->Value(num_points)) ;
    
    BSplCLib::Interpolate(degree,
			  flatknots,
			  parameters,
			  contact_order_array,
			  1,
			  poles(1),
			  inversion_problem) ;
    if (!inversion_problem) {
      myCurve =	new Law_BSpline(poles,
				myParameters->Array1(),
				mults,
				degree) ;
      myIsDone = Standard_True;
    }
    break ;
  }
}

//=======================================================================
//function : Handle(Geom_BSplineCurve)&
//purpose  : 
//=======================================================================

const Handle(Law_BSpline)& Law_Interpolate::Curve() const 
{
  if ( !myIsDone) 
    throw StdFail_NotDone(" ");
  return myCurve;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean Law_Interpolate::IsDone() const
{
  return myIsDone;
}
