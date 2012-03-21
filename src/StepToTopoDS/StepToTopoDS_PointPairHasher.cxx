// Created on: 1993-08-06
// Created by: Martine LANGLOIS
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



#include <StepToTopoDS_PointPairHasher.ixx>

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer StepToTopoDS_PointPairHasher::HashCode
  (const StepToTopoDS_PointPair& P, const Standard_Integer Upper)
{
  return (::HashCode(P.myP1,Upper) + ::HashCode(P.myP2,Upper)) % Upper;
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean StepToTopoDS_PointPairHasher::IsEqual
  (const StepToTopoDS_PointPair& P1,
   const StepToTopoDS_PointPair& P2)
{
  return (((P1.myP1 == P2.myP1) && (P1.myP2 == P2.myP2)) ||
	  ((P1.myP1 == P2.myP2) && (P1.myP2 == P2.myP1)));
}
