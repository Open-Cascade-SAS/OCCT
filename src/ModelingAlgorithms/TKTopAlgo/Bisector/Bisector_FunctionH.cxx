// Created on: 1994-04-05
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#include <Bisector_FunctionH.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp.hxx>

#include <algorithm>
#include <cmath>

namespace
{
//==================================================================================================

gp_Vec2d bisectorNormal(const gp_Vec2d& theGuide, const gp_Vec2d& theTangent)
{
  const double aCos = std::clamp(theGuide.Dot(theTangent), -1.0, 1.0);
  const double aSin = theGuide.Crossed(theTangent);
  double       aHalfSin, aHalfCos;
  if (aCos >= 0.0)
  {
    aHalfCos = std::sqrt(0.5 * (1.0 + aCos));
    aHalfSin = 0.5 * aSin / aHalfCos;
  }
  else
  {
    aHalfSin = std::copysign(std::sqrt(0.5 * (1.0 - aCos)), aSin);
    aHalfCos = 0.5 * aSin / aHalfSin;
  }
  const gp_Vec2d aNormal(-theGuide.Y(), theGuide.X());
  return aHalfSin * theGuide - aHalfCos * aNormal;
}
} // namespace

//==================================================================================================

Bisector_FunctionH::Bisector_FunctionH(const occ::handle<Geom2d_Curve>& C2,
                                       const gp_Pnt2d&                  P1,
                                       const gp_Vec2d&                  T1)
    : Bisector_FunctionH(C2, P1, T1, 0.5 * (C2->FirstParameter() + C2->LastParameter()))
{
}

//==================================================================================================

Bisector_FunctionH::Bisector_FunctionH(const occ::handle<Geom2d_Curve>& theCurve,
                                       const gp_Pnt2d&                  thePoint,
                                       const gp_Vec2d&                  theTangent,
                                       const double                     theReferenceParameter)
    : curve2(theCurve),
      p1(thePoint),
      t1(theTangent)
{
  t1.Normalize();
  const gp_Vec2d aTangent = curve2->EvalD1(theReferenceParameter).D1;
  const double   aSpeed   = aTangent.Magnitude();
  if (aSpeed > gp::Resolution())
  {
    myReference = bisectorNormal(t1, aTangent / aSpeed);
  }
}

//==================================================================================================

Bisector_FunctionH::Bisector_FunctionH(const Geom2dAdaptor_Curve& theCurve,
                                       const gp_Pnt2d&            thePoint,
                                       const gp_Vec2d&            theTangent,
                                       const double               theReferenceParameter)
    : curve2(theCurve.Curve()),
      p1(thePoint),
      t1(theTangent),
      myAdaptor(&theCurve)
{
  t1.Normalize();
  const gp_Vec2d aTangent = theCurve.EvalD1(theReferenceParameter).D1;
  const double   aSpeed   = aTangent.Magnitude();
  if (aSpeed > gp::Resolution())
  {
    myReference = bisectorNormal(t1, aTangent / aSpeed);
  }
}

//==================================================================================================

bool Bisector_FunctionH::Value(const double X, double& F)
{
  const auto [aPoint, aTangent] = myAdaptor != nullptr ? myAdaptor->EvalD1(X) : curve2->EvalD1(X);
  const double aSpeed           = aTangent.Magnitude();
  if (aSpeed <= gp::Resolution() || myReference.SquareMagnitude() == 0.0)
  {
    return false;
  }
  gp_Vec2d aNormal = bisectorNormal(t1, aTangent / aSpeed);
  // Keep the same normal orientation across antiparallel source tangents.
  if (aNormal.Dot(myReference) < 0.0)
  {
    aNormal.Reverse();
  }
  F = gp_Vec2d(aPoint, p1).Dot(aNormal);
  return true;
}

//==================================================================================================

bool Bisector_FunctionH::Derivative(const double X, double& D)
{
  double F;
  return Values(X, F, D);
}

//==================================================================================================

bool Bisector_FunctionH::Values(const double X, double& F, double& D)
{
  const auto [aPoint, aTangent, aD2] =
    myAdaptor != nullptr ? myAdaptor->EvalD2(X) : curve2->EvalD2(X);
  const double aSpeed = aTangent.Magnitude();
  if (aSpeed <= gp::Resolution() || myReference.SquareMagnitude() == 0.0)
  {
    return false;
  }
  gp_Vec2d aNormal = bisectorNormal(t1, aTangent / aSpeed);
  if (aNormal.Dot(myReference) < 0.0)
  {
    aNormal.Reverse();
  }
  // The tangent-bisector normal rotates at half the source tangent's rate.
  const double   aRotation = 0.5 * aTangent.Crossed(aD2) / (aSpeed * aSpeed);
  const gp_Vec2d aDerivative(-aRotation * aNormal.Y(), aRotation * aNormal.X());
  const gp_Vec2d aChord(aPoint, p1);
  F = aChord.Dot(aNormal);
  D = -aTangent.Dot(aNormal) + aChord.Dot(aDerivative);
  return true;
}
