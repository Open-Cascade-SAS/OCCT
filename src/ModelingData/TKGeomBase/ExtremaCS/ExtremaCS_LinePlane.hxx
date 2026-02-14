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

#ifndef _ExtremaCS_LinePlane_HeaderFile
#define _ExtremaCS_LinePlane_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <ExtremaCS_Line.hxx>
#include <ExtremaCS_Plane.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Line-Plane extrema computation.
//!
//! Computes extrema between a 3D line and a plane.
//! Two cases:
//! 1. Line parallel to plane: Infinite solutions at constant distance
//! 2. Line intersects plane: Single extremum at intersection point (distance = 0)
//!
//! For a line L(t) = P + t*D and plane with normal N passing through Q:
//! - Distance from L(t) to plane = |(L(t) - Q) . N|
//! - d(Dist)/dt = D . N = 0 when D perpendicular to N (line parallel to plane)
//! - Otherwise, intersection at t = (Q - P) . N / (D . N)
class ExtremaCS_LinePlane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with line and plane geometry.
  ExtremaCS_LinePlane(const gp_Lin& theLine, const gp_Pln& thePlane)
      : myLine(theLine),
        myPlane(thePlane),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with line and plane geometry and domain.
  ExtremaCS_LinePlane(const gp_Lin&                             theLine,
                      const gp_Pln&                             thePlane,
                      const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myLine(theLine),
        myPlane(thePlane),
        myDomain(theDomain)
  {
  }

  //! @brief Perform extrema computation.
  //! @param theTol tolerance for parallel detection
  //! @param theMode search mode
  //! @return const reference to result
  [[nodiscard]] const ExtremaCS::Result& Perform(
    double                theTol,
    ExtremaCS::SearchMode theMode = ExtremaCS::SearchMode::MinMax) const
  {
    myResult.Clear();

    const gp_Dir& aLineDir  = myLine.Direction();
    const gp_Dir& aPlaneNor = myPlane.Axis().Direction();

    // Check if line is parallel to plane
    const double aDotDN = aLineDir.Dot(aPlaneNor);

    if (std::abs(aDotDN) < ExtremaCS::THE_PARALLEL_TOLERANCE)
    {
      // Line parallel to plane - infinite solutions at constant distance
      const gp_Pnt& aLineP  = myLine.Location();
      const gp_Pnt& aPlaneP = myPlane.Location();

      const double aSignedDist        = gp_Vec(aPlaneP, aLineP).Dot(gp_Vec(aPlaneNor));
      myResult.Status                 = ExtremaCS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aSignedDist * aSignedDist;
      return myResult;
    }

    // Line intersects plane - find intersection point
    const gp_Pnt& aLineP  = myLine.Location();
    const gp_Pnt& aPlaneP = myPlane.Location();

    // t = (PlaneP - LineP) . N / (D . N)
    const double aT = gp_Vec(aLineP, aPlaneP).Dot(gp_Vec(aPlaneNor)) / aDotDN;

    // Check domain constraints
    if (myDomain.has_value())
    {
      if (aT < myDomain->Curve.Min - theTol || aT > myDomain->Curve.Max + theTol)
      {
        myResult.Status = ExtremaCS::Status::NoSolution;
        return myResult;
      }
    }

    // Compute intersection point
    const gp_Pnt aLinePt = myLine.Location().Translated(aT * gp_Vec(aLineDir));

    // Get UV on plane
    double aU, aV;
    ElSLib::Parameters(myPlane, aLinePt, aU, aV);

    // Check surface domain constraints
    if (myDomain.has_value())
    {
      if (aU < myDomain->Surface.UMin - theTol || aU > myDomain->Surface.UMax + theTol
          || aV < myDomain->Surface.VMin - theTol || aV > myDomain->Surface.VMax + theTol)
      {
        myResult.Status = ExtremaCS::Status::NoSolution;
        return myResult;
      }
    }

    // Add extremum (intersection = zero distance = minimum)
    if (theMode == ExtremaCS::SearchMode::Max)
    {
      // For max mode, an intersection doesn't count
      myResult.Status = ExtremaCS::Status::NoSolution;
      return myResult;
    }

    ExtremaCS::ExtremumResult anExt;
    anExt.ParameterOnCurve    = aT;
    anExt.ParameterOnSurfaceU = aU;
    anExt.ParameterOnSurfaceV = aV;
    anExt.PointOnCurve        = aLinePt;
    anExt.PointOnSurface      = aLinePt; // Same point at intersection
    anExt.SquareDistance      = 0.0;
    anExt.IsMinimum           = true;
    myResult.Extrema.Append(anExt);
    myResult.Status = ExtremaCS::Status::OK;

    return myResult;
  }

private:
  gp_Lin                             myLine;   //!< Line geometry
  gp_Pln                             myPlane;  //!< Plane geometry
  std::optional<ExtremaCS::Domain3D> myDomain; //!< Optional domain
  mutable ExtremaCS::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaCS_LinePlane_HeaderFile
