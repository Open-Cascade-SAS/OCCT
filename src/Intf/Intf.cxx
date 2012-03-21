// Created on: 1993-01-20
// Created by: Didier PIFFAULT
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


#include <Intf.ixx>

//=======================================================================
//function : PlaneEquation
//purpose  : 
//=======================================================================

void Intf::PlaneEquation (const gp_Pnt&  P1,
			  const gp_Pnt&  P2,
			  const gp_Pnt&  P3,
			  gp_XYZ&        NormalVector,
			  Standard_Real& PolarDistance)
{
  gp_XYZ v1=P2.XYZ()-P1.XYZ();
  gp_XYZ v2=P3.XYZ()-P2.XYZ();
  gp_XYZ v3=P1.XYZ()-P3.XYZ();
  NormalVector= (v1^v2)+(v2^v3)+(v3^v1);
  NormalVector.Normalize();
  PolarDistance = NormalVector * P1.XYZ();
}


//=======================================================================
//function : Contain
//purpose  : 
//=======================================================================

Standard_Boolean Intf::Contain (const gp_Pnt&  P1,
				const gp_Pnt&  P2,
				const gp_Pnt&  P3,
				const gp_Pnt& ThePnt)
{
  gp_XYZ v1=(P2.XYZ()-P1.XYZ())^(ThePnt.XYZ()-P1.XYZ());
  gp_XYZ v2=(P3.XYZ()-P2.XYZ())^(ThePnt.XYZ()-P2.XYZ());
  gp_XYZ v3=(P1.XYZ()-P3.XYZ())^(ThePnt.XYZ()-P3.XYZ());
  if (v1*v2 >= 0. && v2*v3 >= 0. && v3*v1>=0.) return Standard_True;
  else                                         return Standard_False;
}
