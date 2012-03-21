// Created on: 1992-10-07
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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


#include <IntAna2d_IntPoint.ixx>
#include <Standard_DomainError.hxx>

IntAna2d_IntPoint::IntAna2d_IntPoint (const Standard_Real X, const Standard_Real Y, 
				      const Standard_Real U1,const Standard_Real U2):
       myu1(U1),myu2(U2),myp(X,Y),myimplicit(Standard_False)
 {
 }


IntAna2d_IntPoint::IntAna2d_IntPoint (const Standard_Real X, const Standard_Real Y, 
				      const Standard_Real U1):
       myu1(U1),myp(X,Y),myimplicit(Standard_True)
 {
 }

IntAna2d_IntPoint::IntAna2d_IntPoint ():

  myu1(RealLast()),myu2(RealLast()),myp(RealLast(),RealLast()),myimplicit(Standard_False)
  {
  }

void IntAna2d_IntPoint::SetValue (const Standard_Real X, const Standard_Real Y, 
				  const Standard_Real U1, const Standard_Real U2) {

  myimplicit = Standard_False;
  myp.SetCoord(X,Y);
  myu1 = U1;
  myu2 = U2;

}


void IntAna2d_IntPoint::SetValue (const Standard_Real X, const Standard_Real Y, const Standard_Real U1) {

  myimplicit = Standard_True;
  myp.SetCoord(X,Y);
  myu1 = U1;

}


