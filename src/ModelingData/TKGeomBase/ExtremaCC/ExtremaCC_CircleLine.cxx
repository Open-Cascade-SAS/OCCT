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

#include <ExtremaCC_CircleLine.hxx>

#include <ElCLib.hxx>
#include <ExtremaCC_Circle.hxx>
#include <ExtremaCC_Line.hxx>
#include <gp.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <IntAna2d_AnaIntersection.hxx>
#include <MathRoot_Trig.hxx>
#include <Precision.hxx>

#include <cmath>

namespace
{
  //! Refines direction to eliminate near-zero components.
  //! @param[in,out] theDir direction to refine
  void refineDir(gp_Dir& theDir)
  {
    constexpr double aTol = 1.0e-16;
    double aX = theDir.X();
    double aY = theDir.Y();
    double aZ = theDir.Z();

    if (std::abs(aX) < aTol) aX = 0.0;
    if (std::abs(aY) < aTol) aY = 0.0;
    if (std::abs(aZ) < aTol) aZ = 0.0;

    // Renormalize if needed
    const double aMag = std::sqrt(aX * aX + aY * aY + aZ * aZ);
    if (aMag > gp::Resolution())
    {
      theDir.SetCoord(aX / aMag, aY / aMag, aZ / aMag);
    }
  }
} // namespace

//==================================================================================================

ExtremaCC_CircleLine::ExtremaCC_CircleLine(const gp_Circ& theCircle, const gp_Lin& theLine)
    : myCircle(theCircle),
      myLine(theLine),
      myDomain(std::nullopt)
{
}

//==================================================================================================

ExtremaCC_CircleLine::ExtremaCC_CircleLine(const gp_Circ&              theCircle,
                                           const gp_Lin&               theLine,
                                           const ExtremaCC::Domain2D& theDomain)
    : myCircle(theCircle),
      myLine(theLine),
      myDomain(theDomain)
{
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleLine::Perform(double                theTol,
                                                       ExtremaCC::SearchMode theMode) const
{
  (void)theMode; // All extrema are computed analytically

  myResult.Clear();

  // Try coplanar case first
  if (performPlanar(theTol))
  {
    return myResult;
  }

  // General case: line not in circle plane
  // Transform to circle's coordinate system and solve trigonometric equation
  //
  // For circle C(u) = O2 + R*cos(u)*X2 + R*sin(u)*Y2
  // and line L(v) = O1 + v*D
  //
  // Extrema condition: (C(u) - L(v)) . C'(u) = 0  and  (C(u) - L(v)) . D = 0
  // Second equation gives: v = (C(u) - O1) . D
  // Substituting into first equation gives trigonometric equation in u

  const gp_Dir& aD  = myLine.Direction();
  const gp_Dir& aX2 = myCircle.XAxis().Direction();
  const gp_Dir& aY2 = myCircle.YAxis().Direction();
  const gp_Dir& aZ2 = myCircle.Axis().Direction();

  // Direction D in circle's frame
  double aDx = aD.Dot(aX2);
  double aDy = aD.Dot(aY2);
  double aDz = aD.Dot(aZ2);

  gp_Dir aDLocal(aDx, aDy, aDz);
  refineDir(aDLocal);
  aDLocal.Coord(aDx, aDy, aDz);

  // Vector O2O1 in circle's frame
  const gp_Pnt& aO1 = myLine.Location();
  const gp_Pnt& aO2 = myCircle.Location();
  gp_Vec        aO2O1(aO2, aO1);

  const double aMagO2O1 = aO2O1.Magnitude();
  if (aMagO2O1 > gp::Resolution())
  {
    gp_Dir aDirO2O1;
    aO2O1.Multiply(1.0 / aMagO2O1);
    aDirO2O1.SetCoord(aO2O1.Dot(aX2), aO2O1.Dot(aY2), aO2O1.Dot(aZ2));
    refineDir(aDirO2O1);
    aO2O1.SetXYZ(aMagO2O1 * aDirO2O1.XYZ());
  }
  else
  {
    aO2O1.SetCoord(aO2O1.Dot(aX2), aO2O1.Dot(aY2), aO2O1.Dot(aZ2));
  }

  // V = (O2O1.D)*D - O2O1
  const gp_XYZ aVxyz = (aDLocal.XYZ() * aO2O1.Dot(aDLocal)) - aO2O1.XYZ();

  // Build coefficients for trigonometric equation:
  // A1*cos^2 + 2*A2*cos*sin + A3*cos + A4*sin + A5 = 0
  constexpr double aCoefTol = 1.0e-12;
  const double     aR       = myCircle.Radius();

  double aA5 = aR * aDx * aDy;
  double aA1 = -2.0 * aA5;
  double aA2 = 0.5 * aR * (aDx * aDx - aDy * aDy);
  double aA3 = aVxyz.Y();
  double aA4 = -aVxyz.X();

  // Clean up near-zero coefficients
  if (std::abs(aA1) <= aCoefTol) aA1 = 0.0;
  if (std::abs(aA2) <= aCoefTol) aA2 = 0.0;
  if (std::abs(aA3) <= aCoefTol) aA3 = 0.0;
  if (std::abs(aA4) <= aCoefTol) aA4 = 0.0;
  if (std::abs(aA5) <= aCoefTol) aA5 = 0.0;

  // Solve using MathRoot::Trigonometric (modern solver)
  // Equation: A1*cos^2 + 2*A2*cos*sin + A3*cos + A4*sin + A5 = 0
  MathRoot::TrigResult aSolverResult = MathRoot::Trigonometric(aA1, aA2, aA3, aA4, aA5, 0.0, 2.0 * M_PI);

  if (!aSolverResult.IsDone())
  {
    if (aSolverResult.InfiniteRoots)
    {
      // All points at same distance - parallel case
      myResult.Status                 = ExtremaCC::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aR * aR;
      return myResult;
    }
    myResult.Status = ExtremaCC::Status::NumericalError;
    return myResult;
  }

  if (aSolverResult.InfiniteRoots)
  {
    // All points at same distance - parallel case
    myResult.Status                 = ExtremaCC::Status::InfiniteSolutions;
    myResult.InfiniteSquareDistance = aR * aR;
    return myResult;
  }

  // Store solutions
  const int aNbSol = aSolverResult.NbRoots;
  for (int i = 0; i < aNbSol; ++i)
  {
    const double aU1 = aSolverResult.Roots[i];
    const gp_Pnt aP1 = ElCLib::Value(aU1, myCircle);
    const double aU2 = gp_Vec(aO1, aP1).Dot(aD);

    addSolution(aU1, aU2, theTol);
  }

  myResult.Status = ExtremaCC::Status::OK;
  return myResult;
}

//==================================================================================================

const ExtremaCC::Result& ExtremaCC_CircleLine::PerformWithEndpoints(
  double                theTol,
  ExtremaCC::SearchMode theMode) const
{
  (void)Perform(theTol, theMode);

  // Add endpoint extrema if domain is bounded
  if (myResult.Status == ExtremaCC::Status::OK && myDomain.has_value())
  {
    ExtremaCC_Circle aEval1(myCircle, myDomain->Curve1);
    ExtremaCC_Line   aEval2(myLine, myDomain->Curve2);
    ExtremaCC::AddEndpointExtrema(myResult, *myDomain, aEval1, aEval2, theTol, theMode);
  }

  return myResult;
}

//==================================================================================================

bool ExtremaCC_CircleLine::performPlanar(double theTol) const
{
  const gp_Dir& aDirC = myCircle.Axis().Direction();
  const gp_Dir& aDirL = myLine.Direction();

  // Check if line is perpendicular to circle axis (in circle plane or parallel to it)
  if (std::abs(aDirC.Dot(aDirL)) > Precision::Angular())
  {
    return false; // Not coplanar - use general algorithm
  }

  // Line is in the circle plane or parallel to it
  // Project to 2D and solve

  const gp_XYZ& aCLoc = myCircle.Location().XYZ();
  const gp_XYZ& aDCx  = myCircle.Position().XDirection().XYZ();
  const gp_XYZ& aDCy  = myCircle.Position().YDirection().XYZ();

  const gp_XYZ& aLLoc = myLine.Location().XYZ();
  const gp_XYZ& aLDir = myLine.Direction().XYZ();

  const gp_XYZ aVecCL(aLLoc - aCLoc);

  // Create 2D circle at origin
  const gp_Pnt2d aPC(0.0, 0.0);
  gp_Ax22d       aCircAxis(aPC, gp_Dir2d(1.0, 0.0), gp_Dir2d(0.0, 1.0));
  gp_Circ2d      aCirc2d(aCircAxis, myCircle.Radius());

  // Create 2D line
  gp_Pnt2d aPL(aVecCL.Dot(aDCx), aVecCL.Dot(aDCy));
  gp_Dir2d aDL(aLDir.Dot(aDCx), aLDir.Dot(aDCy));
  gp_Lin2d aLin2d(aPL, aDL);

  // Find intersections (they are also extrema with zero distance)
  IntAna2d_AnaIntersection anInters(aLin2d, aCirc2d);

  if (anInters.IsDone())
  {
    const int aNbSol = anInters.NbPoints();
    for (int i = 1; i <= aNbSol; ++i)
    {
      const double aLinPar  = anInters.Point(i).ParamOnFirst();
      const double aCircPar = anInters.Point(i).ParamOnSecond();

      addSolution(aCircPar, aLinPar, theTol);
    }
  }

  // For coplanar case, also find the non-intersection extrema (min/max distance)
  // These occur when the line of centers is perpendicular to the line direction

  // Vector from circle center to line (perpendicular foot)
  const gp_Vec aO1O2(myLine.Location(), myCircle.Location());
  const double aProj      = aO1O2.Dot(myLine.Direction());
  const gp_Pnt aFootOnLin = ElCLib::Value(aProj, myLine);
  const gp_Vec aPerp(aFootOnLin, myCircle.Location());
  const double aPerpDist = aPerp.Magnitude();

  if (aPerpDist > gp::Resolution())
  {
    // Find the two points on circle closest/farthest from line
    const gp_Dir aPerpDir(aPerp);

    // Closest point: in direction of perpendicular
    const gp_Pnt aPClose =
      myCircle.Location().XYZ() - myCircle.Radius() * aPerpDir.XYZ();
    const double aU1Close = ElCLib::Parameter(myCircle, aPClose);
    const double aU2Close = ElCLib::Parameter(myLine, aPClose);
    addSolution(aU1Close, aU2Close, theTol);

    // Farthest point: opposite direction
    const gp_Pnt aPFar =
      myCircle.Location().XYZ() + myCircle.Radius() * aPerpDir.XYZ();
    const double aU1Far = ElCLib::Parameter(myCircle, aPFar);
    const double aU2Far = ElCLib::Parameter(myLine, aPFar);
    addSolution(aU1Far, aU2Far, theTol);
  }
  else
  {
    // Line passes through circle center in the plane
    // Add 4 extrema: 2 minima (intersections along line direction)
    // and 2 maxima (perpendicular to line direction in plane)
    const gp_Dir& aDirL = myLine.Direction();
    const gp_Dir& aCNorm = myCircle.Axis().Direction();

    // Points along line direction (intersections - minima)
    gp_Pnt aP1Plus = myCircle.Location().XYZ() + myCircle.Radius() * aDirL.XYZ();
    gp_Pnt aP1Minus = myCircle.Location().XYZ() - myCircle.Radius() * aDirL.XYZ();
    double aU1Plus = ElCLib::Parameter(myCircle, aP1Plus);
    double aU1Minus = ElCLib::Parameter(myCircle, aP1Minus);
    double aU2Plus = ElCLib::Parameter(myLine, aP1Plus);
    double aU2Minus = ElCLib::Parameter(myLine, aP1Minus);
    addSolution(aU1Plus, aU2Plus, theTol);
    addSolution(aU1Minus, aU2Minus, theTol);

    // Points perpendicular to line direction (maxima)
    gp_Dir aPerpInPlane = aCNorm.Crossed(aDirL);
    gp_Pnt aP2Plus = myCircle.Location().XYZ() + myCircle.Radius() * aPerpInPlane.XYZ();
    gp_Pnt aP2Minus = myCircle.Location().XYZ() - myCircle.Radius() * aPerpInPlane.XYZ();
    double aU1PerpPlus = ElCLib::Parameter(myCircle, aP2Plus);
    double aU1PerpMinus = ElCLib::Parameter(myCircle, aP2Minus);
    double aU2PerpPlus = ElCLib::Parameter(myLine, aP2Plus);
    double aU2PerpMinus = ElCLib::Parameter(myLine, aP2Minus);
    addSolution(aU1PerpPlus, aU2PerpPlus, theTol);
    addSolution(aU1PerpMinus, aU2PerpMinus, theTol);
  }

  if (myResult.Status != ExtremaCC::Status::InfiniteSolutions)
  {
    myResult.Status = ExtremaCC::Status::OK;
  }

  return true;
}

//==================================================================================================

void ExtremaCC_CircleLine::addSolution(double theU1, double theU2, double theTol) const
{
  // Check bounds if domain is specified
  if (myDomain.has_value())
  {
    const bool aOutside1 = (theU1 < myDomain->Curve1.Min - theTol) ||
                           (theU1 > myDomain->Curve1.Max + theTol);
    const bool aOutside2 = (theU2 < myDomain->Curve2.Min - theTol) ||
                           (theU2 > myDomain->Curve2.Max + theTol);

    if (aOutside1 || aOutside2)
    {
      return; // Outside domain
    }

    // Clamp to bounds
    theU1 = std::max(myDomain->Curve1.Min, std::min(myDomain->Curve1.Max, theU1));
    theU2 = std::max(myDomain->Curve2.Min, std::min(myDomain->Curve2.Max, theU2));
  }

  // Compute points
  const gp_Pnt aP1 = ElCLib::Value(theU1, myCircle);
  const gp_Pnt aP2 = ElCLib::Value(theU2, myLine);

  // Check for duplicate solutions
  constexpr double aDupTol = 1.0e-10;
  for (int i = 0; i < myResult.Extrema.Length(); ++i)
  {
    const auto& anExt = myResult.Extrema(i);
    if (std::abs(anExt.Parameter1 - theU1) < aDupTol &&
        std::abs(anExt.Parameter2 - theU2) < aDupTol)
    {
      return; // Duplicate
    }
  }

  // Store result
  ExtremaCC::ExtremumResult anExt;
  anExt.Parameter1     = theU1;
  anExt.Parameter2     = theU2;
  anExt.Point1         = aP1;
  anExt.Point2         = aP2;
  anExt.SquareDistance = aP1.SquareDistance(aP2);
  anExt.IsMinimum      = true; // Will be classified later if needed

  myResult.Extrema.Append(anExt);
}
