// Created on: 1998-02-18
// Created by: Jeanine PANCIATICI
// Copyright (c) 1998-1999 Matra Datavision
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



#include <Adaptor3d_InterFunc.ixx>
#include <Adaptor2d_HCurve2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

Adaptor3d_InterFunc::Adaptor3d_InterFunc(const Handle(Adaptor2d_HCurve2d)& C, const Standard_Real FixVal, const Standard_Integer Fix) : myCurve2d(C),myFixVal(FixVal),myFix(Fix)
{
  if(Fix != 1 && Fix != 2 ) Standard_ConstructionError::Raise();

}

Standard_Boolean Adaptor3d_InterFunc::Value(const Standard_Real X , Standard_Real& F)
{
   gp_Pnt2d C;
   myCurve2d->D0(X,C);
   if(myFix == 1) 
      F=C.X()-myFixVal;
   else
      F=C.Y()-myFixVal;
 
   return Standard_True;
}
Standard_Boolean Adaptor3d_InterFunc::Derivative(const Standard_Real X , Standard_Real& D)
{
   Standard_Real F;
   return Values(X,F,D);
}
Standard_Boolean Adaptor3d_InterFunc::Values(const Standard_Real X , Standard_Real& F,Standard_Real& D)
{
    gp_Pnt2d C;
    gp_Vec2d DC;
    myCurve2d->D1(X,C,DC);
    if(myFix == 1) { 
      F=C.X()-myFixVal;
      D=DC.X();}
    else {
      F=C.Y()-myFixVal;
      D=DC.Y();}  
    return Standard_True;
}
