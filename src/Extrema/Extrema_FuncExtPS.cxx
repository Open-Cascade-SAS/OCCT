// Created on: 1995-07-18
// Created by: Modelistation
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


//  Modified by skv - Thu Sep 30 15:21:07 2004 OCC593


#include <Extrema_FuncExtPS.ixx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Precision.hxx>
#include <GeomAbs_IsoType.hxx>

Extrema_FuncExtPS::Extrema_FuncExtPS ()
{
  myPinit = Standard_False;
  mySinit = Standard_False;
}

//=============================================================================
Extrema_FuncExtPS::Extrema_FuncExtPS (const gp_Pnt& P,
                                      const Adaptor3d_Surface& S)
{
  myP = P;
  myS = (Adaptor3d_SurfacePtr)&S;
  GeomAbs_SurfaceType aSType = S.GetType();
  myPinit = Standard_True;
  mySinit = Standard_True;
}

//=============================================================================
void Extrema_FuncExtPS::Initialize(const Adaptor3d_Surface& S)
{
  myS = (Adaptor3d_SurfacePtr)&S;
  GeomAbs_SurfaceType aSType = S.GetType();
  mySinit = Standard_True;
  myPoint.Clear();
  mySqDist.Clear();
}

//=============================================================================

void Extrema_FuncExtPS::SetPoint(const gp_Pnt& P)
{
  myP = P;
  myPinit = Standard_True;
  myPoint.Clear();
  mySqDist.Clear();
}

//=============================================================================

//=============================================================================

Standard_Integer Extrema_FuncExtPS::NbVariables () const { return 2;}
//=============================================================================

Standard_Integer Extrema_FuncExtPS::NbEquations () const { return 2;}
//=============================================================================

Standard_Boolean Extrema_FuncExtPS::Value (const math_Vector& UV, 
                                           math_Vector& F)
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  myU = UV(1);
  myV = UV(2);
  gp_Vec Dus, Dvs;
  myS->D1(myU,myV,myPs,Dus,Dvs);

  gp_Vec PPs (myP,myPs);

  F(1) = PPs.Dot(Dus);
  F(2) = PPs.Dot(Dvs);

  return Standard_True;
}
//=============================================================================

Standard_Boolean Extrema_FuncExtPS::Derivatives (const math_Vector& UV, 
                                                 math_Matrix& Df)
{
  math_Vector F(1,2);
  return Values(UV,F,Df);
}
//=============================================================================

Standard_Boolean Extrema_FuncExtPS::Values (const math_Vector& UV, 
                                            math_Vector& F,
                                            math_Matrix& Df)
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  myU = UV(1);
  myV = UV(2);
  gp_Vec Dus, Dvs, Duus, Dvvs, Duvs;
  myS->D2(myU,myV,myPs,Dus,Dvs,Duus,Dvvs,Duvs);

  gp_Vec PPs (myP,myPs);

  Df(1,1) = Dus.SquareMagnitude() + PPs.Dot(Duus);
  Df(1,2) = Dvs.Dot(Dus)          + PPs.Dot(Duvs);
  Df(2,1) = Df(1,2);
  Df(2,2) = Dvs.SquareMagnitude() + PPs.Dot(Dvvs);

  // 3. Value
  F(1) = PPs.Dot(Dus);
  F(2) = PPs.Dot(Dvs);

  return Standard_True;
}
//=============================================================================

Standard_Integer Extrema_FuncExtPS::GetStateNumber ()
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  //comparison of solution with previous solutions
  Standard_Integer i = 1, nbSol = mySqDist.Length();
  Standard_Real tol2d = Precision::PConfusion() * Precision::PConfusion();
   
  for( ; i <=  nbSol; i++)
  {
    Standard_Real aU, aV;
	myPoint(i).Parameter(aU, aV);
	if( ((myU - aU ) * (myU - aU ) + (myV - aV ) * (myV - aV )) <= tol2d )
      break;
  }
  if( i <= nbSol)
	  return 0;
  mySqDist.Append(myPs.SquareDistance(myP));
  myPoint.Append(Extrema_POnSurf(myU,myV,myPs));
  return 0;
}
//=============================================================================

Standard_Integer Extrema_FuncExtPS::NbExt () const
{
  return mySqDist.Length();
}
//=============================================================================

Standard_Real Extrema_FuncExtPS::SquareDistance (const Standard_Integer N) const
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  return mySqDist.Value(N);
}
//=============================================================================

Extrema_POnSurf Extrema_FuncExtPS::Point (const Standard_Integer N) const
{
  if (!myPinit || !mySinit) Standard_TypeMismatch::Raise();
  return myPoint.Value(N);
}
