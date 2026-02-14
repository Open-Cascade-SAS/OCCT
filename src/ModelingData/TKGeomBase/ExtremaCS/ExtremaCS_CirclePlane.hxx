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

#ifndef _ExtremaCS_CirclePlane_HeaderFile
#define _ExtremaCS_CirclePlane_HeaderFile

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Circ.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Circle-Plane extrema computation.
//!
//! Computes extrema between a 3D circle and a plane.
//!
//! For circle C(t) = Center + R*cos(t)*X + R*sin(t)*Y
//! and plane with normal N:
//!
//! Distance from C(t) to plane = |(C(t) - P0) . N|
//! where P0 is any point on the plane.
//!
//! d(Dist)/dt = R*(-sin(t)*X + cos(t)*Y) . N = 0
//! => tan(t) = (Y.N) / (X.N)
//!
//! Two extrema exist at t and t + PI (minimum and maximum distance).
//! Special case: Circle parallel to plane (N perpendicular to circle plane)
//! gives infinite solutions at constant distance.
class ExtremaCS_CirclePlane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle and plane geometry.
  ExtremaCS_CirclePlane(const gp_Circ& theCircle, const gp_Pln& thePlane)
      : myCircle(theCircle),
        myPlane(thePlane),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with circle and plane geometry and domain.
  ExtremaCS_CirclePlane(const gp_Circ&                       theCircle,
                        const gp_Pln&                        thePlane,
                        const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myCircle(theCircle),
        myPlane(thePlane),
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

    const gp_Pnt& aCenter = myCircle.Location();
    const gp_Dir& aCircleX = myCircle.XAxis().Direction();
    const gp_Dir& aCircleY = myCircle.YAxis().Direction();

    const gp_Dir& aPlaneN = myPlane.Axis().Direction();
    const gp_Pnt& aPlaneP = myPlane.Location();

    // Check if circle is parallel to plane
    const double aXdotN = gp_Vec(aCircleX).Dot(gp_Vec(aPlaneN));
    const double aYdotN = gp_Vec(aCircleY).Dot(gp_Vec(aPlaneN));

    if (std::abs(aXdotN) < ExtremaCS::THE_PARALLEL_TOLERANCE
        && std::abs(aYdotN) < ExtremaCS::THE_PARALLEL_TOLERANCE)
    {
      // Circle is parallel to plane - infinite solutions
      const double aDist = gp_Vec(aPlaneP, aCenter).Dot(gp_Vec(aPlaneN));
      myResult.Status = ExtremaCS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aDist * aDist;
      return myResult;
    }

    const double aRadius = myCircle.Radius();

    // Helper to add extremum
    auto addExtremum = [this, &theTol, &aPlaneN](double theT, bool theIsMin) {
      // Normalize to [0, 2*PI)
      double aT = ExtremaCS::NormalizeAngle(theT);

      // Check curve domain
      if (myDomain.has_value())
      {
        if (aT < myDomain->Curve.Min - theTol || aT > myDomain->Curve.Max + theTol)
        {
          // Also check aT + 2*PI and aT - 2*PI
          double aTplus = aT + ExtremaCS::THE_TWO_PI;
          double aTminus = aT - ExtremaCS::THE_TWO_PI;
          if (aTplus >= myDomain->Curve.Min - theTol && aTplus <= myDomain->Curve.Max + theTol)
          {
            aT = aTplus;
          }
          else if (aTminus >= myDomain->Curve.Min - theTol && aTminus <= myDomain->Curve.Max + theTol)
          {
            aT = aTminus;
          }
          else
          {
            return;
          }
        }
      }

      // Compute point on circle
      const gp_Pnt aCircPt = ElCLib::Value(aT, myCircle);

      // Compute projection on plane
      const double aSignedDist = gp_Vec(myPlane.Location(), aCircPt).Dot(gp_Vec(aPlaneN));
      const gp_Pnt aPlanePt = aCircPt.Translated(-aSignedDist * gp_Vec(aPlaneN));

      // Get UV on plane
      double aU, aV;
      ElSLib::Parameters(myPlane, aPlanePt, aU, aV);

      // Check surface domain
      if (myDomain.has_value())
      {
        if (aU < myDomain->Surface.UMin - theTol || aU > myDomain->Surface.UMax + theTol
            || aV < myDomain->Surface.VMin - theTol || aV > myDomain->Surface.VMax + theTol)
        {
          return;
        }
      }

      const double aSqDist = aCircPt.SquareDistance(aPlanePt);

      ExtremaCS::ExtremumResult anExt;
      anExt.ParameterOnCurve    = aT;
      anExt.ParameterOnSurfaceU = aU;
      anExt.ParameterOnSurfaceV = aV;
      anExt.PointOnCurve        = aCircPt;
      anExt.PointOnSurface      = aPlanePt;
      anExt.SquareDistance      = aSqDist;
      anExt.IsMinimum           = theIsMin;
      myResult.Extrema.Append(anExt);
    };

    // Signed distance from circle center to plane
    const double aCenterDist = gp_Vec(aPlaneP, aCenter).Dot(gp_Vec(aPlaneN));

    // Maximum amplitude of circle's deviation from center in plane normal direction
    // This is R * sqrt(aXdotN² + aYdotN²) since:
    // z(t) = d + R * (aXdotN * cos(t) + aYdotN * sin(t))
    // and max of (aXdotN * cos(t) + aYdotN * sin(t)) is sqrt(aXdotN² + aYdotN²)
    const double aAmplitude = aRadius * std::sqrt(aXdotN * aXdotN + aYdotN * aYdotN);

    // Check if circle intersects the plane
    if (std::abs(aCenterDist) <= aAmplitude + theTol)
    {
      // Circle intersects the plane - find intersection parameters
      // z(t) = d + R * (aXdotN * cos(t) + aYdotN * sin(t)) = 0
      // Let A = R * aXdotN, B = R * aYdotN
      // A * cos(t) + B * sin(t) = -d
      // sqrt(A² + B²) * sin(t + phi) = -d where tan(phi) = A/B
      const double aA = aRadius * aXdotN;
      const double aB = aRadius * aYdotN;
      const double aAmp = std::sqrt(aA * aA + aB * aB);

      if (aAmp > theTol)
      {
        const double aSinAlpha = -aCenterDist / aAmp;
        const double aClampedSinAlpha = std::max(-1.0, std::min(1.0, aSinAlpha));
        const double aAlpha = std::asin(aClampedSinAlpha);
        const double aPhi = std::atan2(aA, aB);

        // Two solutions: t = aAlpha - aPhi and t = PI - aAlpha - aPhi
        const double aT1 = aAlpha - aPhi;
        const double aT2 = M_PI - aAlpha - aPhi;

        if (theMode != ExtremaCS::SearchMode::Max)
        {
          addExtremum(aT1, true);
          if (std::abs(ExtremaCS::NormalizeAngle(aT2) - ExtremaCS::NormalizeAngle(aT1)) > theTol)
          {
            addExtremum(aT2, true);
          }
        }
      }

      // Also add maximum (farthest point from plane)
      if (theMode != ExtremaCS::SearchMode::Min)
      {
        // Maximum is at t where derivative is zero and gives max distance
        // tan(t) = (Y.N) / (X.N)
        const double aTmax1 = std::atan2(aYdotN, aXdotN);
        const double aTmax2 = aTmax1 + M_PI;

        gp_Pnt aPt1 = ElCLib::Value(aTmax1, myCircle);
        gp_Pnt aPt2 = ElCLib::Value(aTmax2, myCircle);
        double aDist1 = std::abs(gp_Vec(aPlaneP, aPt1).Dot(gp_Vec(aPlaneN)));
        double aDist2 = std::abs(gp_Vec(aPlaneP, aPt2).Dot(gp_Vec(aPlaneN)));

        addExtremum((aDist1 >= aDist2) ? aTmax1 : aTmax2, false);
      }
    }
    else
    {
      // Circle does not intersect plane - extrema at derivative zeros
      const double aT1 = std::atan2(aYdotN, aXdotN);
      const double aT2 = aT1 + M_PI;

      gp_Pnt aPt1 = ElCLib::Value(aT1, myCircle);
      gp_Pnt aPt2 = ElCLib::Value(aT2, myCircle);
      double aDist1 = std::abs(gp_Vec(aPlaneP, aPt1).Dot(gp_Vec(aPlaneN)));
      double aDist2 = std::abs(gp_Vec(aPlaneP, aPt2).Dot(gp_Vec(aPlaneN)));

      double aTmin = (aDist1 <= aDist2) ? aT1 : aT2;
      double aTmax = (aDist1 <= aDist2) ? aT2 : aT1;

      if (theMode != ExtremaCS::SearchMode::Max)
      {
        addExtremum(aTmin, true);
      }
      if (theMode != ExtremaCS::SearchMode::Min)
      {
        addExtremum(aTmax, false);
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaCS::Status::NoSolution
                                                  : ExtremaCS::Status::OK;
    return myResult;
  }

private:
  gp_Circ                            myCircle; //!< Circle geometry
  gp_Pln                             myPlane;  //!< Plane geometry
  std::optional<ExtremaCS::Domain3D> myDomain; //!< Optional domain
  mutable ExtremaCS::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaCS_CirclePlane_HeaderFile
