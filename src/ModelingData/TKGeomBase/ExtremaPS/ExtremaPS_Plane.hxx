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

#ifndef _ExtremaPS_Plane_HeaderFile
#define _ExtremaPS_Plane_HeaderFile

#include <ElSLib.hxx>
#include <ExtremaPS.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <Standard_DefineAlloc.hxx>

#include <optional>

//! @brief Point-Plane extrema computation.
//!
//! Computes the extrema between a 3D point and a plane.
//! For a plane, there is exactly one extremum - the orthogonal projection.
//!
//! The projection point is computed as:
//!   P_proj = P - ((P - P0) . N) * N
//! where P0 is any point on the plane and N is the unit normal.
//!
//! The domain is fixed at construction time for optimal performance with multiple queries.
//!
//! @section API Design
//!
//! Two methods are provided:
//! - `Perform()` - finds interior extrema only (the orthogonal projection)
//! - `PerformWithBoundary()` - includes corner extrema for bounded domains
//!
//! For planes, edges do not provide additional extrema (distance is constant
//! along any line perpendicular to the query point projection direction).
//!
//! @note If the point lies exactly on the plane, the distance is zero
//!       but this is still a valid single extremum (not infinite solutions).
class ExtremaPS_Plane
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with plane geometry (unbounded domain).
  //! @param[in] thePlane the plane to compute extrema for
  explicit ExtremaPS_Plane(const gp_Pln& thePlane)
      : myPlane(thePlane),
        myDomain(std::nullopt)
  {
    initCache();
  }

  //! Constructor with plane geometry and parameter domain.
  //! @param[in] thePlane the plane to compute extrema for
  //! @param[in] theDomain parameter domain (fixed for all queries)
  ExtremaPS_Plane(const gp_Pln& thePlane, const ExtremaPS::Domain2D& theDomain)
      : myPlane(thePlane),
        myDomain(theDomain.IsFinite() ? std::optional<ExtremaPS::Domain2D>(theDomain)
                                      : std::nullopt)
  {
    initCache();
  }

private:
  //! Initialize cached components.
  void initCache()
  {
    // Cache plane components for fast computation
    const gp_Ax3& aPos = myPlane.Position();
    const gp_Pnt& aLoc = aPos.Location();
    myLocX             = aLoc.X();
    myLocY             = aLoc.Y();
    myLocZ             = aLoc.Z();

    const gp_Dir& aNorm = aPos.Direction();
    myNormX             = aNorm.X();
    myNormY             = aNorm.Y();
    myNormZ             = aNorm.Z();

    const gp_Dir& aXDir = aPos.XDirection();
    myXDirX             = aXDir.X();
    myXDirY             = aXDir.Y();
    myXDirZ             = aXDir.Z();

    const gp_Dir& aYDir = aPos.YDirection();
    myYDirX             = aYDir.X();
    myYDirY             = aYDir.Y();
    myYDirZ             = aYDir.Z();
  }

public:
  //! @name Surface Evaluation
  //! @{

  //! Evaluate surface point at given parameters.
  //! Required for template boundary helpers.
  //! @param theU U parameter
  //! @param theV V parameter
  //! @return point on plane at (U, V)
  gp_Pnt Value(double theU, double theV) const
  {
    return gp_Pnt(myLocX + theU * myXDirX + theV * myYDirX,
                  myLocY + theU * myXDirY + theV * myYDirY,
                  myLocZ + theU * myXDirZ + theV * myYDirZ);
  }

  //! @}

  //! @name Extrema Computation
  //! @{

  //! Find interior extrema only (orthogonal projection).
  //! Uses domain specified at construction time.
  //!
  //! For a plane, there is exactly one interior extremum - the projection point.
  //! This is the minimum distance point. Maximum distances only occur at boundaries.
  //!
  //! @param theP query point
  //! @param theTol tolerance for boundary check
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return const reference to result with interior extrema only
  [[nodiscard]] const ExtremaPS::Result& Perform(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const
  {
    myResult.Clear();

    // Max mode: no interior extrema for plane (max is always at boundary)
    if (theMode == ExtremaPS::SearchMode::Max)
    {
      myResult.Status = ExtremaPS::Status::NoSolution;
      return myResult;
    }

    // Compute delta vector from plane location to query point
    const double aDx = theP.X() - myLocX;
    const double aDy = theP.Y() - myLocY;
    const double aDz = theP.Z() - myLocZ;

    // Compute signed distance (dot product with normal) and UV parameters
    const double aSignedDist = aDx * myNormX + aDy * myNormY + aDz * myNormZ;
    const double aU          = aDx * myXDirX + aDy * myXDirY + aDz * myXDirZ;
    const double aV          = aDx * myYDirX + aDy * myYDirY + aDz * myYDirZ;

    // Fast path: unbounded domain - most common case, skip all bounds checks
    if (!myDomain.has_value())
    {
      myResult.Status = ExtremaPS::Status::OK;
      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aU;
      anExt.V              = aV;
      anExt.Point          = gp_Pnt(theP.X() - aSignedDist * myNormX,
                           theP.Y() - aSignedDist * myNormY,
                           theP.Z() - aSignedDist * myNormZ);
      anExt.SquareDistance = aSignedDist * aSignedDist;
      anExt.IsMinimum      = true;
      myResult.Extrema.Append(anExt);
      return myResult;
    }

    // Bounded domain - inline bounds check for speed
    const double aUMin = myDomain->UMin - theTol;
    const double aUMax = myDomain->UMax + theTol;
    const double aVMin = myDomain->VMin - theTol;
    const double aVMax = myDomain->VMax + theTol;

    if (aU < aUMin || aU > aUMax || aV < aVMin || aV > aVMax)
    {
      myResult.Status = ExtremaPS::Status::NoSolution;
      return myResult;
    }

    // Inline clamp for speed
    const double aClampedU =
      (aU < myDomain->UMin) ? myDomain->UMin : ((aU > myDomain->UMax) ? myDomain->UMax : aU);
    const double aClampedV =
      (aV < myDomain->VMin) ? myDomain->VMin : ((aV > myDomain->VMax) ? myDomain->VMax : aV);

    myResult.Status = ExtremaPS::Status::OK;
    ExtremaPS::ExtremumResult anExt;
    anExt.U              = aClampedU;
    anExt.V              = aClampedV;
    anExt.Point          = gp_Pnt(theP.X() - aSignedDist * myNormX,
                         theP.Y() - aSignedDist * myNormY,
                         theP.Z() - aSignedDist * myNormZ);
    anExt.SquareDistance = aSignedDist * aSignedDist;
    anExt.IsMinimum      = true;
    myResult.Extrema.Append(anExt);
    return myResult;
  }

  //! Find extrema including boundary (corners for plane).
  //! Uses domain specified at construction time.
  //!
  //! For a bounded plane, maximum distances occur at corners.
  //! Edge extrema are not computed because distance is linear along edges.
  //!
  //! @param theP query point
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return const reference to result with interior + boundary extrema
  [[nodiscard]] const ExtremaPS::Result& PerformWithBoundary(
    const gp_Pnt&         theP,
    double                theTol,
    ExtremaPS::SearchMode theMode = ExtremaPS::SearchMode::MinMax) const
  {
    // Start with interior extrema
    if (theMode != ExtremaPS::SearchMode::Max)
    {
      (void)Perform(theP, theTol, theMode);
    }
    else
    {
      myResult.Clear();
      myResult.Status = ExtremaPS::Status::OK;
    }

    // Add boundary extrema (corners only for plane)
    // For planes, edge extrema don't add value - distance varies linearly
    if (theMode != ExtremaPS::SearchMode::Min && myDomain.has_value())
    {
      addCornerExtrema(myResult, theP, *myDomain);
    }

    // Update status if we found any extrema
    if (myResult.Extrema.IsEmpty())
    {
      myResult.Status = ExtremaPS::Status::NoSolution;
    }
    else
    {
      myResult.Status = ExtremaPS::Status::OK;
    }

    return myResult;
  }

  //! @}

  //! Returns the plane geometry.
  const gp_Pln& Plane() const { return myPlane; }

  //! Returns true if domain is bounded.
  bool IsBounded() const { return myDomain.has_value(); }

  //! Returns the parameter domain (only valid if IsBounded() is true).
  const ExtremaPS::Domain2D& Domain() const { return *myDomain; }

private:
  //! Add corner extrema (maximum candidates).
  //! For plane, corners are always potential maximum distance points.
  void addCornerExtrema(ExtremaPS::Result&         theResult,
                        const gp_Pnt&              theP,
                        const ExtremaPS::Domain2D& theDomain) const
  {
    double aMaxSqDist = -1.0;
    double aMaxU = 0.0, aMaxV = 0.0;
    gp_Pnt aMaxPt;

    // Check four corners - use cached directions for fast evaluation
    const double aCorners[4][2] = {{theDomain.UMin, theDomain.VMin},
                                   {theDomain.UMax, theDomain.VMin},
                                   {theDomain.UMin, theDomain.VMax},
                                   {theDomain.UMax, theDomain.VMax}};

    for (int i = 0; i < 4; ++i)
    {
      const double aU        = aCorners[i][0];
      const double aV        = aCorners[i][1];
      const gp_Pnt aCornerPt = Value(aU, aV);
      const double aSqDist   = theP.SquareDistance(aCornerPt);

      if (aSqDist > aMaxSqDist)
      {
        aMaxSqDist = aSqDist;
        aMaxU      = aU;
        aMaxV      = aV;
        aMaxPt     = aCornerPt;
      }
    }

    if (aMaxSqDist >= 0.0)
    {
      ExtremaPS::ExtremumResult anExt;
      anExt.U              = aMaxU;
      anExt.V              = aMaxV;
      anExt.Point          = aMaxPt;
      anExt.SquareDistance = aMaxSqDist;
      anExt.IsMinimum      = false;
      theResult.Extrema.Append(anExt);
    }
  }

private:
  gp_Pln                             myPlane;  //!< Plane geometry
  std::optional<ExtremaPS::Domain2D> myDomain; //!< Parameter domain (nullopt for unbounded)
  mutable ExtremaPS::Result          myResult; //!< Reusable result storage

  // Cached components for fast computation
  double myLocX, myLocY, myLocZ;    //!< Plane location
  double myNormX, myNormY, myNormZ; //!< Plane normal
  double myXDirX, myXDirY, myXDirZ; //!< Plane X direction
  double myYDirX, myYDirY, myYDirZ; //!< Plane Y direction
};

#endif // _ExtremaPS_Plane_HeaderFile
