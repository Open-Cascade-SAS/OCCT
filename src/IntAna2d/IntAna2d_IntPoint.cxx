// File:	IntPoint.cxx
// Created:	Wed Oct 7  9:43:20 1992
// Author:	Laurent BUCHARD
//		<lbr@topsn2>
//-Copyright:	 Matra Datavision 1992

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


