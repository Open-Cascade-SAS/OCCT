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

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <math.h>

IMPLEMENT_STANDARD_RTTIEXT(HelixGeom_HelixCurve, Adaptor3d_Curve)

//=================================================================================================

HelixGeom_HelixCurve::HelixGeom_HelixCurve()
{
  myFirst       = 0.;
  myLast        = 2. * M_PI;
  myPitch       = 1.;
  myRStart      = 1.;
  myTaperAngle  = 0.;
  myIsClockWise = Standard_True;
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

void HelixGeom_HelixCurve::Load(const Standard_Real    aT1,
                                const Standard_Real    aT2,
                                const Standard_Real    aPitch,
                                const Standard_Real    aRStart,
                                const Standard_Real    aTaperAngle,
                                const Standard_Boolean aIsCW)
{
  char          buf[] = {"HelixGeom_HelixCurve::Load"};
  Standard_Real aTwoPI, aHalfPI;
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

Standard_Real HelixGeom_HelixCurve::FirstParameter() const
{
  return myFirst;
}

//=================================================================================================

Standard_Real HelixGeom_HelixCurve::LastParameter() const
{
  return myLast;
}

//=================================================================================================

GeomAbs_Shape HelixGeom_HelixCurve::Continuity() const
{
  return GeomAbs_CN;
}

//=================================================================================================

Standard_Integer HelixGeom_HelixCurve::NbIntervals(const GeomAbs_Shape) const
{
  return 1;
}

//=================================================================================================

void HelixGeom_HelixCurve::Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape) const
{
  T(1) = myFirst;
  T(2) = myLast;
}

//=================================================================================================

Standard_Real HelixGeom_HelixCurve::Resolution(const Standard_Real) const
{
  throw Standard_NotImplemented("HelixGeom_HelixCurve::Resolution");
}

//=================================================================================================

Standard_Boolean HelixGeom_HelixCurve::IsClosed() const
{
  throw Standard_NotImplemented("HelixGeom_HelixCurve::IsClosed");
}

//=================================================================================================

Standard_Boolean HelixGeom_HelixCurve::IsPeriodic() const
{
  throw Standard_NotImplemented("HelixGeom_HelixCurve::IsPeriodic");
}

//=================================================================================================

Standard_Real HelixGeom_HelixCurve::Period() const
{
  throw Standard_DomainError("HelixGeom_HelixCurve::Period");
}

//=================================================================================================

gp_Pnt HelixGeom_HelixCurve::Value(const Standard_Real aT) const
{
  Standard_Real aST, aCT, aX, aY, aZ, a1;
  // Calculate trigonometric values and radius
  aCT = cos(aT);
  aST = sin(aT);
  a1  = myRStart + myC1 * myTgBeta * aT;
  // Calculate Cartesian coordinates
  aX = a1 * aCT;
  aY = a1 * aST;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1 * aT;
  return gp_Pnt(aX, aY, aZ);
}

//=================================================================================================

void HelixGeom_HelixCurve::D0(const Standard_Real aT, gp_Pnt& aP) const
{
  aP = Value(aT);
}

//=================================================================================================

void HelixGeom_HelixCurve::D1(const Standard_Real aT, gp_Pnt& aP, gp_Vec& aV1) const
{
  Standard_Real aST, aCT, aX, aY, aZ, a1, a2;
  // Calculate point and first derivative
  aCT = cos(aT);
  aST = sin(aT);
  // Calculate radius at parameter t
  a1 = myRStart + myC1 * myTgBeta * aT;
  // Calculate point coordinates
  aX = a1 * aCT;
  aY = a1 * aST;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1 * aT;
  aP.SetCoord(aX, aY, aZ);
  // Calculate first derivative coefficients
  a1 = myC1 * myTgBeta;
  a2 = myRStart + a1 * aT;
  // Calculate first derivative components
  aX = a1 * aCT - a2 * aST;
  aY = a1 * aST + a2 * aCT;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1;
  aV1.SetCoord(aX, aY, aZ);
}

//=================================================================================================

void HelixGeom_HelixCurve::D2(const Standard_Real aT, gp_Pnt& aP, gp_Vec& aV1, gp_Vec& aV2) const
{
  Standard_Real aST, aCT, aX, aY, aZ, a1, a2;
  // Calculate point, first and second derivatives
  aCT = cos(aT);
  aST = sin(aT);
  // Calculate radius at parameter t
  a1 = myRStart + myC1 * myTgBeta * aT;
  // Calculate point coordinates
  aX = a1 * aCT;
  aY = a1 * aST;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1 * aT;
  aP.SetCoord(aX, aY, aZ);
  // Calculate first derivative coefficients
  a1 = myC1 * myTgBeta;
  a2 = myRStart + a1 * aT;
  // Calculate first derivative components
  aX = a1 * aCT - a2 * aST;
  aY = a1 * aST + a2 * aCT;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = myC1;
  aV1.SetCoord(aX, aY, aZ);
  // Calculate second derivative
  a1 = 2. * a1;
  aX = -a2 * aCT - a1 * aST;
  aY = -a2 * aST - a1 * aCT;
  if (!myIsClockWise)
  {
    aY = -aY;
  }
  aZ = 0.;
  aV2.SetCoord(aX, aY, aZ);
}

//=================================================================================================

gp_Vec HelixGeom_HelixCurve::DN(const Standard_Real aT, const Standard_Integer aN) const
{
  gp_Pnt aP;
  gp_Vec aV1, aV2;
  // Compute derivative based on order
  switch (aN)
  {
    case 1:
      D1(aT, aP, aV1);
      break;
    case 2:
      D2(aT, aP, aV1, aV2);
      break;
    default:
      throw Standard_NotImplemented("HelixGeom_HelixCurve::DN");
      break;
  }
  return (aN == 1) ? aV1 : aV2;
}
