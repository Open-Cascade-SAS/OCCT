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

#ifndef _ExtremaCS_EllipseSphere_HeaderFile
#define _ExtremaCS_EllipseSphere_HeaderFile

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <ExtremaCS.hxx>
#include <MathRoot_Trig.hxx>
#include <gp_Elips.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Vec.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Ellipse-Sphere extrema computation.
//!
//! Computes extrema between a 3D ellipse and a sphere.
//!
//! The problem finds points on the ellipse that are closest/farthest
//! from the sphere surface.
//!
//! For ellipse E(t) = C + a*cos(t)*X + b*sin(t)*Y and sphere center S:
//! d|E(t) - S|^2/dt = 2*(E(t) - S).E'(t) = 0
//! E'(t) = -a*sin(t)*X + b*cos(t)*Y
//!
//! Expanding: (E(t) - S).(-a*sin(t)*X + b*cos(t)*Y) = 0
//!
//! This is a trigonometric equation solved using MathRoot::TrigonometricCDE.
class ExtremaCS_EllipseSphere
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with ellipse and sphere geometry.
  ExtremaCS_EllipseSphere(const gp_Elips& theEllipse, const gp_Sphere& theSphere)
      : myEllipse(theEllipse),
        mySphere(theSphere),
        myDomain(std::nullopt)
  {
  }

  //! Constructor with ellipse and sphere geometry and domain.
  ExtremaCS_EllipseSphere(const gp_Elips&                      theEllipse,
                          const gp_Sphere&                     theSphere,
                          const std::optional<ExtremaCS::Domain3D>& theDomain)
      : myEllipse(theEllipse),
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

    const gp_Pnt& aEllCenter = myEllipse.Location();
    const gp_Dir& aEllipseX = myEllipse.XAxis().Direction();
    const gp_Dir& aEllipseY = myEllipse.YAxis().Direction();
    const double aMajor = myEllipse.MajorRadius();
    const double aMinor = myEllipse.MinorRadius();

    const gp_Pnt& aSphCenter = mySphere.Location();
    const double aSphRadius = mySphere.Radius();

    // Vector from ellipse center to sphere center
    gp_Vec aCS(aEllCenter, aSphCenter);

    // Project sphere center onto ellipse plane
    const gp_Dir& aN = myEllipse.Axis().Direction();
    const double aDistToPlane = aCS.Dot(gp_Vec(aN));

    // Projection of sphere center onto ellipse plane
    gp_Pnt aProjCenter = aSphCenter.Translated(-aDistToPlane * gp_Vec(aN));

    // Vector from ellipse center to projected sphere center (in ellipse plane)
    gp_Vec aCP(aEllCenter, aProjCenter);
    const double aDistInPlane = aCP.Magnitude();

    // Check for special case: sphere center on ellipse axis
    if (aDistInPlane < Precision::Confusion())
    {
      // All points on ellipse are NOT equidistant from sphere center
      // (unlike circle case) due to different radii.
      // Need to find min/max on the ellipse.
      // Distance squared: d^2 = (a*cos(t))^2 + (b*sin(t))^2 + h^2
      // where h = aDistToPlane
      // d(d^2)/dt = 2*a^2*cos(t)*(-sin(t)) + 2*b^2*sin(t)*cos(t) = 0
      // sin(t)*cos(t)*(b^2 - a^2) = 0
      // Solutions: t = 0, PI/2, PI, 3*PI/2

      auto addExtremumOnAxis = [this, &theTol, &aSphCenter, &aSphRadius](double theT,
                                                                          bool   theCheckMin) {
        double aT = ExtremaCS::NormalizeAngle(theT);

        // Check curve domain
        if (myDomain.has_value())
        {
          if (aT < myDomain->Curve.Min - theTol || aT > myDomain->Curve.Max + theTol)
          {
            double aTplus = aT + ExtremaCS::THE_TWO_PI;
            double aTminus = aT - ExtremaCS::THE_TWO_PI;
            if (aTplus >= myDomain->Curve.Min - theTol && aTplus <= myDomain->Curve.Max + theTol)
              aT = aTplus;
            else if (aTminus >= myDomain->Curve.Min - theTol && aTminus <= myDomain->Curve.Max + theTol)
              aT = aTminus;
            else
              return;
          }
        }

        const gp_Pnt aEllPt = ElCLib::Value(aT, myEllipse);
        const double aDistToCenter = aEllPt.Distance(aSphCenter);
        const double aDistToSurface = std::abs(aDistToCenter - aSphRadius);

        gp_Vec aDir(aSphCenter, aEllPt);
        gp_Pnt aSphPt;
        if (aDir.Magnitude() > Precision::Confusion())
        {
          aDir.Normalize();
          aSphPt = aSphCenter.Translated(aSphRadius * aDir);
        }
        else
        {
          aSphPt = ElSLib::Value(0.0, 0.0, mySphere);
        }

        double aU, aV;
        ElSLib::Parameters(mySphere, aSphPt, aU, aV);

        // Check surface domain
        if (myDomain.has_value())
        {
          if (aV < myDomain->Surface.VMin - theTol || aV > myDomain->Surface.VMax + theTol)
            return;
          if (aU < myDomain->Surface.UMin - theTol)
            aU += ExtremaCS::THE_TWO_PI;
          if (aU > myDomain->Surface.UMax + theTol)
            aU -= ExtremaCS::THE_TWO_PI;
          if (aU < myDomain->Surface.UMin - theTol || aU > myDomain->Surface.UMax + theTol)
            return;
        }

        ExtremaCS::ExtremumResult anExt;
        anExt.ParameterOnCurve    = aT;
        anExt.ParameterOnSurfaceU = aU;
        anExt.ParameterOnSurfaceV = aV;
        anExt.PointOnCurve        = aEllPt;
        anExt.PointOnSurface      = aSphPt;
        anExt.SquareDistance      = aDistToSurface * aDistToSurface;
        anExt.IsMinimum           = theCheckMin;
        myResult.Extrema.Append(anExt);
      };

      // For a >= b: t=0, PI give max distance (major axis), t=PI/2, 3*PI/2 give min
      // For a < b: opposite
      if (aMajor >= aMinor)
      {
        if (theMode != ExtremaCS::SearchMode::Max)
        {
          addExtremumOnAxis(M_PI / 2.0, true);
          addExtremumOnAxis(3.0 * M_PI / 2.0, true);
        }
        if (theMode != ExtremaCS::SearchMode::Min)
        {
          addExtremumOnAxis(0.0, false);
          addExtremumOnAxis(M_PI, false);
        }
      }
      else
      {
        if (theMode != ExtremaCS::SearchMode::Max)
        {
          addExtremumOnAxis(0.0, true);
          addExtremumOnAxis(M_PI, true);
        }
        if (theMode != ExtremaCS::SearchMode::Min)
        {
          addExtremumOnAxis(M_PI / 2.0, false);
          addExtremumOnAxis(3.0 * M_PI / 2.0, false);
        }
      }

      myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaCS::Status::NoSolution
                                                    : ExtremaCS::Status::OK;
      return myResult;
    }

    // General case: use trigonometric solver
    // E(t) - S = (C - S) + a*cos(t)*X + b*sin(t)*Y
    // E'(t) = -a*sin(t)*X + b*cos(t)*Y
    // (E(t) - S).E'(t) = 0
    //
    // Let W = C - S = -aCS
    // W.X = -aPx, W.Y = -aPy (since aCS = C to S, but W = C - S)
    //
    // Actually, let's recompute more carefully:
    // W = aEllCenter - aSphCenter
    gp_Vec aW(aSphCenter, aEllCenter);
    const double aWx = aW.Dot(gp_Vec(aEllipseX));
    const double aWy = aW.Dot(gp_Vec(aEllipseY));

    // E(t) - S = W + a*cos(t)*X + b*sin(t)*Y
    // E'(t) = -a*sin(t)*X + b*cos(t)*Y
    //
    // (E(t) - S).E'(t) = W.(-a*sin(t)*X + b*cos(t)*Y)
    //                    + a*cos(t)*X.(-a*sin(t)*X + b*cos(t)*Y)
    //                    + b*sin(t)*Y.(-a*sin(t)*X + b*cos(t)*Y)
    //
    // = -a*Wx*sin(t) + b*Wy*cos(t)
    //   - a^2*cos(t)*sin(t) + a*b*cos^2(t)
    //   - a*b*sin^2(t) + b^2*sin(t)*cos(t)
    //
    // = -a*Wx*sin(t) + b*Wy*cos(t) + (b^2 - a^2)*sin(t)*cos(t) + a*b*(cos^2(t) - sin^2(t))
    //
    // Using: sin(t)*cos(t) = sin(2t)/2, cos^2(t) - sin^2(t) = cos(2t)
    // = -a*Wx*sin(t) + b*Wy*cos(t) + (b^2 - a^2)*sin(2t)/2 + a*b*cos(2t)
    //
    // This is a general trigonometric equation. Use MathRoot::Trigonometric for full equation.
    // The equation has form: A*cos^2(t) + 2*B*cos(t)*sin(t) + C*cos(t) + D*sin(t) + E = 0
    //
    // Rewriting our equation:
    // -a*Wx*sin(t) + b*Wy*cos(t) + (b^2 - a^2)/2 * 2*sin(t)*cos(t) + a*b*(cos^2(t) - sin^2(t)) = 0
    //
    // = a*b*cos^2(t) + (b^2 - a^2)/2 * 2*sin(t)*cos(t) + b*Wy*cos(t) - a*Wx*sin(t) - a*b*sin^2(t)
    // = a*b*cos^2(t) + (b^2 - a^2)*sin(t)*cos(t) + b*Wy*cos(t) - a*Wx*sin(t) - a*b*(1 - cos^2(t))
    // = 2*a*b*cos^2(t) + (b^2 - a^2)*sin(t)*cos(t) + b*Wy*cos(t) - a*Wx*sin(t) - a*b
    //
    // Mapping to: A*cos^2(t) + 2*B*cos(t)*sin(t) + C*cos(t) + D*sin(t) + E = 0
    // A = 2*a*b
    // 2*B = (b^2 - a^2), so B = (b^2 - a^2)/2
    // C = b*Wy
    // D = -a*Wx
    // E = -a*b

    const double aA = 2.0 * aMajor * aMinor;
    const double aB = (aMinor * aMinor - aMajor * aMajor) / 2.0;
    const double aC = aMinor * aWy;
    const double aD = -aMajor * aWx;
    const double aE = -aMajor * aMinor;

    // Get domain bounds
    double aInfBound = 0.0;
    double aSupBound = ExtremaCS::THE_TWO_PI;
    if (myDomain.has_value())
    {
      aInfBound = myDomain->Curve.Min;
      aSupBound = myDomain->Curve.Max;
    }

    MathRoot::TrigResult aTrigResult = MathRoot::Trigonometric(aA, aB, aC, aD, aE,
                                                                aInfBound, aSupBound);

    if (!aTrigResult.IsDone() && !aTrigResult.InfiniteRoots)
    {
      myResult.Status = ExtremaCS::Status::NumericalError;
      return myResult;
    }

    if (aTrigResult.InfiniteRoots)
    {
      // Degenerate case - shouldn't happen for non-zero ellipse
      myResult.Status = ExtremaCS::Status::InfiniteSolutions;
      return myResult;
    }

    // Helper to add extremum
    auto addExtremum = [this, &theTol, &aSphCenter, &aSphRadius](double theT) {
      double aT = theT;

      // Check curve domain
      if (myDomain.has_value())
      {
        if (aT < myDomain->Curve.Min - theTol || aT > myDomain->Curve.Max + theTol)
        {
          return;
        }
      }

      const gp_Pnt aEllPt = ElCLib::Value(aT, myEllipse);
      gp_Vec aDir(aSphCenter, aEllPt);
      const double aDistToCenter = aDir.Magnitude();

      gp_Pnt aSphPt;
      if (aDistToCenter > Precision::Confusion())
      {
        aDir.Normalize();
        aSphPt = aSphCenter.Translated(aSphRadius * aDir);
      }
      else
      {
        aSphPt = ElSLib::Value(0.0, 0.0, mySphere);
      }

      double aU, aV;
      ElSLib::Parameters(mySphere, aSphPt, aU, aV);

      // Check surface domain
      if (myDomain.has_value())
      {
        if (aV < myDomain->Surface.VMin - theTol || aV > myDomain->Surface.VMax + theTol)
          return;
        if (aU < myDomain->Surface.UMin - theTol)
          aU += ExtremaCS::THE_TWO_PI;
        if (aU > myDomain->Surface.UMax + theTol)
          aU -= ExtremaCS::THE_TWO_PI;
        if (aU < myDomain->Surface.UMin - theTol || aU > myDomain->Surface.UMax + theTol)
          return;
      }

      const double aDistToSurface = std::abs(aDistToCenter - aSphRadius);

      ExtremaCS::ExtremumResult anExt;
      anExt.ParameterOnCurve    = aT;
      anExt.ParameterOnSurfaceU = aU;
      anExt.ParameterOnSurfaceV = aV;
      anExt.PointOnCurve        = aEllPt;
      anExt.PointOnSurface      = aSphPt;
      anExt.SquareDistance      = aDistToSurface * aDistToSurface;
      anExt.IsMinimum           = false;  // Will be set later
      myResult.Extrema.Append(anExt);
    };

    // Add all roots found
    for (int i = 0; i < aTrigResult.NbRoots; ++i)
    {
      addExtremum(aTrigResult.Roots[i]);
    }

    // Determine which are minima and which are maxima
    if (!myResult.Extrema.IsEmpty())
    {
      double aMinDist = std::numeric_limits<double>::max();
      for (int i = 0; i < myResult.Extrema.Length(); ++i)
      {
        const double aDist = myResult.Extrema.Value(i).SquareDistance;
        if (aDist < aMinDist) aMinDist = aDist;
      }

      for (int i = 0; i < myResult.Extrema.Length(); ++i)
      {
        const double aDist = myResult.Extrema.Value(i).SquareDistance;
        myResult.Extrema.ChangeValue(i).IsMinimum = (std::abs(aDist - aMinDist) < theTol);
      }

      // Filter based on search mode by copying to new vector
      if (theMode != ExtremaCS::SearchMode::MinMax)
      {
        NCollection_Vector<ExtremaCS::ExtremumResult> aFiltered;
        for (int i = 0; i < myResult.Extrema.Length(); ++i)
        {
          const ExtremaCS::ExtremumResult& anExt = myResult.Extrema.Value(i);
          if ((theMode == ExtremaCS::SearchMode::Min && anExt.IsMinimum)
              || (theMode == ExtremaCS::SearchMode::Max && !anExt.IsMinimum))
          {
            aFiltered.Append(anExt);
          }
        }
        myResult.Extrema = std::move(aFiltered);
      }
    }

    myResult.Status = myResult.Extrema.IsEmpty() ? ExtremaCS::Status::NoSolution
                                                  : ExtremaCS::Status::OK;
    return myResult;
  }

private:
  gp_Elips                           myEllipse; //!< Ellipse geometry
  gp_Sphere                          mySphere;  //!< Sphere geometry
  std::optional<ExtremaCS::Domain3D> myDomain;  //!< Optional domain
  mutable ExtremaCS::Result          myResult;  //!< Reusable result storage
};

#endif // _ExtremaCS_EllipseSphere_HeaderFile
