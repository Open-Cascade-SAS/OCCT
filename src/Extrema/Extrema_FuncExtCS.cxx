// Created on: 1996-01-09
// Created by: Laurent PAINNOT
// Copyright (c) 1996-1999 Matra Datavision
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
#include <Extrema_FuncExtCS.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>

/*-----------------------------------------------------------------------------
 Fonction permettant de rechercher une distance extremale entre une courbe C 
et une surface S.
 Cette classe herite de math_FunctionWithDerivative et est utilisee par
les algorithmes math_FunctionRoot et math_FunctionRoots.
{ F1(t,u,v) = (C(t)-S(u,v)).Dtc(t) }
{ F2(t,u,v) = (C(t)-S(u,v)).Dus(u,v) }
{ F3(t,u,v) = (C(t)-S(u,v)).Dvs(u,v) }
{ Dtf1(t,u,v) = Dtc(t).Dtc(t)+(C(t)-S(u,v)).Dttc(t) 
              = ||Dtc(t)||**2+(C(t)-S(u,v)).Dttc(t) }
{ Duf1(t,u,v) = -Dus(u,v).Dtc(t) }
{ Dvf1(t,u,v) = -Dvs(u,v).Dtc(t) }
{ Dtf2(t,u,v) = Dtc(t).Dus(u,v) }
{ Duf2(t,u,v) = -Dus(u,v).Dus(u,v)+(C(t)-S(u,v)).Duus(u,v)
              = -||Dus(u,v)||**2+(C(t)-S(u,v)).Duus(u,v) }
{ Dvf2(t,u,v) = -Dvs(u,v).Dus(u,v)+(C(t)-S(u,v)).Duvs(u,v) }
{ Dtf3(t,u,v) = Dtc(t).Dvs(u,v) }
{ Duf3(t,u,v) = -Dus(u,v).Dvs(u,v)+(C(t)-S(u,v)).Duvs(u,v) }
{ Dvf3(t,u,v) = -Dvs(u,v).Dvs(u,v)+(C(t)-S(u,v)).Dvvs(u,v) }
----------------------------------------------------------------------------*/
//=======================================================================
//function : Extrema_FuncExtCS
//purpose  : 
//=======================================================================
 Extrema_FuncExtCS::Extrema_FuncExtCS()
{
  myCinit = Standard_False;
  mySinit = Standard_False;
}

//=======================================================================
//function : Extrema_FuncExtCS
//purpose  : 
//=======================================================================

 Extrema_FuncExtCS::Extrema_FuncExtCS(const Adaptor3d_Curve& C, 
				      const Adaptor3d_Surface& S)
{
  Initialize(C, S);
}

//=======================================================================
//function : Initialize
//purpose  : 
//=======================================================================

void Extrema_FuncExtCS::Initialize(const Adaptor3d_Curve& C, 
				   const Adaptor3d_Surface& S)
{
  myC = (Adaptor3d_CurvePtr)&C;
  myS = (Adaptor3d_SurfacePtr)&S;
  myCinit = Standard_True;
  mySinit = Standard_True;
  myPoint1.Clear();
  myPoint2.Clear();
  mySqDist.Clear();
}

//=======================================================================
//function : NbVariables
//purpose  : 
//=======================================================================

Standard_Integer Extrema_FuncExtCS::NbVariables() const 
{
  return (3);
}

//=======================================================================
//function : NbEquations
//purpose  : 
//=======================================================================

Standard_Integer Extrema_FuncExtCS::NbEquations() const 
{
  return (3);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_FuncExtCS::Value(const math_Vector& UV, 
					  math_Vector& F)
{
  if (!myCinit || !mySinit) throw Standard_TypeMismatch();

  myt = UV(1);
  myU = UV(2);
  myV = UV(3);

//  gp_Vec Dtc, Dttc;
  gp_Vec Dtc;
///  gp_Vec Dus, Dvs, Duvs, Duus, Dvvs;
  gp_Vec Dus, Dvs;
  myC->D1(myt, myP1, Dtc);
  myS->D1(myU,myV,myP2,Dus,Dvs);

  gp_Vec P1P2 (myP2,myP1);

  F(1) = P1P2.Dot(Dtc);
  F(2) = P1P2.Dot(Dus);
  F(3) = P1P2.Dot(Dvs);
  
  return  Standard_True;
}

//=======================================================================
//function : Derivatives
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_FuncExtCS::Derivatives(const math_Vector& UV, 
						math_Matrix& DF)
{
  math_Vector F(1,3);
  return Values(UV,F,DF);
}

//=======================================================================
//function : Values
//purpose  : 
//=======================================================================

Standard_Boolean Extrema_FuncExtCS::Values(const math_Vector& UV, 
					   math_Vector& F, 
					   math_Matrix& Df)
{
  if (!myCinit || !mySinit) throw Standard_TypeMismatch();

  myt = UV(1);
  myU = UV(2);
  myV = UV(3);

  gp_Vec Dtc, Dttc;
  gp_Vec Dus, Dvs, Duvs, Duus, Dvvs;
  myC->D2(myt, myP1, Dtc, Dttc);
  myS->D2(myU,myV,myP2,Dus,Dvs,Duus,Dvvs,Duvs);

  gp_Vec P1P2 (myP2,myP1);

  F(1) = P1P2.Dot(Dtc);
  F(2) = P1P2.Dot(Dus);
  F(3) = P1P2.Dot(Dvs);

  Df(1,1) = Dtc.SquareMagnitude() + P1P2.Dot(Dttc);
  Df(1,2) = -Dus.Dot(Dtc);
  Df(1,3) = -Dvs.Dot(Dtc);

  Df(2,1) = -Df(1, 2);   // Dtc.Dot(Dus);
  Df(2,2) = -Dus.SquareMagnitude()+P1P2.Dot(Duus);
  Df(2,3) = -Dvs.Dot(Dus)+P1P2.Dot(Duvs);

  Df(3,1) = -Df(1,3);    // Dtc.Dot(Dvs);
  Df(3,2) = Df(2,3);     // -Dus.Dot(Dvs)+P1P2.Dot(Duvs);
  Df(3,3) = -Dvs.SquareMagnitude()+P1P2.Dot(Dvvs);

  return Standard_True;

}

//=======================================================================
//function : GetStateNumber
//purpose  : 
//=======================================================================

Standard_Integer Extrema_FuncExtCS::GetStateNumber()
{
  if (!myCinit || !mySinit) throw Standard_TypeMismatch();
#if 0
  math_Vector Sol(1, 3), UVSol(1, 3);
  UVSol(1) = myt; UVSol(2) = myU; UVSol(3) = myV;
  Value(UVSol, Sol);
  cout <<"F(1)= "<<Sol(1)<<" F(2)= "<<Sol(2)<<" F(3)= "<<Sol(3)<<endl;
#endif
  //comparison of solution with previous solutions
  Standard_Real tol2d = Precision::PConfusion() * Precision::PConfusion();
  Standard_Integer i = 1, nbSol = mySqDist.Length();
  for( ; i <=  nbSol; i++)
  {
    Standard_Real aT = myPoint1(i).Parameter();
    if( (myt - aT) * (myt - aT) <= tol2d )
      break;
  }
  if (i <= nbSol)
    return 0;
  mySqDist.Append(myP1.SquareDistance(myP2));
  myPoint1.Append(Extrema_POnCurv(myt,myP1));
  myPoint2.Append(Extrema_POnSurf(myU,myV,myP2));
  return 0;
}

//=======================================================================
//function : NbExt
//purpose  : 
//=======================================================================

Standard_Integer Extrema_FuncExtCS::NbExt() const 
{
  return mySqDist.Length();
}

//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================

Standard_Real Extrema_FuncExtCS::SquareDistance(const Standard_Integer N) const 
{
  if (!myCinit || !mySinit) throw Standard_TypeMismatch();
  return mySqDist.Value(N);
}

//=======================================================================
//function : PointOnCurve
//purpose  : 
//=======================================================================

const Extrema_POnCurv& Extrema_FuncExtCS::PointOnCurve(const Standard_Integer N) const 
{
  if (!myCinit || !mySinit) throw Standard_TypeMismatch();
  return myPoint1.Value(N);
}

//=======================================================================
//function : PointOnSurface
//purpose  : 
//=======================================================================

const Extrema_POnSurf& Extrema_FuncExtCS::PointOnSurface(const Standard_Integer N) const 
{
  if (!myCinit || !mySinit) throw Standard_TypeMismatch();
  return myPoint2.Value(N);
}

//=======================================================================
//function : Bidon1
//purpose  : 
//=======================================================================

Adaptor3d_SurfacePtr Extrema_FuncExtCS::Bidon1() const 
{
  return (Adaptor3d_SurfacePtr)0L;
}

//=======================================================================
//function : Bidon2
//purpose  : 
//=======================================================================

Adaptor3d_CurvePtr Extrema_FuncExtCS::Bidon2() const 
{
  return (Adaptor3d_CurvePtr)0L;
}

