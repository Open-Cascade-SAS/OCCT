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

#ifndef _ExtremaCS_LineSphere_HeaderFile
#define _ExtremaCS_LineSphere_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Line-Sphere extrema computation.
//!
//! Computes extrema between a 3D line and a sphere.
//!
//! For line L(t) = P + t*D and sphere center C with radius R:
//! The minimum distance is at t where (L(t) - C) perpendicular to D.
//! This gives t = (C - P) . D (since D is unit vector).
//!
//! The extremum point on sphere is along the vector from center to line point.
//! - If |L(t_min) - C| < R: line passes through sphere, two intersections
//! - If |L(t_min) - C| = R: line tangent to sphere, one point
//! - If |L(t_min) - C| > R: line misses sphere, minimum at closest approach
//!
//! Maximum distance is infinite for unbounded lines.
class ExtremaCS_LineSphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with line and sphere geometry.
  ExtremaCS_LineSphere(const gp_Lin& theLine, const gp_Sphere& theSphere)
      : myLine(theLine),
        mySphere(theSphere),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with line and sphere geometry and domain.
  ExtremaCS_LineSphere(const gp_Lin&                        theLine,
                       const gp_Sphere&                     theSphere,
                       const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myLine(theLine),
        mySphere(theSphere),
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

    const gp_Pnt& aLineP = myLine.Location();
    const gp_Dir& aLineD = myLine.Direction();
    const gp_Pnt& aCenter = mySphere.Location();
    const double aRadius = mySphere.Radius();

    // Find t where line is closest to sphere center
    // t = (C - P) . D
    const gp_Vec aPC(aLineP, aCenter);
    const double aT = aPC.Dot(gp_Vec(aLineD));

    // Point on line closest to center
    const gp_Pnt aLinePt = aLineP.Translated(aT * gp_Vec(aLineD));
    const double aDistToCenter = aLinePt.Distance(aCenter);

    // Helper to add an extremum
    auto addExtremum = [this, &theTol, &aLineD, &aLineP](double theT, bool theIsMin) {
      // Check curve domain
      if (myDomain.has_value())
      {
        if (theT < myDomain->Curve.Min - theTol || theT > myDomain->Curve.Max + theTol)
        {
          return;
        }
      }

      // Compute points
      const gp_Pnt aCurvePt = aLineP.Translated(theT * gp_Vec(aLineD));
      gp_Vec aDir(mySphere.Location(), aCurvePt);
      const double aDist = aDir.Magnitude();

      gp_Pnt aSphPt;
      double aU, aV;

      if (aDist > Precision::Confusion())
      {
        aDir.Normalize();
        aSphPt = mySphere.Location().Translated(mySphere.Radius() * aDir);
      }
      else
      {
        // Line passes through center
        aSphPt = ElSLib::Value(0.0, 0.0, mySphere);
      }

      ElSLib::Parameters(mySphere, aSphPt, aU, aV);

      // Check surface domain
      if (myDomain.has_value())
      {
        if (aU < myDomain->Surface.UMin - theTol || aU > myDomain->Surface.UMax + theTol
            || aV < myDomain->Surface.VMin - theTol || aV > myDomain->Surface.VMax + theTol)
        {
          return;
        }
      }

      ExtremaCS::ExtremumResult anExt;
      anExt.ParameterOnCurve    = theT;
      anExt.ParameterOnSurfaceU = aU;
      anExt.ParameterOnSurfaceV = aV;
      anExt.PointOnCurve        = aCurvePt;
      anExt.PointOnSurface      = aSphPt;
      anExt.SquareDistance      = aCurvePt.SquareDistance(aSphPt);
      anExt.IsMinimum           = theIsMin;
      myResult.Extrema.Append(anExt);
    };

    // Handle minimum extrema
    if (theMode != ExtremaCS::SearchMode::Max)
    {
      if (aDistToCenter <= aRadius + theTol)
      {
        // Line passes through or is tangent to sphere - find intersection points
        // Solve: |P + t*D - C|^2 = R^2
        // Let W = P - C
        // |W + t*D|^2 = R^2
        // t^2 + 2*(W.D)*t + (|W|^2 - R^2) = 0
        const gp_Vec aW(aCenter, aLineP);
        const double aB = aW.Dot(gp_Vec(aLineD));           // coefficient / 2
        const double aC = aW.SquareMagnitude() - aRadius * aRadius;
        const double aDiscr = aB * aB - aC;

        if (aDiscr >= 0.0)
        {
          const double aSqrtDiscr = std::sqrt(aDiscr);
          const double aT1 = -aB - aSqrtDiscr;
          const double aT2 = -aB + aSqrtDiscr;

          // Add both intersection points as minima (distance = 0)
          addExtremum(aT1, true);
          if (std::abs(aT2 - aT1) > theTol)
          {
            addExtremum(aT2, true);
          }
        }
      }
      else
      {
        // Line misses sphere - closest approach is a minimum
        addExtremum(aT, true);
      }
    }

    // For sphere, there's also a maximum on the opposite side
    // This is only valid for bounded domains
    if (theMode != ExtremaCS::SearchMode::Min)
    {
      // Maximum is at the point where line is farthest from sphere surface
      // For unbounded line, this is infinite - skip
      // For bounded line, check domain endpoints
      if (myDomain.has_value() && myDomain->Curve.IsFinite())
      {
        // Check endpoints of curve domain
        double aTMin = myDomain->Curve.Min;
        double aTMax = myDomain->Curve.Max;

        // Distance function is convex for line-sphere
        // So maximum is at one of the endpoints
        gp_Pnt aPtMin = aLineP.Translated(aTMin * gp_Vec(aLineD));
        gp_Pnt aPtMax = aLineP.Translated(aTMax * gp_Vec(aLineD));

        gp_Vec aDirMin(mySphere.Location(), aPtMin);
        gp_Vec aDirMax(mySphere.Location(), aPtMax);
        const double aDistMin = aDirMin.Magnitude() - aRadius;
        const double aDistMax = aDirMax.Magnitude() - aRadius;

        if (std::abs(aDistMin) >= std::abs(aDistMax))
        {
          addExtremum(aTMin, false);
        }
        else
        {
          addExtremum(aTMax, false);
        }
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaCS::Status::NoSolution
                                                  : ExtremaCS::Status::OK;
    return myResult;
  }

private:
  gp_Lin                             myLine;   //!< Line geometry
  gp_Sphere                          mySphere; //!< Sphere geometry
  std::optional<ExtremaCS::Domain3D> myDomain; //!< Optional domain
  mutable ExtremaCS::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaCS_LineSphere_HeaderFile
