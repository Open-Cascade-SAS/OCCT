// Created on: 1995-07-18
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Extrema_FuncPSNorm.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Extrema_POnSurf.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <math_Matrix.hxx>
#include <Precision.hxx>
#include <Standard_TypeMismatch.hxx>

Extrema_FuncPSNorm::Extrema_FuncPSNorm()
    : myS(NULL),
      myU(0.0),
      myV(0.0)
{
  myPinit = false;
  mySinit = false;
}

//=============================================================================
Extrema_FuncPSNorm::Extrema_FuncPSNorm(const gp_Pnt& P, const Adaptor3d_Surface& S)
    : myU(0.0),
      myV(0.0)
{
  myP     = P;
  myS     = &S;
  myPinit = true;
  mySinit = true;
}

//=============================================================================
void Extrema_FuncPSNorm::Initialize(const Adaptor3d_Surface& S)
{
  myS     = &S;
  mySinit = true;
  myPoint.Clear();
  mySqDist.Clear();
}

//=============================================================================

void Extrema_FuncPSNorm::SetPoint(const gp_Pnt& P)
{
  myP     = P;
  myPinit = true;
  myPoint.Clear();
  mySqDist.Clear();
}

//=============================================================================

//=============================================================================

int Extrema_FuncPSNorm::NbVariables() const
{
  return 2;
}

//=============================================================================

int Extrema_FuncPSNorm::NbEquations() const
{
  return 2;
}

//=============================================================================

bool Extrema_FuncPSNorm::Value(const math_Vector& UV, math_Vector& F)
{
  if (!myPinit || !mySinit)
    throw Standard_TypeMismatch();
  myU = UV(1);
  myV = UV(2);
  gp_Vec Dus, Dvs;
  myS->D1(myU, myV, myPs, Dus, Dvs);

  gp_Vec PPs(myP, myPs);

  F(1) = PPs.Dot(Dus);
  F(2) = PPs.Dot(Dvs);

  return true;
}

//=============================================================================

bool Extrema_FuncPSNorm::Derivatives(const math_Vector& UV, math_Matrix& Df)
{
  math_Vector F(1, 2);
  return Values(UV, F, Df);
}

//=============================================================================

bool Extrema_FuncPSNorm::Values(const math_Vector& UV, math_Vector& F, math_Matrix& Df)
{
  if (!myPinit || !mySinit)
    throw Standard_TypeMismatch();
  myU = UV(1);
  myV = UV(2);
  gp_Vec Dus, Dvs, Duus, Dvvs, Duvs;
  myS->D2(myU, myV, myPs, Dus, Dvs, Duus, Dvvs, Duvs);

  gp_Vec PPs(myP, myPs);

  Df(1, 1) = Dus.SquareMagnitude() + PPs.Dot(Duus);
  Df(1, 2) = Dvs.Dot(Dus) + PPs.Dot(Duvs);
  Df(2, 1) = Df(1, 2);
  Df(2, 2) = Dvs.SquareMagnitude() + PPs.Dot(Dvvs);

  // 3. Value
  F(1) = PPs.Dot(Dus);
  F(2) = PPs.Dot(Dvs);

  return true;
}

//=============================================================================

int Extrema_FuncPSNorm::GetStateNumber()
{
  if (!myPinit || !mySinit)
    throw Standard_TypeMismatch();
  // comparison of solution with previous solutions
  int              i = 1, nbSol = mySqDist.Length();
  constexpr double tol2d = Precision::PConfusion() * Precision::PConfusion();

  for (; i <= nbSol; i++)
  {
    double aU, aV;
    myPoint(i).Parameter(aU, aV);
    if (((myU - aU) * (myU - aU) + (myV - aV) * (myV - aV)) <= tol2d)
      break;
  }
  if (i <= nbSol)
    return 0;
  mySqDist.Append(myPs.SquareDistance(myP));
  myPoint.Append(Extrema_POnSurf(myU, myV, myPs));
  return 0;
}

//=============================================================================

int Extrema_FuncPSNorm::NbExt() const
{
  return mySqDist.Length();
}

//=============================================================================

double Extrema_FuncPSNorm::SquareDistance(const int N) const
{
  if (!myPinit || !mySinit)
    throw Standard_TypeMismatch();
  return mySqDist.Value(N);
}

//=============================================================================

const Extrema_POnSurf& Extrema_FuncPSNorm::Point(const int N) const
{
  if (!myPinit || !mySinit)
    throw Standard_TypeMismatch();
  return myPoint.Value(N);
}
