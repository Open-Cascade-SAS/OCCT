// File:	Adaptor3d_InterFunc.cxx
// Created:	Wed Feb 18 09:29:56 1998
// Author:	Jeanine PANCIATICI
//		<jpi@sgi38>


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
