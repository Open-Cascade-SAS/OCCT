// Created on: 1993-08-27
// Created by: Jean-Louis FRENKEL
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



#include <Prs3d.ixx>

Standard_Boolean Prs3d::MatchSegment 
                 (const Quantity_Length X,
		  const Quantity_Length Y,
		  const Quantity_Length Z,
		  const Quantity_Length aDistance,
		  const gp_Pnt& P1,
		  const gp_Pnt& P2,
                  Quantity_Length& dist) {

		   Standard_Real X1,Y1,Z1,X2,Y2,Z2;
  P1.Coord(X1,Y1,Z1); P2.Coord(X2,Y2,Z2);
  Standard_Real DX = X2-X1; 
  Standard_Real DY = Y2-Y1; 
  Standard_Real DZ = Z2-Z1;
  Standard_Real Dist = DX*DX + DY*DY + DZ*DZ;
  if (Dist == 0.) return Standard_False;
  
  Standard_Real Lambda = ((X-X1)*DX + (Y-Y1)*DY + (Z-Z1)*DZ)/Dist;
  if ( Lambda < 0. || Lambda > 1. ) return Standard_False;
  dist =  Abs(X-X1-Lambda*DX) +
	  Abs(Y-Y1-Lambda*DY) + 
	  Abs(Z-Z1-Lambda*DZ);
  return (dist < aDistance);

}
				      
