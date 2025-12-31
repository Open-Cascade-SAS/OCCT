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

#ifndef _ExtremaSS_ConeTorus_HeaderFile
#define _ExtremaSS_ConeTorus_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>
#include <MathOpt_Powell.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <limits>
#include <optional>

//! Analytical computation of extrema between a cone and a torus.
//!
//! Both surfaces have complex geometry: the cone has varying radius along V,
//! and the torus is a surface of revolution with generating circles.
//! This requires 2D optimization over the combined parameter space.
//!
//! **Parameterization:**
//! - Cone: P(U, V) = Apex + V*cos(α)*Axis + V*sin(α)*(cos(U)*XDir + sin(U)*YDir)
//! - Torus: P(U, V) = Center + (R + r*cos(V))*(cos(U)*XDir + sin(U)*YDir) + r*sin(V)*Axis
class ExtremaSS_ConeTorus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor from cone and torus.
  //! @param theCone The cone
  //! @param theTorus The torus
  ExtremaSS_ConeTorus(const gp_Cone& theCone, const gp_Torus& theTorus)
      : myCone(theCone),
        myTorus(theTorus),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor from torus and cone (swapped order).
  //! @param theTorus The torus
  //! @param theCone The cone
  ExtremaSS_ConeTorus(const gp_Torus& theTorus, const gp_Cone& theCone)
      : myCone(theCone),
        myTorus(theTorus),
        mySwapped(true)
  {
    initCache();
  }

  //! Constructor with bounded domain.
  //! @param theCone The cone
  //! @param theTorus The torus
  //! @param theDomain Bounded domain for both surfaces
  ExtremaSS_ConeTorus(const gp_Cone&             theCone,
                      const gp_Torus&            theTorus,
                      const ExtremaSS::Domain4D& theDomain)
      : myCone(theCone),
        myTorus(theTorus),
        myDomain(theDomain),
        mySwapped(false)
  {
    initCache();
  }

  //! Constructor with bounded domain (swapped order).
  //! @param theTorus The torus
  //! @param theCone The cone
  //! @param theDomain Bounded domain for both surfaces
  ExtremaSS_ConeTorus(const gp_Torus&            theTorus,
                      const gp_Cone&             theCone,
                      const ExtremaSS::Domain4D& theDomain)
      : myCone(theCone),
        myTorus(theTorus),
        myDomain(ExtremaSS::Domain4D{theDomain.Domain2, theDomain.Domain1}),
        mySwapped(true)
  {
    initCache();
  }

  //! Perform extrema computation.
  //! @param theTol Tolerance for considering points as equal
  //! @param theMode Search mode (Min, Max, or MinMax)
  //! @return Result containing extremum points and distances
  [[nodiscard]] const ExtremaSS::Result& Perform(
      double                theTol,
      ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    if (myDomain.has_value())
    {
      return PerformWithBoundary(theTol, theMode);
    }

    computeGeneralCase(theTol, theMode);
    return myResult;
  }

  //! Perform extrema computation including boundary analysis.
  //! @param theTol Tolerance for considering points as equal
  //! @param theMode Search mode (Min, Max, or MinMax)
  //! @return Result containing extremum points and distances
  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
      double                theTol,
      ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    myResult.Clear();

    computeGeneralCase(theTol, theMode);

    if (myDomain.has_value())
    {
      checkBoundaryExtrema(theTol, theMode);
    }

    return myResult;
  }

  //! Evaluate point on cone (surface 1).
  //! @param theU U parameter (angle)
  //! @param theV V parameter (distance from apex)
  //! @return Point on cone
  gp_Pnt Value1(double theU, double theV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aRadius = theV * mySinSemiAngle;
    const double aHeight = theV * myCosSemiAngle;
    return gp_Pnt(myConeApex.X() + aHeight * myConeAxis.X()
                      + aRadius * (aCosU * myConeXDir.X() + aSinU * myConeYDir.X()),
                  myConeApex.Y() + aHeight * myConeAxis.Y()
                      + aRadius * (aCosU * myConeXDir.Y() + aSinU * myConeYDir.Y()),
                  myConeApex.Z() + aHeight * myConeAxis.Z()
                      + aRadius * (aCosU * myConeXDir.Z() + aSinU * myConeYDir.Z()));
  }

  //! Evaluate point on torus (surface 2).
  //! @param theU U parameter (angle around major circle)
  //! @param theV V parameter (angle around minor circle)
  //! @return Point on torus
  gp_Pnt Value2(double theU, double theV) const
  {
    const double aCosU   = std::cos(theU);
    const double aSinU   = std::sin(theU);
    const double aCosV   = std::cos(theV);
    const double aSinV   = std::sin(theV);
    const double aRadius = myMajorRadius + myMinorRadius * aCosV;
    return gp_Pnt(myTorusCenter.X() + aRadius * (aCosU * myTorusXDir.X() + aSinU * myTorusYDir.X())
                      + myMinorRadius * aSinV * myTorusAxis.X(),
                  myTorusCenter.Y() + aRadius * (aCosU * myTorusXDir.Y() + aSinU * myTorusYDir.Y())
                      + myMinorRadius * aSinV * myTorusAxis.Y(),
                  myTorusCenter.Z() + aRadius * (aCosU * myTorusXDir.Z() + aSinU * myTorusYDir.Z())
                      + myMinorRadius * aSinV * myTorusAxis.Z());
  }

  //! Check if surfaces were swapped.
  //! @return True if torus was provided first
  bool IsSwapped() const { return mySwapped; }

  //! Check if domain is bounded.
  //! @return True if bounded domain was provided
  bool IsBounded() const { return myDomain.has_value(); }

private:
  //! Initialize cached values from geometry.
  void initCache()
  {
    // Extract cone data
    const gp_Ax3& aConeAx = myCone.Position();
    myConeApex            = myCone.Apex();
    myConeAxis            = aConeAx.Direction();
    myConeXDir            = aConeAx.XDirection();
    myConeYDir            = aConeAx.YDirection();
    mySemiAngle           = myCone.SemiAngle();
    mySinSemiAngle        = std::sin(mySemiAngle);
    myCosSemiAngle        = std::cos(mySemiAngle);

    // Extract torus data
    const gp_Ax3& aTorusAx = myTorus.Position();
    myTorusCenter          = aTorusAx.Location();
    myTorusAxis            = aTorusAx.Direction();
    myTorusXDir            = aTorusAx.XDirection();
    myTorusYDir            = aTorusAx.YDirection();
    myMajorRadius          = myTorus.MajorRadius();
    myMinorRadius          = myTorus.MinorRadius();
  }

  //! Compute extrema for the general case.
  void computeGeneralCase(double theTol, ExtremaSS::SearchMode theMode) const
  {
    myResult.Status = ExtremaSS::Status::OK;

    // Sample over both cone V and torus U, V
    constexpr int aNbSamples = 36;
    constexpr double aVConeMin = 0.1, aVConeMax = 50.0;

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU1 = 0, aBestMinV1 = 1, aBestMinU2 = 0, aBestMinV2 = 0;
    double aBestMaxU1 = 0, aBestMaxV1 = 1, aBestMaxU2 = 0, aBestMaxV2 = 0;

    for (int iConeV = 0; iConeV <= 20; ++iConeV)
    {
      const double aVCone = aVConeMin + iConeV * (aVConeMax - aVConeMin) / 20;

      for (int iConeU = 0; iConeU < aNbSamples; ++iConeU)
      {
        const double aUCone = 2.0 * M_PI * iConeU / aNbSamples;
        const gp_Pnt aPCone = Value1(aUCone, aVCone);

        for (int iTorusU = 0; iTorusU < aNbSamples; ++iTorusU)
        {
          const double aUTorus = 2.0 * M_PI * iTorusU / aNbSamples;

          for (int iTorusV = 0; iTorusV < aNbSamples; ++iTorusV)
          {
            const double aVTorus  = 2.0 * M_PI * iTorusV / aNbSamples;
            const gp_Pnt aPTorus  = Value2(aUTorus, aVTorus);
            const double aSqDist = aPCone.SquareDistance(aPTorus);

            if (aSqDist < aBestMinSqDist)
            {
              aBestMinSqDist = aSqDist;
              aBestMinU1     = aUCone;
              aBestMinV1     = aVCone;
              aBestMinU2     = aUTorus;
              aBestMinV2     = aVTorus;
            }

            if (aSqDist > aBestMaxSqDist)
            {
              aBestMaxSqDist = aSqDist;
              aBestMaxU1     = aUCone;
              aBestMaxV1     = aVCone;
              aBestMaxU2     = aUTorus;
              aBestMaxV2     = aVTorus;
            }
          }
        }
      }
    }

    // Refine and add extrema
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      refineExtremum(aBestMinU1, aBestMinV1, aBestMinU2, aBestMinV2, true, theTol);
    }

    if (theMode != ExtremaSS::SearchMode::Min)
    {
      refineExtremum(aBestMaxU1, aBestMaxV1, aBestMaxU2, aBestMaxV2, false, theTol);
    }
  }

  //! Refine an extremum using Powell's method (gradient-free optimization).
  void refineExtremum(double theU1,
                      double theV1,
                      double theU2,
                      double theV2,
                      bool   theIsMin,
                      double theTol) const
  {
    struct DistanceFunc
    {
      const ExtremaSS_ConeTorus* myEval;
      bool                       myIsMin;

      DistanceFunc(const ExtremaSS_ConeTorus* theEval, bool theIsMin)
          : myEval(theEval),
            myIsMin(theIsMin)
      {
      }

      bool Value(const math_Vector& theX, double& theF) const
      {
        // Keep V1 positive for cone
        const double aV1     = std::max(theX(2), 0.1);
        const gp_Pnt aP1     = myEval->Value1(theX(1), aV1);
        const gp_Pnt aP2     = myEval->Value2(theX(3), theX(4));
        const double aSqDist = aP1.SquareDistance(aP2);
        theF                 = myIsMin ? aSqDist : -aSqDist;
        return true;
      }
    };

    math_Vector aStartPt(1, 4);
    aStartPt(1) = theU1;
    aStartPt(2) = std::max(theV1, 0.1);
    aStartPt(3) = theU2;
    aStartPt(4) = theV2;

    MathUtils::Config       aConfig(theTol, 50);
    DistanceFunc            aFunc(this, theIsMin);
    MathUtils::VectorResult aOptResult = MathOpt::Powell(aFunc, aStartPt, aConfig);

    double aU1 = theU1, aV1 = theV1, aU2 = theU2, aV2 = theV2;

    if (aOptResult.IsDone() && aOptResult.Solution.has_value())
    {
      const math_Vector& aSol = *aOptResult.Solution;
      aU1                     = aSol(1);
      aV1                     = std::max(aSol(2), 0.1);
      aU2                     = aSol(3);
      aV2                     = aSol(4);
    }

    const gp_Pnt aP1     = Value1(aU1, aV1);
    const gp_Pnt aP2     = Value2(aU2, aV2);
    const double aSqDist = aP1.SquareDistance(aP2);

    addExtremum(aU1, aV1, aU2, aV2, aSqDist, theIsMin, theTol);
  }

  //! Check boundary extrema for bounded domains.
  void checkBoundaryExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (!myDomain.has_value())
      return;

    const ExtremaSS::Domain4D&  aDom  = myDomain.value();
    const MathUtils::Domain2D& aDom1 = aDom.Domain1;
    const MathUtils::Domain2D& aDom2 = aDom.Domain2;

    constexpr int aNbSamples = 20;

    // Check cone boundaries against torus
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom1.UMin + i * (aDom1.UMax - aDom1.UMin) / aNbSamples;
      checkConePointAgainstTorus(aU1, aDom1.VMin, theTol, theMode);
      checkConePointAgainstTorus(aU1, aDom1.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV1 = aDom1.VMin + i * (aDom1.VMax - aDom1.VMin) / aNbSamples;
      checkConePointAgainstTorus(aDom1.UMin, aV1, theTol, theMode);
      checkConePointAgainstTorus(aDom1.UMax, aV1, theTol, theMode);
    }

    // Check torus boundaries against cone
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom2.UMin + i * (aDom2.UMax - aDom2.UMin) / aNbSamples;
      checkTorusPointAgainstCone(aU2, aDom2.VMin, theTol, theMode);
      checkTorusPointAgainstCone(aU2, aDom2.VMax, theTol, theMode);
    }

    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aV2 = aDom2.VMin + i * (aDom2.VMax - aDom2.VMin) / aNbSamples;
      checkTorusPointAgainstCone(aDom2.UMin, aV2, theTol, theMode);
      checkTorusPointAgainstCone(aDom2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a point on cone against torus.
  void checkConePointAgainstTorus(double theU1, double theV1, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP1 = Value1(theU1, theV1);

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU2 = 0, aBestMinV2 = 0;
    double aBestMaxU2 = 0, aBestMaxV2 = 0;

    constexpr int aNbSamples = 36;
    for (int i = 0; i < aNbSamples; ++i)
    {
      const double aU2 = 2.0 * M_PI * i / aNbSamples;
      for (int j = 0; j < aNbSamples; ++j)
      {
        const double aV2     = 2.0 * M_PI * j / aNbSamples;
        const gp_Pnt aP2     = Value2(aU2, aV2);
        const double aSqDist = aP1.SquareDistance(aP2);

        if (aSqDist < aBestMinSqDist)
        {
          aBestMinSqDist = aSqDist;
          aBestMinU2     = aU2;
          aBestMinV2     = aV2;
        }
        if (aSqDist > aBestMaxSqDist)
        {
          aBestMaxSqDist = aSqDist;
          aBestMaxU2     = aU2;
          aBestMaxV2     = aV2;
        }
      }
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      addExtremum(theU1, theV1, aBestMinU2, aBestMinV2, aBestMinSqDist, true, theTol);
    }
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      addExtremum(theU1, theV1, aBestMaxU2, aBestMaxV2, aBestMaxSqDist, false, theTol);
    }
  }

  //! Check a point on torus against cone.
  void checkTorusPointAgainstCone(double theU2, double theV2, double theTol, ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aP2 = Value2(theU2, theV2);

    double aBestMinSqDist = std::numeric_limits<double>::max();
    double aBestMaxSqDist = 0.0;
    double aBestMinU1 = 0, aBestMinV1 = 1;
    double aBestMaxU1 = 0, aBestMaxV1 = 1;

    constexpr int aNbSamples = 36;
    constexpr double aVMin = 0.1, aVMax = 50.0;

    for (int i = 0; i < aNbSamples; ++i)
    {
      const double aU1 = 2.0 * M_PI * i / aNbSamples;
      for (int j = 0; j <= 20; ++j)
      {
        const double aV1     = aVMin + j * (aVMax - aVMin) / 20;
        const gp_Pnt aP1     = Value1(aU1, aV1);
        const double aSqDist = aP1.SquareDistance(aP2);

        if (aSqDist < aBestMinSqDist)
        {
          aBestMinSqDist = aSqDist;
          aBestMinU1     = aU1;
          aBestMinV1     = aV1;
        }
        if (aSqDist > aBestMaxSqDist)
        {
          aBestMaxSqDist = aSqDist;
          aBestMaxU1     = aU1;
          aBestMaxV1     = aV1;
        }
      }
    }

    if (theMode != ExtremaSS::SearchMode::Max)
    {
      addExtremum(aBestMinU1, aBestMinV1, theU2, theV2, aBestMinSqDist, true, theTol);
    }
    if (theMode != ExtremaSS::SearchMode::Min)
    {
      addExtremum(aBestMaxU1, aBestMaxV1, theU2, theV2, aBestMaxSqDist, false, theTol);
    }
  }

  //! Add an extremum to the result, avoiding duplicates.
  void addExtremum(double theU1,
                   double theV1,
                   double theU2,
                   double theV2,
                   double theSqDist,
                   bool   theIsMin,
                   double theTol) const
  {
    const double aU1 = ExtremaSS::NormalizeAngle(theU1);
    const double aU2 = ExtremaSS::NormalizeAngle(theU2);
    const double aV2 = ExtremaSS::NormalizeAngle(theV2);

    const double aTolSq = theTol * theTol;
    for (int i = 0; i < myResult.Extrema.Length(); ++i)
    {
      const ExtremaSS::ExtremumResult& anExisting = myResult.Extrema.Value(i);
      if (std::abs(anExisting.SquareDistance - theSqDist) < aTolSq)
      {
        const double aDU1 = std::abs(anExisting.U1 - aU1);
        const double aDU2 = std::abs(anExisting.U2 - aU2);
        const double aDV1 = std::abs(anExisting.V1 - theV1);
        const double aDV2 = std::abs(anExisting.V2 - aV2);

        if ((aDU1 < theTol || std::abs(aDU1 - 2 * M_PI) < theTol) &&
            (aDU2 < theTol || std::abs(aDU2 - 2 * M_PI) < theTol) && aDV1 < theTol &&
            (aDV2 < theTol || std::abs(aDV2 - 2 * M_PI) < theTol))
        {
          return;
        }
      }
    }

    ExtremaSS::ExtremumResult aResult;

    if (mySwapped)
    {
      aResult.U1     = aU2;
      aResult.V1     = aV2;
      aResult.U2     = aU1;
      aResult.V2     = theV1;
      aResult.Point1 = Value2(aU2, aV2);
      aResult.Point2 = Value1(aU1, theV1);
    }
    else
    {
      aResult.U1     = aU1;
      aResult.V1     = theV1;
      aResult.U2     = aU2;
      aResult.V2     = aV2;
      aResult.Point1 = Value1(aU1, theV1);
      aResult.Point2 = Value2(aU2, aV2);
    }

    aResult.SquareDistance = theSqDist;
    aResult.IsMinimum      = theIsMin;

    myResult.Extrema.Append(aResult);

    if (myResult.Status != ExtremaSS::Status::InfiniteSolutions)
    {
      myResult.Status = ExtremaSS::Status::OK;
    }
  }

private:
  gp_Cone  myCone;  //!< The cone
  gp_Torus myTorus; //!< The torus

  std::optional<ExtremaSS::Domain4D> myDomain; //!< Optional bounded domain
  bool                               mySwapped; //!< True if torus was provided first

  // Cached cone geometry
  gp_Pnt myConeApex;       //!< Cone apex
  gp_Dir myConeAxis;       //!< Cone axis
  gp_Dir myConeXDir;       //!< Cone X direction
  gp_Dir myConeYDir;       //!< Cone Y direction
  double mySemiAngle;      //!< Cone semi-angle
  double mySinSemiAngle;   //!< sin(semi-angle)
  double myCosSemiAngle;   //!< cos(semi-angle)

  // Cached torus geometry
  gp_Pnt myTorusCenter; //!< Torus center
  gp_Dir myTorusAxis;   //!< Torus axis
  gp_Dir myTorusXDir;   //!< Torus X direction
  gp_Dir myTorusYDir;   //!< Torus Y direction
  double myMajorRadius; //!< Torus major radius
  double myMinorRadius; //!< Torus minor radius

  mutable ExtremaSS::Result myResult; //!< Computation result
};

#endif // _ExtremaSS_ConeTorus_HeaderFile
