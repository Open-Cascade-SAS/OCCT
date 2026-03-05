// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <HelixGeom_HelixCurve.hxx>

#include <Geom_Curve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <NCollection_Array1.hxx>

#include <cmath>

IMPLEMENT_STANDARD_RTTIEXT(HelixGeom_HelixCurve, Adaptor3d_Curve)

//=================================================================================================

HelixGeom_HelixCurve::HelixGeom_HelixCurve()
{
  myFirst       = 0.;
  myLast        = 2. * M_PI;
  myPitch       = 1.;
  myRStart      = 1.;
  myTaperAngle  = 0.;
  myIsClockWise = true;
  // Calculate derived parameters
  myC1       = myPitch / myLast;
  myTgBeta   = 0.;
  myTolAngle = 1.e-4;
}

//=================================================================================================

void HelixGeom_HelixCurve::Load()
{
  Load(myFirst, myLast, myPitch, myRStart, myTaperAngle, myIsClockWise);
}

//=================================================================================================

void HelixGeom_HelixCurve::Load(const double aT1,
                                const double aT2,
                                const double aPitch,
                                const double aRStart,
                                const double aTaperAngle,
                                const bool   aIsCW)
{
  char   buf[] = {"HelixGeom_HelixCurve::Load"};
  double aTwoPI, aHalfPI;
  // Define angular constants
  aTwoPI  = 2. * M_PI;
  aHalfPI = 0.5 * M_PI;
  // Store parameter values
  myFirst       = aT1;
  myLast        = aT2;
  myPitch       = aPitch;
  myRStart      = aRStart;
  myTaperAngle  = aTaperAngle;
  myIsClockWise = aIsCW;
  // Validate input parameters
  if (aT1 >= aT2)
  {
    throw Standard_ConstructionError(buf);
  }
  if (myPitch < 0.)
  {
    throw Standard_ConstructionError(buf);
  }
  if (myRStart < 0.)
  {
    throw Standard_ConstructionError(buf);
  }
  if (myTaperAngle <= -aHalfPI || myTaperAngle >= aHalfPI)
  {
    throw Standard_ConstructionError(buf);
  }
  // Calculate helix coefficient
  myC1 = myPitch / aTwoPI;
  if (fabs(myTaperAngle) > myTolAngle)
  {
    myTgBeta = tan(myTaperAngle);
  }
}

//=================================================================================================

double HelixGeom_HelixCurve::FirstParameter() const
{
  return myFirst;
}

//=================================================================================================

double HelixGeom_HelixCurve::LastParameter() const
{
  return myLast;
}

//=================================================================================================

GeomAbs_Shape HelixGeom_HelixCurve::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

int HelixGeom_HelixCurve::NbIntervals(const GeomAbs_Shape) const
{
  return 1;
}

//=================================================================================================

void HelixGeom_HelixCurve::Intervals(NCollection_Array1<double>& T, const GeomAbs_Shape) const
{
  T(1) = myFirst;
  T(2) = myLast;
}

//=================================================================================================

double HelixGeom_HelixCurve::Resolution(const double) const
{
  throw Standard_NotImplemented("HelixGeom_HelixCurve::Resolution");
}

//=================================================================================================

bool HelixGeom_HelixCurve::IsClosed() const
{
  throw Standard_NotImplemented("HelixGeom_HelixCurve::IsClosed");
}

//=================================================================================================

bool HelixGeom_HelixCurve::IsPeriodic() const
{
  throw Standard_NotImplemented("HelixGeom_HelixCurve::IsPeriodic");
}

//=================================================================================================

double HelixGeom_HelixCurve::Period() const
{
  throw Standard_DomainError("HelixGeom_HelixCurve::Period");
}

//=================================================================================================

gp_Pnt HelixGeom_HelixCurve::EvalD0(double theT) const
{
  double aST, aCT, aX, aY, aZ, a1;
  // Calculate trigonometric values and radius
  aCT = cos(theT);
  aST = sin(theT);
  a1  = myRStart + myC1 * myTgBeta * theT;
  // Calculate Cartesian coordinates
  aX = a1 * aCT;
  aY = a1 * aST;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1 * theT;
  return gp_Pnt(aX, aY, aZ);
}

//=================================================================================================

Geom_Curve::ResD1 HelixGeom_HelixCurve::EvalD1(double theT) const
{
  double aST, aCT, aX, aY, aZ, a1, a2;
  // Calculate point and first derivative
  aCT = cos(theT);
  aST = sin(theT);
  // Calculate radius at parameter t
  a1 = myRStart + myC1 * myTgBeta * theT;
  // Calculate point coordinates
  aX = a1 * aCT;
  aY = a1 * aST;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1 * theT;
  gp_Pnt aP(aX, aY, aZ);
  // Calculate first derivative coefficients
  a1 = myC1 * myTgBeta;
  a2 = myRStart + a1 * theT;
  // Calculate first derivative components
  aX = a1 * aCT - a2 * aST;
  aY = a1 * aST + a2 * aCT;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1;
  gp_Vec aV1(aX, aY, aZ);
  return {aP, aV1};
}

//=================================================================================================

Geom_Curve::ResD2 HelixGeom_HelixCurve::EvalD2(double theT) const
{
  double aST, aCT, aX, aY, aZ, a1, a2;
  // Calculate point, first and second derivatives
  aCT = cos(theT);
  aST = sin(theT);
  // Calculate radius at parameter t
  a1 = myRStart + myC1 * myTgBeta * theT;
  // Calculate point coordinates
  aX = a1 * aCT;
  aY = a1 * aST;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1 * theT;
  gp_Pnt aP(aX, aY, aZ);
  // Calculate first derivative coefficients
  a1 = myC1 * myTgBeta;
  a2 = myRStart + a1 * theT;
  // Calculate first derivative components
  aX = a1 * aCT - a2 * aST;
  aY = a1 * aST + a2 * aCT;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1;
  gp_Vec aV1(aX, aY, aZ);
  // Calculate second derivative
  a1 = 2. * a1;
  aX = -a2 * aCT - a1 * aST;
  aY = -a2 * aST - a1 * aCT;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = 0.;
  gp_Vec aV2(aX, aY, aZ);
  return {aP, aV1, aV2};
}

//=================================================================================================

gp_Vec HelixGeom_HelixCurve::EvalDN(double theT, int theN) const
{
  // Compute derivative based on order
  switch (theN)
  {
    case 1:
    {
      const Geom_Curve::ResD1 aRes = EvalD1(theT);
      return aRes.D1;
    }
    case 2:
    {
      const Geom_Curve::ResD2 aRes = EvalD2(theT);
      return aRes.D2;
    }
    default:
      throw Standard_NotImplemented("HelixGeom_HelixCurve::EvalDN");
      break;
  }
  return gp_Vec();
}
