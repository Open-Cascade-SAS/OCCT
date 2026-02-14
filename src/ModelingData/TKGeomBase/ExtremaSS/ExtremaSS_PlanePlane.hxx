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

#ifndef _ExtremaSS_PlanePlane_HeaderFile
#define _ExtremaSS_PlanePlane_HeaderFile

#include <ExtremaSS.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <cmath>
#include <optional>

//! @brief Plane-Plane extrema computation.
//!
//! Computes the extrema between two planar surfaces.
//!
//! Algorithm:
//! 1. Check if planes are parallel by comparing normals
//! 2. If parallel: constant distance everywhere = infinite solutions
//! 3. If not parallel: planes intersect, distance = 0 at intersection line
//!    (no meaningful extrema in the non-parallel case for infinite planes)
//!
//! @note For parallel planes, returns InfiniteSolutions status with the
//!       constant squared distance stored in InfiniteSquareDistance.
//!
//! @note For non-parallel planes, returns NoSolution as there's no
//!       meaningful extremum (they intersect).
class ExtremaSS_PlanePlane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with two plane geometries.
  //! @param[in] thePlane1 first plane
  //! @param[in] thePlane2 second plane
  ExtremaSS_PlanePlane(const gp_Pln& thePlane1, const gp_Pln& thePlane2)
      : myPlane1(thePlane1),
        myPlane2(thePlane2),
        myDomain(std::nullopt)
  {
    initCache();
  }

  //! Constructor with two plane geometries and parameter domains.
  //! @param[in] thePlane1 first plane
  //! @param[in] thePlane2 second plane
  //! @param[in] theDomain parameter domains for both planes
  ExtremaSS_PlanePlane(const gp_Pln&              thePlane1,
                       const gp_Pln&              thePlane2,
                       const ExtremaSS::Domain4D& theDomain)
      : myPlane1(thePlane1),
        myPlane2(thePlane2),
        myDomain(theDomain)
  {
    initCache();
  }

private:
  //! Initialize cached components.
  void initCache()
  {
    // Cache plane 1 components
    const gp_Ax3& aPos1  = myPlane1.Position();
    const gp_Pnt& aOrig1 = aPos1.Location();
    myOrig1X             = aOrig1.X();
    myOrig1Y             = aOrig1.Y();
    myOrig1Z             = aOrig1.Z();

    const gp_Dir& aNorm1 = aPos1.Direction();
    myNorm1X             = aNorm1.X();
    myNorm1Y             = aNorm1.Y();
    myNorm1Z             = aNorm1.Z();

    const gp_Dir& aXDir1 = aPos1.XDirection();
    myXDir1X             = aXDir1.X();
    myXDir1Y             = aXDir1.Y();
    myXDir1Z             = aXDir1.Z();

    const gp_Dir& aYDir1 = aPos1.YDirection();
    myYDir1X             = aYDir1.X();
    myYDir1Y             = aYDir1.Y();
    myYDir1Z             = aYDir1.Z();

    // Cache plane 2 components
    const gp_Ax3& aPos2  = myPlane2.Position();
    const gp_Pnt& aOrig2 = aPos2.Location();
    myOrig2X             = aOrig2.X();
    myOrig2Y             = aOrig2.Y();
    myOrig2Z             = aOrig2.Z();

    const gp_Dir& aNorm2 = aPos2.Direction();
    myNorm2X             = aNorm2.X();
    myNorm2Y             = aNorm2.Y();
    myNorm2Z             = aNorm2.Z();

    const gp_Dir& aXDir2 = aPos2.XDirection();
    myXDir2X             = aXDir2.X();
    myXDir2Y             = aXDir2.Y();
    myXDir2Z             = aXDir2.Z();

    const gp_Dir& aYDir2 = aPos2.YDirection();
    myYDir2X             = aYDir2.X();
    myYDir2Y             = aYDir2.Y();
    myYDir2Z             = aYDir2.Z();

    // Compute dot product of normals to check parallelism
    myNormalDot = myNorm1X * myNorm2X + myNorm1Y * myNorm2Y + myNorm1Z * myNorm2Z;
  }

public:
  //! @name Surface Evaluation
  //! @{

  //! Evaluate plane 1 point at given parameters.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @return point on plane 1 at (U, V)
  gp_Pnt Value1(double theU, double theV) const
  {
    return gp_Pnt(myOrig1X + theU * myXDir1X + theV * myYDir1X,
                  myOrig1Y + theU * myXDir1Y + theV * myYDir1Y,
                  myOrig1Z + theU * myXDir1Z + theV * myYDir1Z);
  }

  //! Evaluate plane 2 point at given parameters.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @return point on plane 2 at (U, V)
  gp_Pnt Value2(double theU, double theV) const
  {
    return gp_Pnt(myOrig2X + theU * myXDir2X + theV * myYDir2X,
                  myOrig2Y + theU * myXDir2Y + theV * myYDir2Y,
                  myOrig2Z + theU * myXDir2Z + theV * myYDir2Z);
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only.
  //!
  //! For two planes:
  //! - Parallel planes: infinite solutions at constant distance
  //! - Non-parallel: planes intersect (no extremum, distance = 0)
  //!
  //! @param theTol tolerance
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result
  [[nodiscard]] const ExtremaSS::Result& Perform(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    (void)theMode; // Not used for plane-plane
    myResult.Clear();

    // Check if planes are parallel
    // |N1 . N2| should be close to 1 for parallel planes
    const double aAbsDot = std::abs(myNormalDot);
    const double aAngleTol = ExtremaSS::THE_ANGULAR_TOLERANCE;

    if (aAbsDot < 1.0 - aAngleTol)
    {
      // Planes are not parallel - they intersect
      // Distance is 0 along the intersection line
      // For infinite planes, there's no meaningful extremum
      myResult.Status = ExtremaSS::Status::NoSolution;
      return myResult;
    }

    // Planes are parallel - compute signed distance
    // Distance = |(O2 - O1) . N1|
    const double aDx = myOrig2X - myOrig1X;
    const double aDy = myOrig2Y - myOrig1Y;
    const double aDz = myOrig2Z - myOrig1Z;

    const double aSignedDist = aDx * myNorm1X + aDy * myNorm1Y + aDz * myNorm1Z;
    const double aDist       = std::abs(aSignedDist);

    // Check if planes are coincident
    if (aDist < theTol)
    {
      // Coincident planes - distance = 0, infinite solutions
      myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
      myResult.InfiniteSquareDistance = 0.0;
      return myResult;
    }

    // Parallel but distinct planes - infinite solutions at constant distance
    myResult.Status                 = ExtremaSS::Status::InfiniteSolutions;
    myResult.InfiniteSquareDistance = aDist * aDist;
    return myResult;
  }

  //! Find extrema including boundary.
  //!
  //! For bounded plane domains, the extrema may occur at corners or edges.
  //!
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result
  [[nodiscard]] const ExtremaSS::Result& PerformWithBoundary(
    double                theTol,
    ExtremaSS::SearchMode theMode = ExtremaSS::SearchMode::MinMax) const
  {
    // Start with interior extrema
    (void)Perform(theTol, theMode);

    // If no bounded domain, return
    if (!myDomain.has_value())
    {
      return myResult;
    }

    // If result is already infinite (parallel planes), sample boundary
    // to provide representative extrema points
    if (myResult.IsInfinite())
    {
      checkBoundaryExtrema(theTol, theMode);
      if (myResult.Extrema.IsEmpty())
      {
        myResult.Status = ExtremaSS::Status::NoSolution;
      }
      else
      {
        myResult.Status = ExtremaSS::Status::OK;
      }
      return myResult;
    }

    // For non-parallel planes (they intersect), check bounded domains
    checkBoundaryExtrema(theTol, theMode);

    if (myResult.Extrema.IsEmpty())
    {
      myResult.Status = ExtremaSS::Status::NoSolution;
    }
    else
    {
      myResult.Status = ExtremaSS::Status::OK;
    }
    return myResult;
  }

private:
  //! Check extrema on domain boundaries.
  void checkBoundaryExtrema(double theTol, ExtremaSS::SearchMode theMode) const
  {
    if (!myDomain.has_value())
    {
      return;
    }

    const ExtremaSS::Domain4D& aDom = myDomain.value();
    constexpr int              aNbSamples = 20;

    // Sample plane 1 boundary edges (Domain1)
    const double aDU1 = (aDom.Domain1.UMax - aDom.Domain1.UMin) / aNbSamples;
    const double aDV1 = (aDom.Domain1.VMax - aDom.Domain1.VMin) / aNbSamples;

    // U edges (VMin and VMax)
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU1 = aDom.Domain1.UMin + i * aDU1;
      checkPlane1PointAgainstPlane2(aU1, aDom.Domain1.VMin, theTol, theMode);
      checkPlane1PointAgainstPlane2(aU1, aDom.Domain1.VMax, theTol, theMode);
    }

    // V edges (UMin and UMax)
    for (int i = 1; i < aNbSamples; ++i)
    {
      const double aV1 = aDom.Domain1.VMin + i * aDV1;
      checkPlane1PointAgainstPlane2(aDom.Domain1.UMin, aV1, theTol, theMode);
      checkPlane1PointAgainstPlane2(aDom.Domain1.UMax, aV1, theTol, theMode);
    }

    // Sample plane 2 boundary edges (Domain2)
    const double aDU2 = (aDom.Domain2.UMax - aDom.Domain2.UMin) / aNbSamples;
    const double aDV2 = (aDom.Domain2.VMax - aDom.Domain2.VMin) / aNbSamples;

    // U edges (VMin and VMax)
    for (int i = 0; i <= aNbSamples; ++i)
    {
      const double aU2 = aDom.Domain2.UMin + i * aDU2;
      checkPlane2PointAgainstPlane1(aU2, aDom.Domain2.VMin, theTol, theMode);
      checkPlane2PointAgainstPlane1(aU2, aDom.Domain2.VMax, theTol, theMode);
    }

    // V edges (UMin and UMax)
    for (int i = 1; i < aNbSamples; ++i)
    {
      const double aV2 = aDom.Domain2.VMin + i * aDV2;
      checkPlane2PointAgainstPlane1(aDom.Domain2.UMin, aV2, theTol, theMode);
      checkPlane2PointAgainstPlane1(aDom.Domain2.UMax, aV2, theTol, theMode);
    }
  }

  //! Check a plane 1 boundary point against plane 2.
  void checkPlane1PointAgainstPlane2(double thePlane1U, double thePlane1V, double theTol,
                                     ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aPt1 = Value1(thePlane1U, thePlane1V);

    // Project onto plane 2
    const double aDistToPlane2 = (aPt1.X() - myOrig2X) * myNorm2X
                               + (aPt1.Y() - myOrig2Y) * myNorm2Y
                               + (aPt1.Z() - myOrig2Z) * myNorm2Z;

    const gp_Pnt aProjPt(aPt1.X() - aDistToPlane2 * myNorm2X,
                         aPt1.Y() - aDistToPlane2 * myNorm2Y,
                         aPt1.Z() - aDistToPlane2 * myNorm2Z);

    // Compute plane 2 UV parameters
    double aPlane2U = (aProjPt.X() - myOrig2X) * myXDir2X
                    + (aProjPt.Y() - myOrig2Y) * myXDir2Y
                    + (aProjPt.Z() - myOrig2Z) * myXDir2Z;
    double aPlane2V = (aProjPt.X() - myOrig2X) * myYDir2X
                    + (aProjPt.Y() - myOrig2Y) * myYDir2Y
                    + (aProjPt.Z() - myOrig2Z) * myYDir2Z;

    const ExtremaSS::Domain4D& aDom = myDomain.value();

    // Clamp to plane 2 domain
    const double aClampedU = std::clamp(aPlane2U, aDom.Domain2.UMin, aDom.Domain2.UMax);
    const double aClampedV = std::clamp(aPlane2V, aDom.Domain2.VMin, aDom.Domain2.VMax);
    const gp_Pnt aPt2 = Value2(aClampedU, aClampedV);
    const double aSqDist = aPt1.SquareDistance(aPt2);

    // Only minimum makes sense for fixed plane 1 point
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      ExtremaSS::AddExtremum(myResult, thePlane1U, thePlane1V, aClampedU, aClampedV, aPt1, aPt2,
                             aSqDist, true, theTol);
    }
  }

  //! Check a plane 2 boundary point against plane 1.
  void checkPlane2PointAgainstPlane1(double thePlane2U, double thePlane2V, double theTol,
                                     ExtremaSS::SearchMode theMode) const
  {
    const gp_Pnt aPt2 = Value2(thePlane2U, thePlane2V);

    // Project onto plane 1
    const double aDistToPlane1 = (aPt2.X() - myOrig1X) * myNorm1X
                               + (aPt2.Y() - myOrig1Y) * myNorm1Y
                               + (aPt2.Z() - myOrig1Z) * myNorm1Z;

    const gp_Pnt aProjPt(aPt2.X() - aDistToPlane1 * myNorm1X,
                         aPt2.Y() - aDistToPlane1 * myNorm1Y,
                         aPt2.Z() - aDistToPlane1 * myNorm1Z);

    // Compute plane 1 UV parameters
    double aPlane1U = (aProjPt.X() - myOrig1X) * myXDir1X
                    + (aProjPt.Y() - myOrig1Y) * myXDir1Y
                    + (aProjPt.Z() - myOrig1Z) * myXDir1Z;
    double aPlane1V = (aProjPt.X() - myOrig1X) * myYDir1X
                    + (aProjPt.Y() - myOrig1Y) * myYDir1Y
                    + (aProjPt.Z() - myOrig1Z) * myYDir1Z;

    const ExtremaSS::Domain4D& aDom = myDomain.value();

    // Clamp to plane 1 domain
    const double aClampedU = std::clamp(aPlane1U, aDom.Domain1.UMin, aDom.Domain1.UMax);
    const double aClampedV = std::clamp(aPlane1V, aDom.Domain1.VMin, aDom.Domain1.VMax);
    const gp_Pnt aPt1 = Value1(aClampedU, aClampedV);
    const double aSqDist = aPt2.SquareDistance(aPt1);

    // Only minimum makes sense for fixed plane 2 point
    if (theMode != ExtremaSS::SearchMode::Max)
    {
      ExtremaSS::AddExtremum(myResult, aClampedU, aClampedV, thePlane2U, thePlane2V, aPt1, aPt2,
                             aSqDist, true, theTol);
    }
  }

public:
  //! @}

  //! Returns the first plane geometry.
  const gp_Pln& Plane1() const { return myPlane1; }

  //! Returns the second plane geometry.
  const gp_Pln& Plane2() const { return myPlane2; }

  //! Returns true if planes are parallel.
  bool IsParallel() const
  {
    return std::abs(myNormalDot) >= 1.0 - ExtremaSS::THE_ANGULAR_TOLERANCE;
  }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

private:
  gp_Pln                               myPlane1; //!< First plane geometry
  gp_Pln                               myPlane2; //!< Second plane geometry
  std::optional<ExtremaSS::Domain4D> myDomain; //!< Parameter domain
  mutable ExtremaSS::Result            myResult; //!< Reusable result storage

  // Cached plane 1 components
  double myOrig1X, myOrig1Y, myOrig1Z;
  double myNorm1X, myNorm1Y, myNorm1Z;
  double myXDir1X, myXDir1Y, myXDir1Z;
  double myYDir1X, myYDir1Y, myYDir1Z;

  // Cached plane 2 components
  double myOrig2X, myOrig2Y, myOrig2Z;
  double myNorm2X, myNorm2Y, myNorm2Z;
  double myXDir2X, myXDir2Y, myXDir2Z;
  double myYDir2X, myYDir2Y, myYDir2Z;

  // Precomputed parallelism check
  double myNormalDot; //!< N1 . N2
};

#endif // _ExtremaSS_PlanePlane_HeaderFile
