// Created on: 1996-07-03
// Created by: Joelle CHAUVET
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



#include <AdvApprox_DichoCutting.ixx>
#include <Precision.hxx>

AdvApprox_DichoCutting::AdvApprox_DichoCutting()
{
}

Standard_Boolean AdvApprox_DichoCutting::Value(const Standard_Real a,
						const Standard_Real b,
						Standard_Real& cuttingvalue) const
{
//  longueur minimum d'un intervalle pour F(U,V) : EPS1=1.e-9 (cf.MEPS1)
  Standard_Real lgmin = 10*Precision::PConfusion();
  cuttingvalue = (a+b) / 2;
  return (Abs(b-a)>=2*lgmin);
}

