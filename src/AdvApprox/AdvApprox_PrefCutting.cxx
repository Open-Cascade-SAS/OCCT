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



#include <AdvApprox_PrefCutting.ixx>
#include <Precision.hxx>

AdvApprox_PrefCutting::AdvApprox_PrefCutting(const TColStd_Array1OfReal& CutPnts):
  myPntOfCutting(1, CutPnts.Length()) 
{
  myPntOfCutting = CutPnts;
}

Standard_Boolean AdvApprox_PrefCutting::Value(const Standard_Real a,
					      const Standard_Real b,
					      Standard_Real& cuttingvalue) const
{
//  longueur minimum d'un intervalle parametrique : PConfusion()
//                                    pour F(U,V) : EPS1=1.e-9 (cf.MMEPS1)
  Standard_Real lgmin = 10 * Precision::PConfusion();
  Standard_Integer i;
  Standard_Real cut, mil=(a+b)/2,
                dist = Abs((a-b)/2);
  cut = mil;
  for ( i=myPntOfCutting.Lower(); i<= myPntOfCutting.Upper(); i++) {
    if ((dist-lgmin) > Abs(mil-myPntOfCutting.Value(i))) {
      cut = myPntOfCutting.Value(i);
      dist = Abs(mil-myPntOfCutting.Value(i));
    }
  }
  cuttingvalue = cut;
  return (Abs(cut-a)>=lgmin && Abs(b-cut)>=lgmin);
}
