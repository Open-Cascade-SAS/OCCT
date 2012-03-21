// Created on: 1997-05-28
// Created by: Xavier BENVENISTE
// Copyright (c) 1997-1999 Matra Datavision
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



#include <GeomLib_CheckBSplineCurve.ixx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
//=======================================================================
//function : GeomLib_CheckBSplineCurve
//purpose  : 
//=======================================================================

GeomLib_CheckBSplineCurve::GeomLib_CheckBSplineCurve(const Handle(Geom_BSplineCurve)& Curve,
						     const Standard_Real Tolerance,
						     const Standard_Real AngularTolerance)
:myCurve(Curve),
myDone(Standard_False),
myFixFirstTangent(Standard_False),
myFixLastTangent(Standard_False),
myAngularTolerance(Abs(AngularTolerance)),
myTolerance(Abs(Tolerance)),
myFirstPole(1.0,0.0e0,0.e0),
myLastPole(1.0e0,0.0e0,0.e0)
{
  

  Standard_Integer ii,
    num_poles ;
  Standard_Real tangent_magnitude,
    value,
    angular_value,
    factor,
    vector_magnitude ;
  num_poles = Curve->NbPoles() ;
  if (( ! myCurve->IsPeriodic() )&& num_poles >= 4) {
    
    gp_Vec tangent,
      diff,
      a_vector;
    for (ii = 1 ; ii <= 3 ; ii++) {
      tangent.SetCoord(ii,myCurve->Pole(2).Coord(ii) - myCurve->Pole(1).Coord(ii))  ;
      a_vector.SetCoord(ii, myCurve->Pole(3).Coord(ii) - myCurve->Pole(1).Coord(ii)) ;
    }
    tangent_magnitude = tangent.Magnitude() ;
    vector_magnitude = a_vector.Magnitude() ;
    if (tangent_magnitude > myTolerance &&
	vector_magnitude > myTolerance)
      {
        value = tangent.Dot(a_vector) ;
        if ( value < 0.0e0) {
	  for (ii = 1 ; ii <= 3 ; ii++) {
	    diff.SetCoord(ii, (tangent.Coord(ii) / tangent_magnitude) + (a_vector.Coord(ii) / vector_magnitude)) ; 
          } 
	  angular_value = 
	    diff.Magnitude() ;
          if (angular_value < myAngularTolerance) {
	    myFixFirstTangent = Standard_True ;
	    factor = 1.0e0 ;
	    if (tangent_magnitude > 0.5e0 * vector_magnitude) {
	      factor = 0.5e0 *  vector_magnitude / tangent_magnitude ;
	    }
	    for (ii = 1 ; ii <= 3 ; ii++) {
	      myFirstPole.SetCoord(ii, myCurve->Pole(1).Coord(ii)  - factor * tangent.Coord(ii))  ;
	    }
          }
	  
	}
      }
    for (ii = 1 ; ii <= 3 ; ii++) {
      tangent.SetCoord(ii,myCurve->Pole(num_poles-1).Coord(ii) - myCurve->Pole(num_poles).Coord(ii))  ;
      a_vector.SetCoord(ii, myCurve->Pole(num_poles-2).Coord(ii) - myCurve->Pole(num_poles).Coord(ii)) ;
    }
    tangent_magnitude = tangent.Magnitude() ;
    vector_magnitude = a_vector.Magnitude() ;
    
    if (tangent_magnitude > myTolerance &&
	vector_magnitude > myTolerance)
      {
        value = tangent.Dot(a_vector) ;
	if (value < 0.0e0) {
	  for (ii = 1 ; ii <= 3 ; ii++) {
	    diff.SetCoord(ii, (tangent.Coord(ii) / tangent_magnitude) + (a_vector.Coord(ii) / vector_magnitude)) ; 
          } 
	  angular_value = 
	    diff.Magnitude() ;
	  if ( angular_value < myAngularTolerance) {
	    myFixLastTangent = Standard_True ;
	    factor = 1.0e0 ;
	    if (tangent_magnitude > 0.5e0 * vector_magnitude) {
	      factor = 0.5e0 *  vector_magnitude / tangent_magnitude ;
	    }
	    for (ii = 1 ; ii <= 3 ; ii++) {
	      myLastPole.SetCoord(ii, myCurve->Pole(num_poles).Coord(ii)  - factor * tangent.Coord(ii))  ;
	    }
	  }
	  
        }
      }
    
  }
  else {
    myDone = Standard_True ;
  }
}
  
//=======================================================================
//function : NeedTangentFix
//purpose  : 
//=======================================================================

void GeomLib_CheckBSplineCurve::NeedTangentFix(Standard_Boolean & FirstFlag,
					       Standard_Boolean & LastFlag) const 
{
  FirstFlag = myFixFirstTangent ;
  LastFlag = myFixLastTangent ;
}
//=======================================================================
//function : FixTangent
//purpose  : 
//=======================================================================

Handle(Geom_BSplineCurve)  GeomLib_CheckBSplineCurve::FixedTangent(const Standard_Boolean FirstFlag,
								 const Standard_Boolean LastFlag)
{ 
  Handle(Geom_BSplineCurve) new_curve ;
  if ((myFixFirstTangent && FirstFlag) ||(myFixLastTangent && LastFlag)) {
    new_curve =
      Handle(Geom_BSplineCurve)::DownCast(myCurve->Copy()) ;
    
  }
  if (myFixFirstTangent && FirstFlag) {
    new_curve->SetPole(2,
		     myFirstPole) ;
  }
  if (myFixLastTangent && LastFlag) {
    Standard_Integer num_poles = myCurve->NbPoles() ;
    new_curve->SetPole(num_poles-1,
		     myLastPole) ;
  }
  
  myDone = Standard_True ;
  return new_curve ;
}				   
//=======================================================================
//function : FixTangent
//purpose  : 
//=======================================================================

void GeomLib_CheckBSplineCurve::FixTangent(const Standard_Boolean FirstFlag,
					   const Standard_Boolean LastFlag)
{ 
  
  if (myFixFirstTangent && FirstFlag) {
    myCurve->SetPole(2,
		     myFirstPole) ;
  }
  if (myFixLastTangent && LastFlag) {
    Standard_Integer num_poles = myCurve->NbPoles() ;
    myCurve->SetPole(num_poles-1,
		     myLastPole) ;
  }
  
  myDone = Standard_True ;
}				   
