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

//! @brief Point-Plane extrema computation.
//!
//! Computes the extrema between a 3D point and a plane.
//! For a plane, there is exactly one extremum - the orthogonal projection.
//!
//! The projection point is computed as:
//!   P_proj = P - ((P - P0) . N) * N
//! where P0 is any point on the plane and N is the unit normal.
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

  //! Constructor with plane geometry.
  //! @param thePlane the plane to compute extrema for
  explicit ExtremaPS_Plane(const gp_Pln& thePlane)
      : myPlane(thePlane)
  {
    // Cache plane components for fast computation
    const gp_Ax3& aPos = thePlane.Position();
    const gp_Pnt& aLoc = aPos.Location();
    myLocX = aLoc.X();
    myLocY = aLoc.Y();
    myLocZ = aLoc.Z();

    const gp_Dir& aNorm = aPos.Direction();
    myNormX = aNorm.X();
    myNormY = aNorm.Y();
    myNormZ = aNorm.Z();

    const gp_Dir& aXDir = aPos.XDirection();
    myXDirX = aXDir.X();
    myXDirY = aXDir.Y();
    myXDirZ = aXDir.Z();

    const gp_Dir& aYDir = aPos.YDirection();
    myYDirX = aYDir.X();
    myYDirY = aYDir.Y();
    myYDirZ = aYDir.Z();
  }

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
  //!
  //! For a plane, there is exactly one interior extremum - the projection point.
  //! This is the minimum distance point. Maximum distances only occur at boundaries.
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain
  //! @param theTol tolerance for boundary check
  //! @param theMode search mode (MinMax, Min, Max)
  //! @return result with interior extrema only
  ExtremaPS::Result Perform(const gp_Pnt&                theP,
                            const ExtremaPS::Domain2D&   theDomain,
                            double                       theTol,
                            ExtremaPS::SearchMode        theMode = ExtremaPS::SearchMode::MinMax) const
  {
    ExtremaPS::Result aResult;

    // Max mode: no interior extrema for plane (max is always at boundary)
    if (theMode == ExtremaPS::SearchMode::Max)
    {
      aResult.Status = ExtremaPS::Status::NoSolution;
      return aResult;
    }

    // Compute signed distance and UV parameters
    const double aDx = theP.X() - myLocX;
    const double aDy = theP.Y() - myLocY;
    const double aDz = theP.Z() - myLocZ;

    const double aSignedDist = aDx * myNormX + aDy * myNormY + aDz * myNormZ;
    const double aU = aDx * myXDirX + aDy * myXDirY + aDz * myXDirZ;
    const double aV = aDx * myYDirX + aDy * myYDirY + aDz * myYDirZ;

    // Fast path: large domain (effectively unbounded) - skip bounds check
    if (theDomain.IsLarge())
    {
      aResult.Extrema.SetValue(0, ExtremaPS::ExtremumResult{
        aU, aV,
        gp_Pnt(theP.X() - aSignedDist * myNormX,
               theP.Y() - aSignedDist * myNormY,
               theP.Z() - aSignedDist * myNormZ),
        aSignedDist * aSignedDist,
        true
      });
      aResult.Status = ExtremaPS::Status::OK;
      return aResult;
    }

    // Bounded domain - check if projection is within bounds
    if (!theDomain.Contains(aU, aV, theTol))
    {
      aResult.Status = ExtremaPS::Status::NoSolution;
      return aResult;
    }

    // Clamp to domain bounds
    double aClampedU = aU;
    double aClampedV = aV;
    theDomain.Clamp(aClampedU, aClampedV);

    aResult.Extrema.SetValue(0, ExtremaPS::ExtremumResult{
      aClampedU, aClampedV,
      gp_Pnt(theP.X() - aSignedDist * myNormX,
             theP.Y() - aSignedDist * myNormY,
             theP.Z() - aSignedDist * myNormZ),
      aSignedDist * aSignedDist,
      true
    });
    aResult.Status = ExtremaPS::Status::OK;
    return aResult;
  }

  //! Find extrema including boundary (corners for plane).
  //!
  //! For a bounded plane, maximum distances occur at corners.
  //! Edge extrema are not computed because distance is linear along edges.
  //!
  //! @param theP query point
  //! @param theDomain 2D parameter domain
  //! @param theTol tolerance
  //! @param theMode search mode
  //! @return result with interior + boundary extrema
  ExtremaPS::Result PerformWithBoundary(const gp_Pnt&                theP,
                                        const ExtremaPS::Domain2D&   theDomain,
                                        double                       theTol,
                                        ExtremaPS::SearchMode        theMode = ExtremaPS::SearchMode::MinMax) const
  {
    // Start with interior extrema
    ExtremaPS::Result aResult;

    if (theMode != ExtremaPS::SearchMode::Max)
    {
      aResult = Perform(theP, theDomain, theTol, theMode);
    }
    else
    {
      aResult.Status = ExtremaPS::Status::OK;
    }

    // Add boundary extrema (corners only for plane)
    // For planes, edge extrema don't add value - distance varies linearly
    if (theMode != ExtremaPS::SearchMode::Min && theDomain.IsFinite())
    {
      addCornerExtrema(aResult, theP, theDomain);
    }

    // Update status if we found any extrema
    if (aResult.Extrema.IsEmpty())
    {
      aResult.Status = ExtremaPS::Status::NoSolution;
    }
    else
    {
      aResult.Status = ExtremaPS::Status::OK;
    }

    return aResult;
  }

  //! @}

  //! Returns the plane geometry.
  const gp_Pln& Plane() const { return myPlane; }

private:
  //! Add corner extrema (maximum candidates).
  //! For plane, corners are always potential maximum distance points.
  void addCornerExtrema(ExtremaPS::Result&           theResult,
                        const gp_Pnt&                theP,
                        const ExtremaPS::Domain2D&   theDomain) const
  {
    double aMaxSqDist = -1.0;
    double aMaxU = 0.0, aMaxV = 0.0;
    gp_Pnt aMaxPt;

    // Check four corners - use cached directions for fast evaluation
    const double aCorners[4][2] = {
      {theDomain.UMin, theDomain.VMin},
      {theDomain.UMax, theDomain.VMin},
      {theDomain.UMin, theDomain.VMax},
      {theDomain.UMax, theDomain.VMax}
    };

    for (int i = 0; i < 4; ++i)
    {
      const double aU = aCorners[i][0];
      const double aV = aCorners[i][1];
      const gp_Pnt aCornerPt = Value(aU, aV);
      const double aSqDist = theP.SquareDistance(aCornerPt);

      if (aSqDist > aMaxSqDist)
      {
        aMaxSqDist = aSqDist;
        aMaxU = aU;
        aMaxV = aV;
        aMaxPt = aCornerPt;
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
  gp_Pln myPlane;  //!< Plane geometry

  // Cached components for fast computation
  double myLocX, myLocY, myLocZ;     //!< Plane location
  double myNormX, myNormY, myNormZ;  //!< Plane normal
  double myXDirX, myXDirY, myXDirZ;  //!< Plane X direction
  double myYDirX, myYDirY, myYDirZ;  //!< Plane Y direction
};

#endif // _ExtremaPS_Plane_HeaderFile
