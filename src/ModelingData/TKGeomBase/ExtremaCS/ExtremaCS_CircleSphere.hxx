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

#ifndef _ExtremaCS_CircleSphere_HeaderFile
#define _ExtremaCS_CircleSphere_HeaderFile

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Circle-Sphere extrema computation.
//!
//! Computes extrema between a 3D circle and a sphere.
//!
//! The problem reduces to finding points on the circle that are closest/farthest
//! from the sphere center, then offsetting by the sphere radius.
//!
//! For circle C(t) and sphere center S:
//! d|C(t) - S|^2/dt = 2*(C(t) - S).C'(t) = 0
//!
//! This gives the points on circle closest/farthest from sphere center.
//! The actual extrema are on the line from center through these points,
//! at distance R from center.
class ExtremaCS_CircleSphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with circle and sphere geometry.
  ExtremaCS_CircleSphere(const gp_Circ& theCircle, const gp_Sphere& theSphere)
      : myCircle(theCircle),
        mySphere(theSphere),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with circle and sphere geometry and domain.
  ExtremaCS_CircleSphere(const gp_Circ&                            theCircle,
                         const gp_Sphere&                          theSphere,
                         const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myCircle(theCircle),
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

    const gp_Pnt& aCircCenter = myCircle.Location();
    const gp_Dir& aCircleX    = myCircle.XAxis().Direction();
    const gp_Dir& aCircleY    = myCircle.YAxis().Direction();
    const gp_Pnt& aSphCenter  = mySphere.Location();
    const double  aSphRadius  = mySphere.Radius();

    // Project sphere center onto circle plane
    const gp_Dir& aN = myCircle.Axis().Direction();
    gp_Vec        aCS(aCircCenter, aSphCenter);

    // Distance from sphere center to circle plane
    const double aDistToPlane = aCS.Dot(gp_Vec(aN));

    // Projection of sphere center onto circle plane
    gp_Pnt aProjCenter = aSphCenter.Translated(-aDistToPlane * gp_Vec(aN));

    // Vector from circle center to projected sphere center
    gp_Vec       aCP(aCircCenter, aProjCenter);
    const double aDistInPlane = aCP.Magnitude();

    // Check for concentric circles (sphere center on circle axis)
    if (aDistInPlane < Precision::Confusion())
    {
      // Infinite solutions - all points on circle are equidistant from sphere center
      // Distance from any circle point to sphere center:
      // sqrt(circleRadius^2 + aDistToPlane^2)
      const double aCircleRadius = myCircle.Radius();
      const double aDistToCenter =
        std::sqrt(aCircleRadius * aCircleRadius + aDistToPlane * aDistToPlane);
      const double aDistToSurface     = std::abs(aDistToCenter - aSphRadius);
      myResult.Status                 = ExtremaCS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = aDistToSurface * aDistToSurface;
      return myResult;
    }

    // Direction from circle center toward projected sphere center
    aCP.Normalize();

    // Parameter at closest point to projected center
    // C(t) = center + R*(cos(t)*X + sin(t)*Y)
    // We want t such that C(t) - CircCenter points toward ProjCenter
    const double aCosT = aCP.Dot(gp_Vec(aCircleX));
    const double aSinT = aCP.Dot(gp_Vec(aCircleY));
    const double aT1   = std::atan2(aSinT, aCosT);
    const double aT2   = aT1 + M_PI;

    // Helper to add extremum
    auto addExtremum = [this, &theTol, &aSphCenter, &aSphRadius](double theT, bool theIsMin) {
      // Normalize to [0, 2*PI)
      double aT = ExtremaCS::NormalizeAngle(theT);

      // Check curve domain
      if (myDomain.has_value())
      {
        if (aT < myDomain->Curve.Min - theTol || aT > myDomain->Curve.Max + theTol)
        {
          double aT2 = aT + ExtremaCS::THE_TWO_PI;
          double aT3 = aT - ExtremaCS::THE_TWO_PI;
          if (aT2 >= myDomain->Curve.Min - theTol && aT2 <= myDomain->Curve.Max + theTol)
          {
            aT = aT2;
          }
          else if (aT3 >= myDomain->Curve.Min - theTol && aT3 <= myDomain->Curve.Max + theTol)
          {
            aT = aT3;
          }
          else
          {
            return;
          }
        }
      }

      // Point on circle
      const gp_Pnt aCircPt = ElCLib::Value(aT, myCircle);

      // Direction from sphere center to circle point
      gp_Vec       aDir(aSphCenter, aCircPt);
      const double aDist = aDir.Magnitude();

      gp_Pnt aSphPt;
      if (aDist > Precision::Confusion())
      {
        aDir.Normalize();
        aSphPt = aSphCenter.Translated(aSphRadius * aDir);
      }
      else
      {
        // Circle point coincides with sphere center - use any direction
        aSphPt = aSphCenter.Translated(aSphRadius * gp_Vec(1.0, 0.0, 0.0));
      }

      // Get UV on sphere
      double aU, aV;
      ElSLib::Parameters(mySphere, aSphPt, aU, aV);

      // Check surface domain
      if (myDomain.has_value())
      {
        if (aV < myDomain->Surface.VMin - theTol || aV > myDomain->Surface.VMax + theTol)
        {
          return;
        }
        // U is periodic
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
      anExt.ParameterOnCurve    = aT;
      anExt.ParameterOnSurfaceU = aU;
      anExt.ParameterOnSurfaceV = aV;
      anExt.PointOnCurve        = aCircPt;
      anExt.PointOnSurface      = aSphPt;
      anExt.SquareDistance      = aCircPt.SquareDistance(aSphPt);
      anExt.IsMinimum           = theIsMin;
      myResult.Extrema.Append(anExt);
    };

    // T1 is closest to projected center, T2 is farthest
    // But we need to check which gives min/max distance to sphere surface

    gp_Pnt aPt1   = ElCLib::Value(aT1, myCircle);
    gp_Pnt aPt2   = ElCLib::Value(aT2, myCircle);
    double aDist1 = aPt1.Distance(aSphCenter);
    double aDist2 = aPt2.Distance(aSphCenter);

    // Distance to sphere surface
    double aDistSurf1 = std::abs(aDist1 - aSphRadius);
    double aDistSurf2 = std::abs(aDist2 - aSphRadius);

    if (theMode != ExtremaCS::SearchMode::Max)
    {
      if (aDistSurf1 <= aDistSurf2)
      {
        addExtremum(aT1, true);
      }
      else
      {
        addExtremum(aT2, true);
      }
    }

    if (theMode != ExtremaCS::SearchMode::Min)
    {
      if (aDistSurf1 >= aDistSurf2)
      {
        addExtremum(aT1, false);
      }
      else
      {
        addExtremum(aT2, false);
      }
    }

    myResult.Status =
      myResult.Extrema.IsEmpty() ? ExtremaCS::Status::NoSolution : ExtremaCS::Status::OK;
    return myResult;
  }

private:
  gp_Circ                            myCircle; //!< Circle geometry
  gp_Sphere                          mySphere; //!< Sphere geometry
  std::optional<ExtremaCS::Domain3D> myDomain; //!< Optional domain
  mutable ExtremaCS::Result          myResult; //!< Reusable result storage
};

#endif // _ExtremaCS_CircleSphere_HeaderFile
