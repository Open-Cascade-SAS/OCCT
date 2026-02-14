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

#ifndef _ExtremaCS_LineCylinder_HeaderFile
#define _ExtremaCS_LineCylinder_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Line-Cylinder extrema computation.
//!
//! Computes extrema between a 3D line and a cylinder.
//!
//! For a line L(t) = P + t*D and cylinder with axis A through Q with radius R:
//! The problem decomposes into:
//! 1. Distance from line to cylinder axis (line-line distance)
//! 2. Radial offset by R
//!
//! Special cases:
//! - Line parallel to axis: Constant distance to cylinder surface
//! - Line perpendicular to axis: Simple 2D problem
//! - General case: Minimum at point of closest approach to axis
class ExtremaCS_LineCylinder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with line and cylinder geometry.
  ExtremaCS_LineCylinder(const gp_Lin& theLine, const gp_Cylinder& theCylinder)
      : myLine(theLine),
        myCylinder(theCylinder),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with line and cylinder geometry and domain.
  ExtremaCS_LineCylinder(const gp_Lin&                             theLine,
                         const gp_Cylinder&                        theCylinder,
                         const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myLine(theLine),
        myCylinder(theCylinder),
        myDomain(theDomain)
  {
  }

  //! @brief Perform extrema computation.
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result
  [[nodiscard]] const ExtremaCS::Result& Perform(
    double                theTol,
    ExtremaCS::SearchMode theMode = ExtremaCS::SearchMode::MinMax) const
  {
    myResult.Clear();

    const gp_Pnt& aLineP   = myLine.Location();
    const gp_Dir& aLineD   = myLine.Direction();
    const gp_Ax3& aCylAxis = myCylinder.Position();
    const gp_Pnt& aCylP    = aCylAxis.Location();
    const gp_Dir& aCylD    = aCylAxis.Direction();
    const double  aRadius  = myCylinder.Radius();

    // Compute line-axis relationship
    const double aDotDD = aLineD.Dot(aCylD);
    const gp_Vec aPC(aLineP, aCylP);

    // Check if line is parallel to cylinder axis
    if (std::abs(std::abs(aDotDD) - 1.0) < ExtremaCS::THE_PARALLEL_TOLERANCE)
    {
      // Line parallel to axis - infinite solutions at constant distance
      // Project line onto plane perpendicular to axis
      const double aDistToAxis    = aPC.Crossed(gp_Vec(aCylD)).Magnitude();
      const double aDistToSurface = std::abs(aDistToAxis - aRadius);

      myResult.Status                 = ExtremaCS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aDistToSurface * aDistToSurface;
      return myResult;
    }

    // General case: Find closest point between skew lines
    // Line: L(t) = P + t*D
    // Axis: A(s) = Q + s*E
    // Closest approach: minimize |L(t) - A(s)|^2

    // From the system of equations:
    // t - s*(D.E) = (Q-P).D
    // s - t*(D.E) = (P-Q).E
    const double aA      = 1.0;
    const double aB      = -aDotDD;
    const double aC      = -aDotDD;
    const double aD_coef = 1.0;
    const double aE1     = aPC.Dot(gp_Vec(aLineD));
    const double aE2     = -aPC.Dot(gp_Vec(aCylD));

    const double aDet = aA * aD_coef - aB * aC;
    if (std::abs(aDet) < 1.0e-15)
    {
      // Lines are parallel (already handled above, but safety check)
      myResult.Status = ExtremaCS::Status::NumericalError;
      return myResult;
    }

    const double aT = (aE1 * aD_coef - aB * aE2) / aDet;
    const double aS = (aA * aE2 - aE1 * aC) / aDet;

    // Check curve domain
    if (myDomain.has_value())
    {
      if (aT < myDomain->Curve.Min - theTol || aT > myDomain->Curve.Max + theTol)
      {
        myResult.Status = ExtremaCS::Status::NoSolution;
        return myResult;
      }
    }

    // Point on line closest to axis
    const gp_Pnt aLinePt = aLineP.Translated(aT * gp_Vec(aLineD));

    // Point on axis
    const gp_Pnt aAxisPt = aCylP.Translated(aS * gp_Vec(aCylD));

    // Direction from axis to line point (radial direction)
    gp_Vec       aRadialDir(aAxisPt, aLinePt);
    const double aDistToAxis = aRadialDir.Magnitude();

    if (aDistToAxis < Precision::Confusion())
    {
      // Line intersects axis - pick perpendicular direction
      aRadialDir = gp_Vec(aCylD).Crossed(gp_Vec(aLineD));
      if (aRadialDir.Magnitude() < Precision::Confusion())
      {
        aRadialDir = gp_Vec(aCylAxis.XDirection());
      }
    }
    aRadialDir.Normalize();

    // Helper to add extremum
    auto addExtremum = [this, &aCylP, &aCylD, &aS, &theTol](const gp_Pnt& theLinePt,
                                                            double        theT,
                                                            const gp_Vec& theRadialDir,
                                                            bool          theIsMin) {
      // Point on cylinder surface
      const gp_Pnt aCylPt =
        aCylP.Translated(aS * gp_Vec(aCylD) + myCylinder.Radius() * theRadialDir);

      // Get UV on cylinder
      double aU, aV;
      ElSLib::Parameters(myCylinder, aCylPt, aU, aV);

      // Check surface domain
      if (myDomain.has_value())
      {
        if (aV < myDomain->Surface.VMin - theTol || aV > myDomain->Surface.VMax + theTol)
        {
          return;
        }
        // U is periodic, handle wrapping
        if (aU < myDomain->Surface.UMin - theTol)
          aU += ExtremaCS::THE_TWO_PI;
        if (aU > myDomain->Surface.UMax + theTol)
          aU -= ExtremaCS::THE_TWO_PI;
        if (aU < myDomain->Surface.UMin - theTol || aU > myDomain->Surface.UMax + theTol)
        {
          return;
        }
      }

      ExtremaCS::ExtremumResult anExt;
      anExt.ParameterOnCurve    = theT;
      anExt.ParameterOnSurfaceU = aU;
      anExt.ParameterOnSurfaceV = aV;
      anExt.PointOnCurve        = theLinePt;
      anExt.PointOnSurface      = aCylPt;
      anExt.SquareDistance      = theLinePt.SquareDistance(aCylPt);
      anExt.IsMinimum           = theIsMin;
      myResult.Extrema.Append(anExt);
    };

    // Minimum: closest point on cylinder to line
    if (theMode != ExtremaCS::SearchMode::Max)
    {
      if (aDistToAxis >= aRadius)
      {
        // Line is outside cylinder - minimum on same side
        addExtremum(aLinePt, aT, aRadialDir, true);
      }
      else
      {
        // Line inside cylinder - minimum is on opposite side (toward line)
        addExtremum(aLinePt, aT, aRadialDir, true);
      }
    }

    // Maximum: farthest point on cylinder from line
    if (theMode != ExtremaCS::SearchMode::Min)
    {
      // Maximum is on the opposite side of cylinder
      addExtremum(aLinePt, aT, -aRadialDir, false);
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaCS::Status::NoSolution : ExtremaCS::Status::OK;
    return myResult;
  }

private:
  gp_Lin                             myLine;     //!< Line geometry
  gp_Cylinder                        myCylinder; //!< Cylinder geometry
  std::optional<ExtremaCS::Domain3D> myDomain;   //!< Optional domain
  mutable ExtremaCS::Result          myResult;   //!< Reusable result storage
};

#endif // _ExtremaCS_LineCylinder_HeaderFile
