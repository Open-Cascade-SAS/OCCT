// Created on: 1997-11-06
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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



#include <ProjLib_PrjResolve.ixx>
#include <ProjLib_PrjFunc.hxx>
#include <math_FunctionSetRoot.hxx>
#include <math_NewtonFunctionSetRoot.hxx>

ProjLib_PrjResolve::ProjLib_PrjResolve(const Adaptor3d_Curve& C,const Adaptor3d_Surface& S,const Standard_Integer Fix) : myFix(Fix)
{
  if (myFix > 3 || myFix < 1) Standard_ConstructionError::Raise();
  mySolution = gp_Pnt2d(0.,0.);
  myCurve    = (Adaptor3d_CurvePtr)&C;
  mySurface  = (Adaptor3d_SurfacePtr)&S; 
}

// void ProjLib_PrjResolve::Perform(const Standard_Real t, const Standard_Real U, const Standard_Real  V, const gp_Pnt2d& Tol2d, const gp_Pnt2d& Inf, const gp_Pnt2d& Sup, const Standard_Real FuncTol, const Standard_Boolean StrictInside)
 void ProjLib_PrjResolve::Perform(const Standard_Real t, const Standard_Real U, const Standard_Real  V, const gp_Pnt2d& Tol2d, const gp_Pnt2d& Inf, const gp_Pnt2d& Sup, const Standard_Real FuncTol, const Standard_Boolean )
{

  myDone = Standard_False;
  Standard_Real FixVal = 0.;
  gp_Pnt2d ExtInf(0.,0.), ExtSup(0.,0.);
  Standard_Real ExtU = 10*Tol2d.X(), ExtV = 10*Tol2d.Y();
  math_Vector Tol(1, 2), Start(1, 2), BInf(1, 2), BSup(1, 2);

  ExtInf.SetCoord(Inf.X() - ExtU, Inf.Y() - ExtV);
  ExtSup.SetCoord(Sup.X() + ExtU, Sup.Y() + ExtV);
  BInf(1) = ExtInf.X();
  BInf(2) = ExtInf.Y();
  BSup(1) = ExtSup.X();
  BSup(2) = ExtSup.Y();
  Tol(1) = Tol2d.X();
  Tol(2) = Tol2d.Y();
 
  switch(myFix) {
  case 1:
    Start(1) = U;
    Start(2) = V;
    FixVal = t;
    break;
  case 2:
    Start(1) = t;
    Start(2) = V;
    FixVal = U;
    break;
  case 3:
    Start(1) = t;
    Start(2) = U;
    FixVal = V;
  }

  ProjLib_PrjFunc F(myCurve, FixVal, mySurface, myFix);
  

//  Standard_Integer option = 1;//2;
//  if (option == 1) {
//    math_FunctionSetRoot S1 (F, Start,Tol, BInf, BSup);
//    if (!S1.IsDone()) { return; }
//  }
//  else {
    math_NewtonFunctionSetRoot SR (F, Tol, 1.e-10);
    SR.Perform(F, Start, BInf, BSup);
//    if (!SR.IsDone()) { return; }
    if (!SR.IsDone()) {
        math_FunctionSetRoot S1 (F, Start,Tol, BInf, BSup);
        if (!S1.IsDone()) { return; }
    }
   


  mySolution.SetXY(F.Solution().XY());

// computation of myDone
  myDone = Standard_True;

  Standard_Real ExtraU , ExtraV;
//  if(!StrictInside) {
    ExtraU = Tol2d.X();
    ExtraV = Tol2d.Y();
//  }
  if (Abs(mySolution.X()-Inf.X()) < Tol2d.X()) mySolution.SetX(Inf.X());
  if (Abs(mySolution.X()-Sup.X()) < Tol2d.X()) mySolution.SetX(Sup.X()); 
  if (Abs(mySolution.Y()-Inf.Y()) < Tol2d.Y()) mySolution.SetY(Inf.Y());
  if (Abs(mySolution.Y()-Sup.Y()) < Tol2d.Y()) mySolution.SetY(Sup.Y()); 
  if (mySolution.X() < Inf.X() - ExtraU || 
      mySolution.X() > Sup.X() + ExtraU ||
      mySolution.Y() < Inf.Y() - ExtraV || 
      mySolution.Y() > Sup.Y() + ExtraV) myDone = Standard_False;
  else if (FuncTol > 0) {
    math_Vector X(1,2,0.), FVal(1,2,0.);
    X(1) = mySolution.X();
    X(2) = mySolution.Y();

    
    F.Value(X, FVal);   

    if ((FVal(1)*FVal(1) + FVal(2)*FVal(2)) > FuncTol) myDone = Standard_False;
  }


}

 Standard_Boolean ProjLib_PrjResolve::IsDone() const
{
  return myDone;
}

 gp_Pnt2d ProjLib_PrjResolve::Solution() const
{
  if (!IsDone())  StdFail_NotDone::Raise(); 
  return mySolution;
}
