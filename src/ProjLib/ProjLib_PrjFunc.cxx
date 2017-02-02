// Created on: 1997-11-06
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <ProjLib_PrjFunc.hxx>
#include <Standard_ConstructionError.hxx>

ProjLib_PrjFunc::ProjLib_PrjFunc(const Adaptor3d_CurvePtr & C,const Standard_Real FixVal,const Adaptor3d_SurfacePtr & S, const Standard_Integer Fix) : myCurve(C), mySurface(S), myt(0), myU(0), myV(0), myFix(Fix)
{
  myNorm=Min(1.,Min(mySurface->UResolution(1.),mySurface->VResolution(1.)));
// myNorm=1.;
  switch(myFix) {
  case 1: myt = FixVal; break;
  case 2: myU = FixVal; break;
  case 3: myV = FixVal; break;
  default: throw Standard_ConstructionError();
  }
}

 Standard_Integer ProjLib_PrjFunc::NbVariables() const
{
  return 2;
}

 Standard_Integer ProjLib_PrjFunc::NbEquations() const
{
  return 2;
}

 Standard_Boolean ProjLib_PrjFunc::Value(const math_Vector& X,math_Vector& F) 
{
  math_Matrix D (1,2,1,2);
  return Values(X,F,D);
}

 Standard_Boolean ProjLib_PrjFunc::Derivatives(const math_Vector& X,math_Matrix& D) 
{
  math_Vector F(1,2);
  return Values(X,F,D);
}

 Standard_Boolean ProjLib_PrjFunc::Values(const math_Vector& X,math_Vector& F,math_Matrix& D) 
{
  Standard_Real u = 0., v = 0., t = 0.;
  switch(myFix) {
  case 1:
    t = myt;
    u = X(1);
    v = X(2);
    break;
  case 2:
    t = X(1);
    u = myU;
    v = X(2);
    break;
  case 3:
    t = X(1);
    u = X(2);
    v = myV;
  }

/*  if(X(1) > mySup.X() || X(1) < myInf.X()) return Standard_False;
  if(X(2) > mySup.Y() || X(2) < myInf.Y()) return Standard_False;
*/  
  gp_Pnt S, C;
  gp_Vec DS1_u, DS1_v, DS2_u, DS2_uv, DS2_v, DC1_t;
  myCurve->D1(t, C, DC1_t);
  mySurface->D2(u, v, S, DS1_u, DS1_v, DS2_u, DS2_v, DS2_uv);
  
  gp_Vec V(C, S);
  
  F(1) = V*DS1_u*myNorm;
  F(2) = V*DS1_v*myNorm;

  switch(myFix) {
  case 1:
    D(1, 1) = (DS1_u.SquareMagnitude() + V*DS2_u)*myNorm; // dE1/du
    D(1, 2) = (DS1_v*DS1_u + V*DS2_uv)*myNorm; // dE1/dv
    D(2, 1) = D(1, 2); // dE2/du
    D(2, 2) = (DS1_v.SquareMagnitude() + V*DS2_v)*myNorm; // dE2/dv
    break;
  case 2:
    D(1, 1) = (-DC1_t*DS1_u)*myNorm; // dE1/dt
    D(1, 2) = (DS1_v*DS1_u + V*DS2_uv)*myNorm; // dE1/dv
    D(2, 1) = (-DC1_t*DS1_v)*myNorm; // dE2/dt
    D(2, 2) = (DS1_v.SquareMagnitude() + V*DS2_v)*myNorm; // dE2/dv
    break;
  case 3:
    D(1, 1) = -DC1_t*DS1_u*myNorm; // dE1/dt
    D(1, 2) = (DS1_u.SquareMagnitude() + V*DS2_u)*myNorm; // dE1/du
    D(2, 1) = -DC1_t*DS1_v*myNorm; // dE2/dt
    D(2, 2) = (DS1_v*DS1_u + V*DS2_uv)*myNorm;// dE2/du
  }

/*  cout<<"F = ("<<F(1)<<", "<<F(2)<<")"<<endl;
  cout<<"dE1/dt = "<<D(1,1)<<endl;
  cout<<"dE1/dv = "<<D(1,2)<<endl;
  cout<<"dE2/dt = "<<D(2,1)<<endl;
  cout<<"dE2/dv = "<<D(2,2)<<endl;
*/


  myU = u;
  myV = v;
  myt = t;

  return Standard_True;
}

gp_Pnt2d ProjLib_PrjFunc::Solution() const
{
  switch(myFix) {
  case 1: return gp_Pnt2d(myU, myV);
  case 2: return gp_Pnt2d(myt, myV);
  case 3: return gp_Pnt2d(myt, myU);
  }
  // pout NT, meme si on n'y passe pas.
  return gp_Pnt2d(0.,0.);
}


