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

#ifndef _ExtremaCS_EllipsePlane_HeaderFile
#define _ExtremaCS_EllipsePlane_HeaderFile

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <MathRoot_Trig.hxx>
#include <gp_Elips.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Ellipse-Plane extrema computation.
//!
//! Computes extrema between a 3D ellipse and a plane.
//!
//! For ellipse E(t) = Center + a*cos(t)*X + b*sin(t)*Y
//! and plane with normal N:
//!
//! Distance from E(t) to plane = |(E(t) - P0) . N|
//!
//! d(Dist)/dt = (-a*sin(t)*X + b*cos(t)*Y) . N = 0
//! => tan(t) = (b*Y.N) / (a*X.N)
//!
//! Two extrema exist at t and t + PI (minimum and maximum distance).
//! Special case: Ellipse parallel to plane gives infinite solutions.
class ExtremaCS_EllipsePlane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with ellipse and plane geometry.
  ExtremaCS_EllipsePlane(const gp_Elips& theEllipse, const gp_Pln& thePlane)
      : myEllipse(theEllipse),
        myPlane(thePlane),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with ellipse and plane geometry and domain.
  ExtremaCS_EllipsePlane(const gp_Elips&                           theEllipse,
                         const gp_Pln&                             thePlane,
                         const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myEllipse(theEllipse),
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

    const gp_Pnt& aCenter   = myEllipse.Location();
    const gp_Dir& aEllipseX = myEllipse.XAxis().Direction();
    const gp_Dir& aEllipseY = myEllipse.YAxis().Direction();
    const double  aMajor    = myEllipse.MajorRadius();
    const double  aMinor    = myEllipse.MinorRadius();

    const gp_Dir& aPlaneN = myPlane.Axis().Direction();
    const gp_Pnt& aPlaneP = myPlane.Location();

    // Check if ellipse is parallel to plane
    const double aXdotN = gp_Vec(aEllipseX).Dot(gp_Vec(aPlaneN));
    const double aYdotN = gp_Vec(aEllipseY).Dot(gp_Vec(aPlaneN));

    if (std::abs(aXdotN) < ExtremaCS::THE_PARALLEL_TOLERANCE
        && std::abs(aYdotN) < ExtremaCS::THE_PARALLEL_TOLERANCE)
    {
      // Ellipse is parallel to plane - infinite solutions
      const double aDist              = gp_Vec(aPlaneP, aCenter).Dot(gp_Vec(aPlaneN));
      myResult.Status                 = ExtremaCS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aDist * aDist;
      return myResult;
    }

    // Helper to add extremum
    auto addExtremum = [this, &theTol, &aPlaneN](double theT, bool theIsMin) {
      // Normalize to [0, 2*PI)
      double aT = ExtremaCS::NormalizeAngle(theT);

      // Check curve domain
      if (myDomain.has_value())
      {
        if (aT < myDomain->Curve.Min - theTol || aT > myDomain->Curve.Max + theTol)
        {
          double aTplus  = aT + ExtremaCS::THE_TWO_PI;
          double aTminus = aT - ExtremaCS::THE_TWO_PI;
          if (aTplus >= myDomain->Curve.Min - theTol && aTplus <= myDomain->Curve.Max + theTol)
          {
            aT = aTplus;
          }
          else if (aTminus >= myDomain->Curve.Min - theTol
                   && aTminus <= myDomain->Curve.Max + theTol)
          {
            aT = aTminus;
          }
          else
          {
            return;
          }
        }
      }

      // Compute point on ellipse
      const gp_Pnt aElliPt = ElCLib::Value(aT, myEllipse);

      // Compute projection on plane
      const double aSignedDist = gp_Vec(myPlane.Location(), aElliPt).Dot(gp_Vec(aPlaneN));
      const gp_Pnt aPlanePt    = aElliPt.Translated(-aSignedDist * gp_Vec(aPlaneN));

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

      ExtremaCS::ExtremumResult anExt;
      anExt.ParameterOnCurve    = aT;
      anExt.ParameterOnSurfaceU = aU;
      anExt.ParameterOnSurfaceV = aV;
      anExt.PointOnCurve        = aElliPt;
      anExt.PointOnSurface      = aPlanePt;
      anExt.SquareDistance      = aElliPt.SquareDistance(aPlanePt);
      anExt.IsMinimum           = theIsMin;
      myResult.Extrema.Append(anExt);
    };

    // Signed distance from ellipse center to plane
    const double aCenterDist = gp_Vec(aPlaneP, aCenter).Dot(gp_Vec(aPlaneN));

    // Maximum amplitude of ellipse's deviation from center in plane normal direction
    // z(t) = d + a*cos(t)*(X.N) + b*sin(t)*(Y.N)
    // Let A = a*(X.N), B = b*(Y.N)
    // max of (A*cos(t) + B*sin(t)) is sqrt(A^2 + B^2)
    const double aA         = aMajor * aXdotN;
    const double aB         = aMinor * aYdotN;
    const double aAmplitude = std::sqrt(aA * aA + aB * aB);

    // Get domain bounds for filtering
    double aInfBound = 0.0;
    double aSupBound = ExtremaCS::THE_TWO_PI;
    if (myDomain.has_value())
    {
      aInfBound = myDomain->Curve.Min;
      aSupBound = myDomain->Curve.Max;
    }

    // Check if ellipse intersects the plane
    if (std::abs(aCenterDist) <= aAmplitude + theTol)
    {
      // Ellipse intersects the plane - find intersection parameters using TrigonometricCDE
      // A*cos(t) + B*sin(t) + d = 0
      if (theMode != ExtremaCS::SearchMode::Max)
      {
        MathRoot::TrigResult aTrigResult =
          MathRoot::TrigonometricCDE(aA, aB, aCenterDist, aInfBound, aSupBound);
        if (aTrigResult.IsDone())
        {
          for (int i = 0; i < aTrigResult.NbRoots; ++i)
          {
            addExtremum(aTrigResult.Roots[i], true);
          }
        }
      }

      // Also add maximum (farthest point from plane)
      if (theMode != ExtremaCS::SearchMode::Min)
      {
        // Maximum is at t where derivative is zero and gives max distance
        // tan(t) = (b*Y.N) / (a*X.N)
        const double aTmax1 = std::atan2(aMinor * aYdotN, aMajor * aXdotN);
        const double aTmax2 = aTmax1 + M_PI;

        gp_Pnt aPt1   = ElCLib::Value(aTmax1, myEllipse);
        gp_Pnt aPt2   = ElCLib::Value(aTmax2, myEllipse);
        double aDist1 = std::abs(gp_Vec(aPlaneP, aPt1).Dot(gp_Vec(aPlaneN)));
        double aDist2 = std::abs(gp_Vec(aPlaneP, aPt2).Dot(gp_Vec(aPlaneN)));

        addExtremum((aDist1 >= aDist2) ? aTmax1 : aTmax2, false);
      }
    }
    else
    {
      // Ellipse does not intersect plane - extrema at derivative zeros
      const double aT1 = std::atan2(aMinor * aYdotN, aMajor * aXdotN);
      const double aT2 = aT1 + M_PI;

      gp_Pnt aPt1   = ElCLib::Value(aT1, myEllipse);
      gp_Pnt aPt2   = ElCLib::Value(aT2, myEllipse);
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

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaCS::Status::NoSolution : ExtremaCS::Status::OK;
    return myResult;
  }

private:
  gp_Elips                           myEllipse; //!< Ellipse geometry
  gp_Pln                             myPlane;   //!< Plane geometry
  std::optional<ExtremaCS::Domain3D> myDomain;  //!< Optional domain
  mutable ExtremaCS::Result          myResult;  //!< Reusable result storage
};

#endif // _ExtremaCS_EllipsePlane_HeaderFile
