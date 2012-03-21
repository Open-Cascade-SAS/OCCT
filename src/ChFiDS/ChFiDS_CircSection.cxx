// Created on: 1996-03-06
// Created by: Laurent BOURESCHE
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



#include <ChFiDS_CircSection.ixx>

//=======================================================================
//function : ChFiDS_CircSection
//purpose  : 
//=======================================================================

ChFiDS_CircSection::ChFiDS_CircSection(){}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Set(const gp_Circ&      C,
			     const Standard_Real F,
			     const Standard_Real L)
{
  myCirc = C;
  myF    = F;
  myL    = L;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Set(const gp_Lin&      C,
			     const Standard_Real F,
			     const Standard_Real L)
{
  myLin  = C;
  myF    = F;
  myL    = L;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Get(gp_Circ&       C,
			     Standard_Real& F,
			     Standard_Real& L) const 
{
  C = myCirc;
  F = myF;
  L = myL;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

void ChFiDS_CircSection::Get(gp_Lin&        C,
			     Standard_Real& F,
			     Standard_Real& L) const 
{
  C = myLin;
  F = myF;
  L = myL;
}
