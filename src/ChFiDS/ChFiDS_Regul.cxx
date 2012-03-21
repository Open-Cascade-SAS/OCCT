// Created on: 1995-03-21
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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



#include <ChFiDS_Regul.ixx>

//=======================================================================
//function : ChFiDS_Regul
//purpose  : 
//=======================================================================

ChFiDS_Regul::ChFiDS_Regul()
{
}


//=======================================================================
//function : SetCurve
//purpose  : 
//=======================================================================

void ChFiDS_Regul::SetCurve(const Standard_Integer IC)
{
  icurv = Abs(IC);
}


//=======================================================================
//function : SetS1
//purpose  : 
//=======================================================================

void ChFiDS_Regul::SetS1(const Standard_Integer IS1, 
			const Standard_Boolean IsFace)
{
  if(IsFace) is1 = Abs(IS1);
  else is1 = -Abs(IS1);
}


//=======================================================================
//function : SetS2
//purpose  : 
//=======================================================================

void ChFiDS_Regul::SetS2(const Standard_Integer IS2, 
			const Standard_Boolean IsFace)
{
  if(IsFace) is2 = Abs(IS2);
  else is2 = -Abs(IS2);
}


//=======================================================================
//function : IsSurface1
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_Regul::IsSurface1() const 
{
  return (is1<0);
}


//=======================================================================
//function : IsSurface2
//purpose  : 
//=======================================================================

Standard_Boolean ChFiDS_Regul::IsSurface2() const 
{
  return (is2<0);
}


//=======================================================================
//function : Curve
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Regul::Curve() const 
{
  return icurv;
}


//=======================================================================
//function : S1
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Regul::S1() const 
{
  return Abs(is1);
}


//=======================================================================
//function : S2
//purpose  : 
//=======================================================================

Standard_Integer ChFiDS_Regul::S2() const 
{
  return Abs(is2);
}


